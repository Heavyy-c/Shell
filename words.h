#ifndef WORDS_H_SENTRY
#define WORDS_H_SENTRY

struct word_item {
	char *word;
	struct word_item *next;
};

struct word_list {
	struct word_item *first, *last;
};

void word_list_init(struct word_list *list);
void word_list_append(struct word_list *list, char *word);
void word_list_free(struct word_list *list);

void word_list_pop(struct word_list *list, void *target, char **dest);
int word_list_empty(struct word_list list);
int word_list_count(struct word_list list);

#endif
