#include <stdio.h>
#include <stdlib.h>
#include "string_handler.h"

enum read_states { RSTATE_EOF, RSTATE_GOT_STR };
#define SHELL_SYMBOL "->>"
#define UNMACH_QUOTES_ERR "UNMATCHED QUOTES, ERR\n"

void print_words(struct word_item *words)
{
	for(; words; words = words->next)
	{
		printf("[");
		printf(words->word);
		printf("]\n");
	}
}

int main()
{
	struct input_item *input_list;
	struct word_item *words;
	int is_eof;
	char *string;
	input_list_init(&input_list);
	while(1)
	{
		printf(SHELL_SYMBOL);
		fflush(stdout);
		is_eof = input_read(&input_list);
		if(is_eof)
			break;
		string = input_make_string(input_list);
		if(!input_correct(string))
		{
			printf(UNMACH_QUOTES_ERR);
			continue;
		}
		input_split_string(&words, string);

		print_words(words);

		free(string);
		input_list_free(input_list);
		input_words_free(words);
	}
	return 0;
}
