/* #include "command.h" */

#include <stdlib.h>
#include "words.h"

#define CMD_SEP_CONVEYOR_STR	"|"
#define CMD_SEP_DISJUCTION_STR	"||"
#define CMD_SEP_CONJUCTION_STR	"&&"
#define CMD_SEP_BACKGROUND_STR	"&"

#define REDIRECT_INPUT_STR	"<"
#define REDIRECT_OUTPUT_STR	">"

enum cmd_separators {
	CMD_SEP_NONE,
	CMD_SEP_CONVEYOR,
	CMD_SEP_DISJUCTION,
	CMD_SEP_CONJUCTION,
	CMD_SEP_BACKGROUND
};

enum stream_type {
	ST_STD_INPUT = 0,
	ST_STD_OUT = 1,
	ST_STD_ERR = 2,
	ST_CONVEYOR,
	ST_FILE
};

enum command_modes {
	CM_WAIT,
	CM_DONT_WAIT,
	CM_NEXT_IF_SUCCESSFULLY,
	CM_NEXT_IF_NOT_SUCCESSFULLY
};

enum making_states {
	MS_LIST_EMPTY,
	MS_ERR,
	MS_OK
};

struct command_item {
	char *name;
	char **args;
	enum command_modes mode;
	enum stream_type stream_read, stream_write, stream_err;
	char *file_input;	/*if stream type (input / output) is a file*/
	char *file_output;
	struct command_item *next;
};

struct command_list {
	struct command_item *first, *last;
};

void init_command_item(struct command_item *command)
{
	command->name = NULL;
	command->args = NULL;
	command->mode = CM_WAIT;
	command->stream_read = ST_STD_OUT;
	command->stream_write = ST_STD_INPUT;
	command->stream_err = ST_STD_ERR;
	command->file_input = NULL;
	command->file_output = NULL;
	command->next = NULL;
}

void command_list_init(struct command_list *list)
{
	list->first = NULL;
	list->last = NULL;
}

void command_list_append(struct command_list *list, struct command_item *item)
{
	if(list->first == NULL)
		list->first = item;
	else
		list->last->next = item;
	list->last = item;
}

void copy_data(char *dest, const char *src)
{
	for(; src && *src && dest; src++, dest++)
	{
		*dest = *src;
	}
}

int string_equal(const char *s1, const char *s2)
{
	for(; s1 && *s1 && s2 && *s2; s1++, s2++)
	{
		if(*s1 != *s2)
			return 0;
	}
	if(*s1 != *s2)
		return 0;
	return 1;
}

void command_list_free(struct command_list *list)
{
	struct command_item *current = list->first, *tmp;
	for(; current; )
	{
		free(current->name);
		for(; *(current->args); *(current->args) =
						*((current->args)+1))
			free(*(current->args));
		free(current->args);
		free(current->file_input);
		free(current->file_output);
		tmp = current;
		current = current->next;
		free(tmp);
	}
	command_list_init(list);
}

int is_conjuction(const char *string)
{
	int i;
	for(i = 0; string && *string && CMD_SEP_CONJUCTION_STR[i]; i++)
		if(string[i] != CMD_SEP_CONJUCTION_STR[i])
			return 0;
	return 1;
}

int is_disjuction(const char *string)
{
	int i;
	for(i = 0; string && *string && CMD_SEP_DISJUCTION_STR[i]; i++)
		if(string[i] != CMD_SEP_DISJUCTION_STR[i])
			return 0;
	return 1;
}

int is_conveyor(const char *string)
{
	int i;
	for(i = 0; string && *string && CMD_SEP_CONVEYOR_STR[i]; i++)
		if(string[i] != CMD_SEP_CONVEYOR_STR[i])
			return 0;
	return 1;
}

int is_input_redirect(const char *string)
{
	return string_equal(string, REDIRECT_OUTPUT_STR);
}

int is_output_redirect(const char *string)
{
	return string_equal(string, REDIRECT_OUTPUT_STR);
}

int is_stream_redirect(const char *string)
{
	return is_input_redirect(string) || is_output_redirect(string);
}

int is_mode_special(const char *string)
{
	return is_conjuction(string) || is_disjuction(string)
		|| is_conveyor(string);
}

int is_stream_redirect_special(const char *string)
{
	return is_input_redirect(string) || is_output_redirect(string);
		/*|| is_error_redirect(*string); */
}

