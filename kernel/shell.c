#include "init.h"

int END_CODE = 8848;

uint8_t buffer_flag = 0;

uint8_t continue_flag = 1;

extern char recorded_command[100];

extern int recorded_input[100];

char* point = recorded_command;

int* input_point = recorded_input;

uint8_t input_flag = 1;

uint8_t integer_flag = 0;

uint8_t schedule_flag = 0;

void thread_B()
{
	while (1)
	{
		if (schedule_flag == 1)
		{
			put_str("B");
			schedule_flag = 2;
		}
	}
}

void thread_C()
{
	while (1)
	{
		if (schedule_flag == 2)
		{
			put_str("C");
			schedule_flag = 0;
		}
	}
}




int get_integer()
{
    integer_flag = 1;
	int i = 0;
	int count = 0;
	int integer = 0;
	int operators = 1;

	while(input_flag);

	while (recorded_input[i++] !=END_CODE )
	{
		count++;
	}
	for (count--; count >= 0; count--)
	{
		integer = recorded_input[count] * operators + integer;
		operators = operators * 10;
	}


	integer_flag = 0;
	input_flag = 1;
	return integer;
}



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
	char* command_bmi = "mi";
	char* command_thread = "hread";

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
	case 't':
		++read_command;
		while (*read_command++ == *command_thread++)
		{
			if (*read_command == '\n' && *command_thread == NULL)
			{
				put_str("running thread demo....\n");
				put_str("main thread pid is:");
				put_int(current->pid);
				put_str("\n");
				create_thread(thread_B, NULL);
				create_thread(thread_C, NULL);
				while (1)
				{
					if (schedule_flag == 0)
					{
						put_str("A");
						schedule_flag = 1;
					}
				}
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
				put_str("Type command 'memory' can show memory allocate test which allocate memory size  4k per page\n");
				put_str("Type command 'bmi' will execute bmi function which get height and weight value  that calculate bmi\n");
				put_str("Type command 'thread' will show thread schedule function which base on timer device.");
				put_str("Main thread will print 'A',Then there are two threads be created which print'B' and 'C'\n");
				put_str("It will be print 'A' 'B' 'C' characters regularly due to timer device interrupt\n");
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
	case 'b':
	    ++read_command;
		while(*read_command++ == *command_bmi++)
		{
			if (*read_command == '\n' && *command_bmi == NULL)
			{
				put_str("Your weight(kg):");
				int weight = get_integer();
				put_str("\nYour height(cm):");
				int height = get_integer();
				int bmi = (weight*10000) / (height*height);
				put_str("\nYour bmi is:");
				put_int(bmi);
				put_str("  Body status:");
				if (bmi <= 18)
				{
					put_str(" Thin");
				}
				else if (bmi > 18 && bmi < 24)
				{
					put_str(" Normal");
				}
				else if (bmi > 24)
				{
					put_str(" Fat");
				}
				put_str("\n");
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
