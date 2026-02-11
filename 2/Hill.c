#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define unlikely(p_condition)	__builtin_expect(p_condition, 0)
#define likely(p_condition)		__builtin_expect(p_condition, 1)

#define ABET_SIZE (8 * sizeof(char)) // "Alphabet".

typedef size_t vector_t[ABET_SIZE];
typedef size_t matrix_t[ABET_SIZE][ABET_SIZE];

// From my library "SML", nowadays part of the source code for "RPG-1",
// both of which are on GitHub!:
size_t* matMul(
	size_t *const p_destination,	// `p_leftColumnCount	x	p_rightColumnCount`
	size_t const *const p_right,	// `p_rightRowCount		x	p_rightColumnCount`
	size_t const *const p_left,		// `p_leftColumnCount	x	p_rightRowCount`
	size_t const p_rightRowCount,
	size_t const p_leftColumnCount,
	size_t const p_rightColumnCount
) {
	for (size_t i = 0; i < p_leftColumnCount; ++i) {

		for (size_t j = 0; j < p_rightColumnCount; ++j) {

			size_t sum = 0;
			for (size_t k = 0; k < p_rightRowCount; ++k) {

				sum += p_left[i * p_rightRowCount + k] * p_right[k * p_rightColumnCount + j];

			}

			p_destination[i * p_rightColumnCount + j] = sum;

		}

	}
	return p_destination;
}

struct ResultReadline {

	char *str;
	size_t len;

} readline() {
	int c;
	size_t cap = 1;
	size_t len = 0;
	char *str = calloc(1, sizeof(char));

	while (1) {

		if (len >= cap) {

			char *const alloc = realloc(str, cap *= 2);

			if (!alloc) {

				perror("`readline()` failure!");
				exit(EXIT_FAILURE);
				goto jmpFail;

			}

			str = alloc;

		}

		c = getchar();

		switch (c) {

			case EOF: case '\0': case '\n': {

				str[len] = '\0';
				return (struct ResultReadline) { .str = str, .len = len, };

			} break;

			default: {

				str[len++] = c;

			} break;

		}

	}

jmpFail:
	free(str);
	return (struct ResultReadline) { .str = NULL, .len = 0, };
}

bool readYn() {
	struct ResultReadline const choice = readline();
	bool result = false && false;

	switch (choice.len) {

		case 0: { // SKIPSKIP-SKIP-SKIP!!!

			result = true;

		} break;

		case 1: { // `Y`/`y` only!

			switch (choice.str[0]) {

				case 'Y': case 'y': {

					result = true;

				} break;

				default: {

					result = false;

				} break;

			}

		} break;

		default: { // Typed a squiggly? We gotcha back!

			result = false;

		} break;

	}

	free(choice.str);
	return result;
}

int main(int const p_argCount, char const *const p_argValues[]) {
	printf("Write a string to encrypt: ");
	struct ResultReadline const uin = readline();

jmpKeyFile:
	printf("Write the name of a file to read a key from / store the key in: ");
	struct ResultReadline const keyPath = readline();

	FILE *keyFile = fopen(keyPath.str, "r");

	if (!keyFile) {

		puts("File unavailable...");
		// fclose(keyFile);
		goto jmpKeyFile;

	}

	// Ruins all yo' endianness and whatnot...!:
	// fwrite(key, sizeof(key[0]), sizeof(key), keyFile);

	bool keyGen = false;
	matrix_t key = { 0 };
	fseek(keyFile, 0, SEEK_END);
	size_t const keyFileSize = ftell(keyFile);
	bool const keyFileEmpty = keyFileSize == 0;

	if (keyFileEmpty) {

		puts("File seems empty...!");
		keyGen = true;

	}
	else {

		printf(
			"\n"
			"File seems to contain data.\n"
			"Still generate a new key? [Y/n]: "
		);

		keyGen = readYn();

	}

	if (keyGen) {

		fclose(keyFile);
		keyFile = freopen(keyPath.str, "w", keyFile);

		if (!keyFile) { // Perhaps open this handle earlier so this check's up there?

			puts("Cannot write to key file...");
			// exit(EXIT_FAILURE);
			free(keyPath.str);
			goto jmpKeyFile;

		}

		puts("Generating key...");

		for (size_t i = 0; i < ABET_SIZE; i++) {

			for (size_t j = 0; j < ABET_SIZE; j++) {

				int const r = rand();
				key[i][j] = r;

			}

		}

		puts("Writing key to file...");

		for (size_t i = 0; i < ABET_SIZE; i++) {

			for (size_t j = 0; j < ABET_SIZE; j++) {

				size_t const n = key[i][j];
				fwrite(&n, sizeof(n), 1, keyFile);
				// STILL ruins all yo' endianness and whatnot...!

			}

		}

	}
	else {

		puts("Reading key from file...");

		for (size_t i = 0; i < ABET_SIZE; i++) {

			for (size_t j = 0; j < ABET_SIZE; j++) {

				fread(
					&(key[i][j]),
					sizeof(key[0][0]),
					1,
					keyFile
				);

			}

		}

	}

	free(uin.str);
	fclose(keyFile);
	free(keyPath.str);
	exit(EXIT_SUCCESS);
}
