
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "graph.h"
#include "dfs_stack.h"


#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )
struct target_block* tar;

//Validate the input arguments, bullet proof the program
int main(int argc, char *argv[])
{
	target* head = NULL;
	// length of files
	int len = 0;
	//$./mymake Makefile
	//Similarly account for -r flag
	char **lines = NULL;


	node* root = NULL;

	if (argc == 3 && (!strncmp(argv[1], "-p", 2) || !strncmp(argv[1], "-r", 2)))
	{
		lines = read_file(argv[2], &len);
		if( lines == NULL )
			return EXIT_SUCCESS;

		root = Build(lines, len);
		if ( 0 == strncmp(argv[1], "-r", 2) )
		{
			PostOrderPrint(root);
			return EXIT_SUCCESS;
		}

		if ( 0 == strncmp(argv[1], "-p", 2) )
		{
			head = process_file(lines, len);
			display(head);
			return EXIT_SUCCESS;
		}
	}

	//$./mymake Makefile [target]
	if( argc == 2 || argc == 3)
	{
		lines = read_file(argv[1], &len);

		if( lines == NULL )
			return EXIT_SUCCESS;

		root = Build(lines, len);
		char* target = NULL;
		if( argc == 3)
			target = argv[2];
		ExecuteTarget(target, root);

		return EXIT_SUCCESS;
	}

	if(argc > 3)
	{
		printf("Too many arguments. \n");
		return EXIT_FAILURE;
	}
	printf("Wrong input order.\n");
	return EXIT_FAILURE;
}
