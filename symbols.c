#include <stdlib.h>
#include "symbols.h"

struct symbol_item {
	char c;
	struct symbol_item *next;
};

void symbol_list_init(struct symbol_list *list)
{
	list->first = NULL;
	list->last = NULL;
	list->len = 0;
	list->quoted = 0;
}

void symbol_list_set_quoted(struct symbol_list *list)
{
	list->quoted = 1;
}

void symbol_list_append(struct symbol_list *list, char c)
{
	struct symbol_item *temp = malloc(sizeof(struct symbol_list));
	temp->c = c;
	temp->next = NULL;
	if(list->first == NULL)
		list->first = temp;
	else
		list->last->next = temp;
	list->last = temp;
	list->len += 1;
}

void symbol_list_assemble(struct symbol_list list, char **result)
{
	if(list.quoted)
		list.len += 2;

	else if(list.len == 0)
		return;
	struct symbol_item *current = list.first;
	char *res = malloc(sizeof(char) * (list.len + 1));
	for(int i = list.quoted; current; current = current->next, i++)
	{
		*(res+i) = current->c;
	}
	res[list.len] = '\0';
	if(list.quoted)
	{
		res[0] = '"';
		res[list.len-1] = '"';
	}
	*result = res;
}

void symbol_list_free(struct symbol_list *list)
{
	struct symbol_item *first = list->first, *temp;
	for(; first; )
	{
		temp = first;
		first = first->next;
		free(temp);
	}
	symbol_list_init(list);
}
