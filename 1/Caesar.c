#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STR2(x)	STR(x)
#define STR(x) 	#x
#define SIZE 	99

char caesar(char const p_plain, char const p_key) {
	return p_plain + p_key;
}

int main(int const p_argc, char *const *const p_argv) {
	puts("Please enter under " STR2(SIZE) " characters of plaintext:");
	char cipher[SIZE];
	char plain[SIZE];

	fgets(plain, SIZE, stdin);
	size_t const len = strlen(plain);

	printf("Key (`int`)?: ");
	int key;
	scanf("%d", &key);

	for (size_t i = 0; i < len - 1; ++i) {

		cipher[i] = caesar(*(plain + i), key);

	}

	cipher[len - 1] = '\0';

	printf("Ciphertext: %s\n", cipher);
	exit(EXIT_SUCCESS);

}
