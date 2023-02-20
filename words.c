#include <stdlib.h>
#include <string.h>
#include "words.h"


void word_list_init(struct word_list *list)
{
	list->first = NULL;
	list->last = NULL;
}

void word_list_append(struct word_list *list, char *word)
{
	struct word_item *temp = malloc(sizeof(struct word_item));
	temp->word = word;
	temp->next = NULL;
	if(list->first == NULL)
		list->first = temp;
	else
		list->last->next = temp;
	list->last = temp;
}

void word_list_free(struct word_list *list)
{
	struct word_item *first = list->first, *temp;
	for(; first; )
	{
		temp = first;
		first = first->next;
		free(temp->word);
		free(temp);
	}
	word_list_init(list);
}


void copy_data(char *dest, const char *src)
{
	for(; src && *src && dest; src++, dest++)
	{
		*dest = *src;
	}
}

void word_list_pop(struct word_list *list, void *target, char **dest)
{
	struct word_item *current = list->first, *last;
	for(last = NULL; current; last = current, current = current->next)
	{
		if(current == target)
		{
			if(!last)
				list->first = current->next;
			else
				last->next = current->next;
			*dest = malloc(sizeof(char) * strlen(current->word));
			copy_data(*dest, current->word);
			free(current->word);
			free(current);
			break;
		}
	}
}

int word_list_empty(struct word_list list)
{
	return (list.first == NULL);
}

int word_list_count(struct word_list list)
{
	int c;
	struct word_item *current = list.first;
	for(c = 0; current; current = current->next, c++)
	{}
	return c;
}

