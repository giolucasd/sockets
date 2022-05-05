// file: integer-input.h
#ifndef INTEGER_INPUT_H
#define INTEGER_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_DIGITS 12

/**
 * This function removes surplus characters from the input buffer.
 * Otherwise, if more than the permitted number of characters have been entered during the
 * call to fgets(), the surplus characters (after MAX_DIGITS chars) remain in the input buffer
 * and will be wrongly accepted as input on the next iteration of the loop.
 * */
static inline void clear_input_buffer()
{
	char c = 0;
	// Loop over input buffer and consume chars until buffer is empty
	while ((c = getchar()) != '\n' && c != EOF)
		;
}

void get_int_from_stdin(u_int32_t *input_integer)
{
	clear_input_buffer();

	char *input_buffer = (char *)malloc(sizeof(char) * MAX_DIGITS);
	memset(input_buffer, 0, MAX_DIGITS);
	char *input = NULL;
	while (input == NULL)
	{
		// Note that fgets returns input_buffer on success.
		// This becomes important when freeing - free either `input` or
		// `input_buffer` to avoid an attempted double-free error.
		input = fgets(input_buffer, MAX_DIGITS, stdin);

		// If fgets() receives less than MAX_DIGITS, the last char in the array is '\n'.
		// Therefore if the last char is not '\n', too many characters were entered.
		if (input_buffer[strlen(input_buffer) - 1] != '\n')
		{
			fprintf(stderr, "[ERRO]: Muitos caracteres: número máximo é %d chars.\n", MAX_DIGITS);
			clear_input_buffer();
			input = NULL;
			continue;
		}

		// Check that the input can be intepreted as an integer
		// Convert to integer using `strtol()`
		errno = 0;
		char *endptr = NULL;
		*input_integer = (u_int32_t)strtol(input, &endptr, 10);

		// If an integer was not found, endptr remains set to input
		if (input == endptr)
		{
			// Remove trailing newline by adding NUL at the index of the
			// terminating '\n' character. See man strcspn - this function
			// gets the length of a prefix substring.
			input[strcspn(input, "\n")] = 0;
			printf("Entrada inválida: inteiro não encontrado em %s.\n", input);
			input = NULL;
		}
		if (errno != 0)
		{
			fprintf(stderr, "[ERRO]: Entrada fornecida não é um inteiro.\n");
			input = NULL;
		}
	}
	free(input_buffer);
}

void get_str_from_stdin(char *input_string, int max_size)
{
	char *input_buffer = (char *)malloc(sizeof(char) * max_size);
	memset(input_buffer, 0, max_size);
	char *input = NULL;
	while (input == NULL)
	{
		// Note that fgets returns input_buffer on success.
		// This becomes important when freeing - free either `input` or
		// `input_buffer` to avoid an attempted double-free error.
		input = fgets(input_buffer, max_size, stdin);

		for (int i=0; i < max_size; i++)
			if (input[i] == '\n')
				input[i] = 0;

		memcpy(input_string, input, max_size);

	}
	free(input_buffer);
}

#endif