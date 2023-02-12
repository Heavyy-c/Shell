#ifndef SYMBOLS_H_SENTRY
#define SYMBOLS_H_SENTRY

struct symbol_list {
	struct symbol_item *first, *last;
	int len;
};

void symbol_list_init(struct symbol_list *list);
void symbol_list_append(struct symbol_list *list, char c);
void symbol_list_assemble(struct symbol_list list, char **result);
void symbol_list_free(struct symbol_list *list);


#endif
