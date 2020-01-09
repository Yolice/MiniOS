#include "shell.h"
#include "init.h"



uint8_t buffer_flag = 0;

uint8_t continue_flag = 1;

extern char recorded_command[100];

char* point = recorded_command;





void memory_test()
{
	put_str("kernel in memory start:");
	put_hex(kern_start);
	put_str("\n");

	put_str("kernel in memory end:");
	put_hex(kern_end);
	put_str("\n");

	put_str("kernel in memory used:");
	put_int((kern_end - kern_start) / 1024);
	put_str(" KB\n\n");


	show_memory_map();


	put_str("\nTest Physical Memory Allocate:\n");
	uint32_t allc_addr = NULL;
	allc_addr = page_alloc();
	put_str("Allocate Memory: ");
	put_hex(allc_addr);
	put_str("\n");
	allc_addr = page_alloc();
	put_str("Allocate Memory: ");
	put_hex(allc_addr);
	put_str("\n");
	allc_addr = page_alloc();
	put_str("Allocate Memory: ");
	put_hex(allc_addr);
	put_str("\n");
	allc_addr = page_alloc();
	put_str("Allocate Memory: ");
	put_hex(allc_addr);
	put_str("\n");
	allc_addr = page_alloc();
	put_str("Allocate Memory: ");
	put_hex(allc_addr);
	put_str("\n\n");



	//test_heap();
}



void login_shell()
{
	put_str("--------------------------------------------------------------------------------\n");
	put_str("A simple toy OS kernel work on x86 cpu\n");
	put_str("Press any(enter) key load shell......\n");

	while (continue_flag);
	clear();
	while (1)
	{
		if (buffer_flag)
		{
			command_execute(point);
			put_str("MiniOS@Kernal$:");
			buffer_flag = 0;
		}
	}
}


void command_execute(char* read_command)
{
	char* local_command = read_command;
	char* command_clear = "lear";
	char* command_help = "elp";
	char* command_memory = "emory";

	if (*read_command == '\n')
	{
		return;
	}

	switch (*read_command)
	{
	case 'c':
		++read_command;
		while (*read_command++ == *command_clear++ )
		{
			if (*read_command == '\n' && *command_clear == NULL)
			{
				clear();
				return;
			}
		}
		break;
	case 'h':
		++read_command;
		while (*read_command++ == *command_help++)
		{
			if (*read_command == '\n' && *command_help == NULL)
			{
				put_str("--------------------------------------info--------------------------------------\n");
				put_str("Type command 'clear' will clean the shell screen\n");
				put_str("Type command 'memory' can show memory allocate test which allocate memory size 4k per page\n");
				put_str("This OS have schedule function which make kernel_thread and new_create_thread ");
				put_str("schedule base on timer device.But it's not work on the shell \n");
				put_str("You can review the source code that uploaded in the Github ");
			    put_str("URL:https://github.com/Yolice/MiniOS \n\n");
				put_str("--------------------------------------end---------------------------------------\n");
				return;
			}
		}
		break;
	case 'm':
		++read_command;
		while (*read_command++ == *command_memory++)
		{
			if (*read_command == '\n' && *command_memory == NULL)
			{
				memory_test();
				return;
			}
		}
		break;
	default:
		break;
	}

	read_command = local_command;

	while (*read_command && *read_command != '\n')
	{
		put_char(*read_command);
		read_command++;
	}

	put_str(":Command not found!\n");

	memset(recorded_command, NULL, 100);



}
