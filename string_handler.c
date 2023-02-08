#include <stdlib.h>
#include <unistd.h>
#include "string_handler.h"

enum {
	CHAR_SPACE = ' ', CHAR_TAB = '\t',
	CHAR_EOL = '\n', CHAR_QUOTE = '\'',
	CHAR_DOUBLE_QUOTE = '"', CHAR_EOS = '\0'
};

enum char_types {
	CHAR_TP_SEPARATOR,
	CHAR_TP_CHAR
};


void input_list_init(struct input_item **list)
{
	(*list) = NULL;
}

static int end_of_line(const char *buffer)
{
	for(; buffer && *buffer; buffer++)
		if(*buffer == CHAR_EOL)
			return 1;
	return 0;
}

int input_read(struct input_item **list)
{
	struct input_item *temp = NULL;
	int eof_in_stream;
	while(1)
	{
		temp = malloc(sizeof(struct input_item));
		temp->next = NULL;
		int len = read(0, temp->str, INPUT_BUFFER_SIZE);
		if(len == 0 || end_of_line(temp->str))
		{
			eof_in_stream = (len == 0);
			break;
		}
		if((*list) == NULL)
		{
			*list = temp;
		}
		else
		{
			(*list)->next = temp;
			list = &((*list)->next);
		}
	}
	return eof_in_stream;
}

static int get_items_count(const struct input_item *list)
{
	int count;
	for(count = 0; list; list = list->next, count++)
	{}
	return count;
}

static int get_input_length(const struct input_item *list)
{
	int count = get_items_count(list);
	return (count * INPUT_BUFFER_SIZE);
}

static char *copy_data(char *dest, const char *src, int size)
{
	for(; size > 0; size--, *dest = *src, dest++, src++)
	{}
	return dest;
}

static void copy_all_buffers(char *dest, const struct input_item *list,
							int buffer_size)
{
	for(;list; list = list->next)
	{
		dest = copy_data(dest, list->str, buffer_size);
		*(dest+1) = CHAR_EOS;		/* end of string */
	}
}

char *input_make_string(const struct input_item *list)
{
	int string_len = get_input_length(list) + 1;	/* for padding zero */
	char *result_string = malloc(sizeof(char)*string_len);
	copy_all_buffers(result_string, list, INPUT_BUFFER_SIZE);
	return result_string;
}

static int is_separator(char c)
{
	switch(c)
	{
		case CHAR_TAB:
		case CHAR_SPACE:
			return 1;
		default:
			return 0;
	}
}

static int is_quote(char c)
{
	return (c == CHAR_QUOTE) || (c == CHAR_DOUBLE_QUOTE);
}

static int is_char(char c)
{
	return ((c >= 'a') && (c <= 'z')) ||
		((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9'));
}

static int find_typed(const char *string, enum char_types type)
{
	int pos;
	for(pos = 0; string && *string; string++, pos++)
	{
		if(type == CHAR_TP_SEPARATOR && is_separator(*string))
			break;
		if(type == CHAR_TP_CHAR && is_char(*string))
			break;
	}
	return pos;
}

static int find_symbol(const char *string, char c)
{
	int pos;
	for(pos = 0; string && *string; string++, pos++)
		if(*string == c)
			return pos;
	return -1;
}

static int quote_in_range(const char *start, const char *end)
{
	for(; start && end && (start != end); start++)
		if(is_quote(*start))
			return 1;
	return 0;
}

static int get_length(const char *string)
{
	int len;
	for(len = 0; string && *string; len++, string++)
	{}
	return len;
}

static int find_quote(const char *string)
{
	int len = get_length(string);
	int pos = 0;
	for(pos = 0; string && *string; string++, pos++)
		if(is_quote(*string))
			break;
	return (pos == len) ? -1 : pos;
}

static void find_word(const char *string, int *start, int *end)
{
	int quote_read = 0;
	char symbol;
	*start = find_typed(string, CHAR_TP_CHAR);
	if(quote_in_range(string, string+(*start)))
	{
		*start = find_quote(string);
		symbol = *(string+(*start));
		quote_read = 1;
	}
	string += *start;
	if(quote_read)
		*end = find_symbol(string, symbol);
	else
		*end = find_typed(string, CHAR_TP_SEPARATOR);
}

void input_split_string(struct word_item **list, const char *string)
{
	int start_pos, end_pos, len;
	struct word_item *temp = NULL;
	for( ; string && *string; )
	{
		temp = malloc(sizeof(struct word_item));
		temp->next = NULL;
		if(*list == NULL)
			*list = temp;
		find_word(string, &start_pos, &end_pos);
		end_pos += 1;
		len = end_pos - start_pos;		/* for padding zero */
		temp->word = malloc(sizeof(char)*len);
		copy_data(temp->word, string, len-1);
		string += end_pos;
		(temp->word)[len] = CHAR_EOS;	/* end of string */
		list = &((*list)->next);
	}
}

int input_correct(const char *string)
{
	int pos, quote_open;
	for(pos = 0, quote_open = 0; string && *string; )
	{
		pos = find_quote(string);
		if(pos == -1)
			break;
		quote_open = !quote_open;
		string += pos;
	}
	return !quote_open;
}

void input_list_free(struct input_item *list)
{
	struct input_item *tmp;
	for(; list; )
	{
		tmp = list;
		list = list->next;
		free(tmp);
	}
}

void input_words_free(struct word_item *list)
{
	struct word_item *tmp;
	for(; list; )
	{
		tmp = list;
		list = list->next;
		free(tmp);
	}
}
