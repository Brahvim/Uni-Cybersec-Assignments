#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>

#define unlikely(p_condition)	__builtin_expect(p_condition, 0)
#define likely(p_condition)		__builtin_expect(p_condition, 1)

#pragma region "Alphabet".
#define ABET_DTYPE uint8_t
typedef ABET_DTYPE abet_t;

#define ABET_DTYPE_SIZE	 		sizeof(abet_t)
#define ABET_DTYPE_CHARS	 	8
#define ABET_DTYPE_BYTES	 	8 * ABET_DTYPE_CHARS
#define ABET_DTYPE_BITS	 		8 * ABET_DTYPE_BYTES
#pragma endregion

typedef uint8_t matrix_t[ABET_DTYPE_SIZE][ABET_DTYPE_SIZE];
typedef uint8_t vector_t[ABET_DTYPE_SIZE];

// From my library "SML", nowadays part of the source code for "RPG-1",
// both of which are on GitHub!:
uint8_t* matMul(
	uint8_t *const p_destination,	// `p_leftColumnCount	x	p_rightColumnCount`
	uint8_t const *const p_right,	// `p_rightRowCount		x	p_rightColumnCount`
	uint8_t const *const p_left,	// `p_leftColumnCount	x	p_rightRowCount`
	uint8_t const p_rightRowCount,
	uint8_t const p_leftColumnCount,
	uint8_t const p_rightColumnCount
) {
	for (uint8_t i = 0; i < p_leftColumnCount; ++i) {

		for (uint8_t j = 0; j < p_rightColumnCount; ++j) {

			uint8_t sum = 0;
			for (uint8_t k = 0; k < p_rightRowCount; ++k) {

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

// uint16_t bytesSwap(uint16_t const p_value) {
// 	// See [ https://stackoverflow.com/a/36850166 ].
// 	uint16_t result;
// 	result = p_value >> 8;
// 	result += (p_value & 0xFF) << 8;
// 	return result;
// }

// See [ https://stackoverflow.com/a/8979034 ].
#define isBigEndian() (!*((char*) ((int[]) { 1 })))

void byteswap(void *const p_value, size_t const p_bytes) {
	uint8_t *const val = p_value;
	for (size_t i = 0; i < p_bytes / 2; ++i) {

		uint8_t const second = val[p_bytes - 1 - i];
		uint8_t const first = val[i];

		val[p_bytes - 1 - i] = first;
		val[i] = second;

	}
}

int main(int const p_argCount, char const *const p_argValues[]) {
	printf("Write a string to encrypt: ");
	struct ResultReadline const uin = readline();

jmpKeyFile:
	printf("Write the name of a file to read a key from / store the key in: ");
	struct ResultReadline const keyPath = readline();

	FILE *keyFile = fopen(keyPath.str, "wb+");

	if (!keyFile) {

		puts("File unavailable...");
		// fclose(keyFile);
		free(keyPath.str);
		goto jmpKeyFile;

	}

	// Ruins all yo' endianness and whatnot...!:
	// fwrite(key, sizeof(key[0]), sizeof(key), keyFile);

	bool keyGen = false;
	matrix_t key = { 0 };
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

		// fclose(keyFile);
		// keyFile = freopen(keyPath.str, "w", keyFile);
		//
		// if (!keyFile) { // Perhaps open this handle earlier so this check's up there?
		//
		// 	puts("Cannot write to key file...");
		// 	// exit(EXIT_FAILURE);
		// 	free(keyPath.str);
		// 	goto jmpKeyFile;
		//
		// }

		puts("Generating key...");

		for (size_t i = 0; i < ABET_DTYPE_CHARS; i++) {

			for (size_t j = 0; j < ABET_DTYPE_CHARS; j++) {

				int const r = rand() + 1;
				key[i][j] = r;

			}

		}

		puts("Writing key to file...");

		for (size_t i = 0; i < ABET_DTYPE_CHARS; i++) {

			for (size_t j = 0; j < ABET_DTYPE_CHARS; j++) {

				abet_t const n = key[i][j];
				fwrite(&n, sizeof(n), 1, keyFile);
				// STILL ruins all yo' endianness and whatnot...!

			}

		}

	}
	else {

		puts("Reading key from file...");

		for (uint8_t i = 0; i < ABET_DTYPE_CHARS; i++) {

			for (uint8_t j = 0; j < ABET_DTYPE_CHARS; j++) {

				fread(
					&(key[i][j]),
					ABET_DTYPE_BYTES,
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
