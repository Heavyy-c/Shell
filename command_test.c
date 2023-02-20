#include <stdlib.h>

struct process_item {
	char *name;
	char **arguments;
	enum stream_types stream_write, stream_read, stream_err;\
	char *file_input, *file_output;
	enum exec_mode mode;
	struct process_item *next, *prev;
};

struct process_list {
	struct process_item *first, *last;
};

void process_list_init(struct process_list *list)
{
	list->first = NULL;
	list->last = NULL;
}

void process_list_append(struct process_list *list,
				struct process_item *process)
{
	if(list->first == NULL)
		list->first = process;
	else
		list->last->next = process;
	process->prev = list->last;
	list->last = process;
}

static void process_item_free(struct process_item *proc)
{
	free(proc->name);
	char **args = proc->args;
	for(int i = 0; args[i]; i++)
		free(args[i]);
	free(proc->args);
	free(proc->file_input);
	free(proc->file_output);
	free(proc);
}

void process_list_free(struct process_list *list)
{
	struct process_item *current = list->first, *temp;
	for(; current; )
	{
		temp = current;
		current = current->next;
		process_item_free(temp);
	}
}

static void get_process_params(struct word_list *arguments,
				struct word_list *process_params)
{
	struct word_list params;
	word_list_init(&params);
	for(; current; current = current->next)
	{
		if(is_separator(current))
			break;
		word_list_append(&params, current);
	}
	arguments->first = current;
	process_params = params;
}

static void set_process_data(struct process_item *process,
					struct process_params params)
{
	
}

void process_make_list(struct word_list arguments, struct process_list *list)
{
	struct process_item *process;
	struct word_list process_params;
	while(1)
	{
		if(word_list_empty(*arguments))
			break;
		get_process_params(&arguments, &process_params);
		set_process_data(process, process_params);
		process_list_append(process);
		word_list_free(process_params);
	}
}

void process_format_arguments();
void process_set_streams();
