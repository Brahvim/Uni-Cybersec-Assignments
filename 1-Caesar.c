#include <stdio.h>
#include <stdlib.h>

char caesar(char const p_plain, char const p_key) {
	return p_plain + p_key;
}

int main(int const p_argc, char *const *const p_argv) {
	puts("Please enter plaintext to convert:");

	char *uin = calloc(1, sizeof(char));
	size_t uinlen = 0;
	size_t uincap = 1;

	while (uin[uinlen] != '\n') {

		if (uincap <= uinlen) {

			uin = realloc(uin, uincap *= 2);

		}

		scanf("%c", uin + uinlen);

	}

	uin[uinlen] = '\0';

	printf("You entered: %s", uin);

	

	free(uin);
	exit(EXIT_SUCCESS);
}

