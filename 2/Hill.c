#define _GNU_SOURCE // For `getline()`!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef float vector[3];
typedef float matrix[3][3];

int main(int const p_argCount, char const *const p_argValues[]) {
	puts("Write:");
	char *uin = NULL;
	ssize_t uinlen = 0;
	ssize_t const len = getline(&uin, &uinlen, stdin);

	printf("You wrote \"%s\"!\n", uin);

	exit(EXIT_SUCCESS);
}
