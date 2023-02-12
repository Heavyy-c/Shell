#include <stdlib.h>
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
