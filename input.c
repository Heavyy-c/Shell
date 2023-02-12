#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "input.h"

enum {
	INPUT_BUFFER_SIZE = 20, STR_EOS = '\0',
	CHAR_EOL = '\n', STD_INPUT = 0
};

struct input_item {
	char *buffer;
	int len;
	struct input_item *next;
};

void input_list_init(struct input_list *list)
{
	list->first = NULL;
	list->last = NULL;
}

static void copy_data(const char *src, char *dest, int len)
{
	for(; src && *src && dest && len; src++, dest++, len--)
		*dest = *src;
}

static int eol_in_buffer(const char *buffer, int len)
{
	for(; buffer && *buffer && len; buffer++, len--)
		if(*buffer == CHAR_EOL)
			return 1;
	return 0;
}

static void input_append_new(struct input_list *list,
					const char *buffer, int len)
{
	struct input_item *temp = malloc(sizeof(struct input_item));
	temp->buffer = malloc(sizeof(char)*len);
	temp->len = len;
	temp->next = NULL;
	copy_data(buffer, temp->buffer, len);
	if(list->first == NULL)
		list->first = temp;
	else
		list->last->next = temp;
	list->last = temp;
}

void input_list_read(struct input_list *list)
{
	int exiting = 0;
	char *buffer = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
	while(!exiting)
	{
		int len = read(STD_INPUT, buffer, INPUT_BUFFER_SIZE);
		if(eol_in_buffer(buffer, INPUT_BUFFER_SIZE))
			exiting = 1;
		if(!len)
			break;
		input_append_new(list, buffer, len);
	}
	free(buffer);
}

static int get_input_length(struct input_list list)
{
	int len = 0;
	struct input_item *current = list.first;
	for(; current; current = current->next)
		len += current->len;
	return len;
}

void input_assemble(struct input_list list, char **result)
{
	int length = get_input_length(list) + 1;	/* for ending zero */
	char *res = malloc(sizeof(char) * length);
	res[length-1] = STR_EOS;			/* ending zero */
	*result = res;
	struct input_item *current = list.first;
	for(; current; current = current->next)
	{
		copy_data(current->buffer, res, current->len);
		res += current->len;
	}
}

void input_string_free(char **result)
{
	free(*result);
	*result = NULL;
}

void input_list_free(struct input_list *list)
{
	struct input_item *first = list->first, *temp;
	for(; first; )
	{
		temp = first;
		first = first->next;
		free(temp->buffer);
		free(temp);
	}
	input_list_init(list);
}
