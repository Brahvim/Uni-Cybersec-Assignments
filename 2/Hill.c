#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ALPHABET_SIZE (8 * sizeof(char))

typedef size_t vector_t[ALPHABET_SIZE];
typedef size_t matrix_t[ALPHABET_SIZE][ALPHABET_SIZE];

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

			size_t sum = 0.0f;
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
	printf("Write the name of a file to store the key in: ");
	struct ResultReadline const keyPath = readline();

	FILE *keyFile = fopen(keyPath.str, "wb+");

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

	if (keyFileSize == 0) {
jmpKeygen:

		puts("Generating key...");

		for (size_t i = 0; i < ALPHABET_SIZE; i++) {

			for (size_t j = 0; j < ALPHABET_SIZE; i++) {

				int const r = rand();
				key[i][j] = r;

			}

		}

		puts("Writing key to file...");

		for (size_t i = 0; i < ALPHABET_SIZE; i++) {

			for (size_t j = 0; j < ALPHABET_SIZE; i++) {

				size_t const n = key[i][j];
				fwrite(&n, sizeof(n), 1, keyFile);

			}

		}

	}
	else {

		printf(
			"\n"
			"File seems to contain data.\n"
			"Still generate a new key? [Y/n]: "
		);

		if (readYn()) {

			goto jmpKeygen;

		}
		else {



		}

	}

	free(uin.str);
	fclose(keyFile);
	free(keyPath.str);
	exit(EXIT_SUCCESS);
}
