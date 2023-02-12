#ifndef INPUT_H_SENTRY
#define INPUT_H_SENTRY

struct input_list {
	struct input_item *first, *last;
};

void input_list_init(struct input_list *list);
void input_list_read(struct input_list *list);
void input_assemble(struct input_list list, char **result);
void input_string_free(char **result);
void input_list_free(struct input_list *list);

#endif