void set_command_mode(struct command_item *command, enum cmd_separators sep)
{
	switch(sep)
	{
		case CMD_SEP_CONVEYOR:
			command->stream_write = ST_CONVEYOR;
			break;
		case CMD_SEP_CONJUCTION:
			command->mode = CM_NEXT_IF_SUCCESSFULLY;
			break;
		case CMD_SEP_DISJUCTION:
			command->mode = CM_NEXT_IF_NOT_SUCCESSFULLY;
			break;
		case CMD_SEP_BACKGROUND:
			command->mode = CM_DONT_WAIT;
		default:
			return;
	}
}

enum cmd_separators get_mode_special(const char *string)
{
	if(is_conjuction(string))
		return CMD_SEP_CONJUCTION;
	if(is_disjunction(string))
		return CMD_SEP_DISJUCTION;
	if(is_conveyor(string))
		return CMD_SEP_CONVEYOR;
}


enum cmd_separators get_arguments_words(struct word_list *args,
					struct word_list *words)
{
	enum cmd_separators sep = CMD_SEP_NONE;
	struct word_item *current = words->first;
	for(; current; current = current->next)
	{
		if(is_mode_special(current->word))
		{
			sep = get_mode_special(current->word);
			set_command_mode(current, sep);
			words->first = current->next;
			break;
		}
		word_list_append(words, current->word);
	}
	return sep;
}

void set_command_name(struct command_item *command,
				struct word_list arguments)
{
	copy_data(command->name, arguments.first->word);
}

enum making_states assign_command_stream(struct command_item *command, 
			const char *redirector, const char *file_name)
{
	if(string_equal(redirector, REDIRECT_INPUT_STR))
	{
		if(command->file_input == NULL)
			copy_data(command->file_input, file_name);
		else
			return MS_ERR;
		return MS_OK;
	}
	if(string_equal(redirector, REDIRECT_OUTPUT_STR))
	{
		if(command->file_output == NULL)
			copy_data(command->file_output, file_name);
		else
			return MS_ERR;
		return MS_OK;
	}
}

enum making_states set_command_streams(struct command_item *command,
				struct word_list *arguments)
{
	struct word_item *current = arguments->first, *last, *next;
	char *redirector, *file_name;
	enum making_states success = MS_OK;
	for(next = current->next; success && current; current = current->next)
	{
		next = current->next;
		if(is_stream_redirect(current->word) && !next)
		{
			word_list_pop(arguments, current, &redirector);
			word_list_pop(arguments, current, &file_name);
			success = assign_command_stream(command,
						redirector, file_name);
			free(file_name);
			free(redirector);
		}
		else if(is_stream_redirect(current->word))
		{
			success = MS_ERR;
		}
	}
	return success;
}

void set_command_arguments(struct command_item *command,
				struct word_list args_l)
{
	char **args;
	struct word_item *current = args_l.first;
	int args_count = word_list_count(args_l) + 1;	/* for NULL */
	args = malloc(sizeof(void) * args_count);
	command->args = args;
	for(; current; current = current->next, args++)
	{
		int length = get_length(current->word);
		*args = malloc(sizeof(char) * length);
		copy_data(*args, current->word);
	}
	*args = NULL;
}

void *command_make_new_item()
{
	struct command_item *command = malloc(sizeof(struct command_item));
	init_command_item(command);
	return command;
}

enum making_states make_command(struct word_list *words,
					struct command_item **command)
{
	if(word_list_empty(*words))
		return MS_LIST_EMPTY;
	struct word_list arguments;
	enum cmd_separators separator;
	int success;
	
	word_list_init(&arguments);
	*command = command_make_new_item();
	separator = get_arguments_words(&arguments, words);
	success = set_command_streams(*command, &arguments);
	if(success != MS_OK)
	{
		free(*command);
		goto memory_free;
	}
	set_command_name(*command, arguments);
	set_command_mode(*command, separator);
	set_command_arguments(*command, arguments);
memory_free:
	word_list_free(&arguments);
	return success;
}

int command_distribute_words(struct word_list words,
					struct command_list *commands)
{
	struct command_item *current;
	enum making_states status;
	while(1)
	{
		status = make_command(&words, &current);
		if(status == MS_LIST_EMPTY)
			return 1;
		else if(status == MS_ERR)
			return 0;
		command_list_append(commands, current);
	}
}
