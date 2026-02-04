#include <stdio.h>
#include <stdlib.h>

#define STR2(x)	STR(x)
#define STR(x) 	#x
#define SIZE 	99

char caesar(char const p_plain, char const p_key) {
	return p_plain + p_key;
}

int main(int const p_argc, char *const *const p_argv) {
	puts("Please enter under " STR2(SIZE) " characters of plaintext:");
	char plain[SIZE];

	int const len = scanf("%" STR2(SIZE) "c", plain);
	plain[SIZE - 1] = '\0';

	printf("%d", len);

	char cipher[SIZE];

	for (int i = 0; i < SIZE - 1; ++i) {

		cipher[i] = caesar(plain[i], 0);

	}

	printf("Ciphertext: `%s`.\n", cipher);
	exit(EXIT_SUCCESS);
}

