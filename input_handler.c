#include <stdio.h>
#include <stdlib.h>
#include "input_handler.h"
#include "symbols.h"
#include "words.h"
 
enum {
	CHAR_BACKSLASH = '\\',
	CHAR_QUOTE = '\'',
	CHAR_DOUBLE_QUOTE = '"',
	CHAR_TABULATION = '\t',
	CHAR_SPACE = ' ',
	CHAR_EOL = '\n'
};

static int is_backslash(char c)
{
	return (c == CHAR_BACKSLASH);
}

static int is_quote(char c)
{
	return (c == CHAR_DOUBLE_QUOTE);
}

static int is_separator(char c)
{
	switch(c)
	{
		case CHAR_TABULATION:
		case CHAR_SPACE:
			return 1;
		default:
			return 0;
	}
}

static int is_text(char c)
{
	return !is_quote(c) && (c != CHAR_EOL) && !is_separator(c);
}

int handler_input_correct(const char *string)
{
	int quote_open;
	char last = (string == NULL) ? ' ' : (*string);
	char quote = ' ';
	for(quote_open = 0; string && *string; last = *string, string++)
	{
		if(is_backslash(last))
			continue;
		if(is_quote(*string) && !quote_open)
		{
			quote_open = 1;
			quote = *string;
		}
		else if(quote_open && (*string) == quote)
		{
			quote_open = 0;
		}
	}
	return !quote_open;
}

static int first_char_pos(const char *string)
{
	int pos;
	for(pos = 0; string && *string; string++, pos++)
		if(!is_separator(*string))
			break;
	return pos;
}

static int get_word_symbols(const char *string, struct symbol_list *symbols)
{
	char last = ' ';
	int len;
	int reading_quote = 0;
	for(len = 0; string && *string; last = *string, string++, len++)
	{
		if(is_backslash(last))
		{
			symbol_list_append(symbols, *string);
			continue;
		}
		if(is_separator(*string) && !reading_quote)
			break;
		if(is_quote(*string))
		{
			reading_quote = !reading_quote;
			continue;
		}
		if((reading_quote || is_text(*string)) &&
					!is_backslash(*string))
			symbol_list_append(symbols, *string);
	}
	return len;
}

static int cut_word(char **string, char **result)
{
	struct symbol_list symbols;
	symbol_list_init(&symbols);
	int start = first_char_pos(*string);
	if(start == -1)
		return 0;
	(*string) += start;
	(*string) += get_word_symbols(*string, &symbols);
	symbol_list_assemble(symbols, result);
	symbol_list_free(&symbols);
	return 1;
}

void handler_split_string(char *string, struct word_list *list)
{
	char *word;
	for(; string && *string; )
	{
		int found = cut_word(&string, &word);
		if(!found)
			break;
		word_list_append(list, word);
	}
}
