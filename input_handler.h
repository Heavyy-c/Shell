#ifndef INPUT_HANDLER_H_SENTRY
#define INPUT_HANDLER_H_SENTRY

#include "words.h"

int handler_input_correct(const char *string);
void handler_split_string(char *string, struct word_list *list);

#endif
