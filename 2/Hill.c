#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>

#define unlikely(p_condition)	__builtin_expect(p_condition, 0)
#define likely(p_condition)		__builtin_expect(p_condition, 1)

#pragma region "Alphabet".
#define ABET_DTYPE		int
typedef ABET_DTYPE		abet_t;
#define ABET_BLK_SIZE	3
#pragma endregion

typedef abet_t mat_t[ABET_BLK_SIZE][ABET_BLK_SIZE];
typedef abet_t vec_t[ABET_BLK_SIZE];

// From my library "SML", nowadays part of the source code for "RPG-1",
// both of which are on GitHub!:
abet_t* matMul(
	abet_t *p_destination,	// `p_leftColumnCount	x	p_rightColumnCount`
	abet_t *p_right,	// `p_rightRowCount		x	p_rightColumnCount`
	abet_t *p_left,	// `p_leftColumnCount	x	p_rightRowCount`
	size_t const p_rightRowCount,
	size_t const p_leftColumnCount,
	size_t const p_rightColumnCount
) {
	for (size_t i = 0; i < p_leftColumnCount; ++i) {

		for (size_t j = 0; j < p_rightColumnCount; ++j) {

			abet_t sum = 0;
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

	while (true) {

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

// See [ https://stackoverflow.com/a/8979034 ].
#define isBigEndian() (!*((char*) ((int[]) { 1 })))

// See [ https://stackoverflow.com/a/36850166 ].
void byteSwap(void *const p_value, size_t const p_bytes) {
	uint8_t *const val = p_value;
	for (size_t i = 0; i < p_bytes / 2; ++i) {

		uint8_t const second = val[p_bytes - 1 - i];
		uint8_t const first = val[i];

		val[p_bytes - 1 - i] = first;
		val[i] = second;

	}
}

int main(int const p_argCount, char const *const p_argValues[]) {
	printf("Write the text to deal with: ");
	struct ResultReadline const uin = readline();

jmpKeyFile:
	printf("Write the path to a file to read a key from / store the key in: ");
	struct ResultReadline const keyPath = readline();
	FILE *keyFile = fopen(keyPath.str, "rb");

	if (!keyFile) {

		puts("Creating a new file to store the key in...");
		keyFile = fopen(keyPath.str, "w");

		if (!keyFile) {

			puts("There was a problem creating the file...");
			goto jmpKeyFile;

		}
		else {

			// keyFile = freopen(keyPath.str, "rb", keyFile);

		}

	}

	// Ruins all yo' endianness and whatnot...!:
	// fwrite(key, sizeof(key[0]), sizeof(key), keyFile);


	bool keyGen = false;
	mat_t keyMat = { 0 };
	fseek(keyFile, 0, SEEK_END);
	long const keyFileSize = ftell(keyFile);
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

		puts("Generating key...");
		srand(time(NULL));

		for (size_t i = 0; i < ABET_BLK_SIZE; i++) {

			for (size_t j = 0; j < ABET_BLK_SIZE; j++) {

				int const r = 1 + (rand() % RAND_MAX);
				keyMat[i][j] = r;

			}

		}

		puts("Writing key to file...");
		keyFile = freopen(keyPath.str, "wb", keyFile);

		for (size_t i = 0; i < ABET_BLK_SIZE; i++) {

			for (size_t j = 0; j < ABET_BLK_SIZE; j++) {

				abet_t const n = keyMat[i][j];
				fwrite(&n, sizeof(n), 1, keyFile);
				// STILL ruins all yo' endianness and whatnot...!

			}

		}

	}
	else {

		puts("Reading key from file...");

		for (size_t i = 0; i < ABET_BLK_SIZE; i++) {

			for (size_t j = 0; j < ABET_BLK_SIZE; j++) {

				fread(
					&(keyMat[i][j]),
					sizeof(abet_t),
					1,
					keyFile
				);

			}

		}

	}

	printf("Perform encryption? [Y/n]: ");
	bool const taskIsEncrypt = readYn();

	if (taskIsEncrypt) {

		abet_t *const ciph = calloc(uin.len, sizeof(abet_t));

		for (size_t i = 0; i < uin.len; i += ABET_BLK_SIZE) { // DEAL with the BLKs for me!

			vec_t in = { 0 };
			vec_t out = { 0 };

			for (size_t j = 0; j < ABET_BLK_SIZE; j++) {

				char const c = *(uin.str + i + j);
				bool const cond = j < i;

				in[j] =
					cond
					?
					EOF
					:
					c
					;

				// in[j] =
				// 	j < i
				// 	? EOF
				// 	: *(uin.str + i);

			}

			matMul(out, in, (abet_t*) keyMat, ABET_BLK_SIZE, ABET_BLK_SIZE, 1);

			for (size_t j = 0; j < ABET_BLK_SIZE; j++) {

				ciph[i + j] = out[i];

			}

		}

		printf("Ciphertext: `");
		for (size_t i = 0; i < uin.len; i++) {

			putchar(ciph[i]);

		}

		puts("`.");

		printf("Ciphertext `int`s: `[ ");
		for (size_t i = 0; i < uin.len - 1; i++) {

			printf("%d, ", (int) ciph[i]);

		}

		printf("%d", ciph[uin.len - 1]);
		puts(" ]`.");

		free(ciph);

	}
	else {



	}

	free(uin.str);
	fclose(keyFile);
	free(keyPath.str);
	exit(EXIT_SUCCESS);
}
