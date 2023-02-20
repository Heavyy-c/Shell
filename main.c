#include <stdio.h>
#include <stdlib.h>
#include "words.h"
#include "input.h"
#include "input_handler.h"

enum read_states { STR_EOS = '\0', CHAR_EOL = '\n' };
#define SHELL_STR "->> "
#define UNMATCHED_QUOTES_ERR "UNMATCHED QUOTES, ERR\n"

void print_words(struct word_list list)
{
	struct word_item *current = list.first;
	for(; current; current = current->next)
	{
		printf("[");
		printf(current->word);
		printf("]\n");
	}
}

int main()
{
	struct input_list input_l;
	struct word_list word_l;
	char *string = NULL;

	input_list_init(&input_l);
	word_list_init(&word_l);

	while(1)
	{
		printf(SHELL_STR);
		fflush(stdout);

		input_list_read(&input_l);
		input_assemble(input_l, &string);
		input_list_free(&input_l);

		if(*string == STR_EOS) /* || NULL */
			break;
		if(!handler_input_correct(string))
		{
			printf(UNMATCHED_QUOTES_ERR);
			goto memory_free;
		}
		handler_split_string(string, &word_l);
		command_assemble_commands(word_l, &commands);
		command_execute(commands);

		print_words(word_l);

memory_free:
		input_list_free(&input_l);
		input_string_free(&string);
		word_list_free(&word_l);
		command_list_free(&commands);
	}
	return 0;
}
