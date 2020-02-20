#ifndef __GRAPH_H
#define __GRAPH_H

#define LINE_SIZE 128
#define MAX_LINES 128
#define MAX_DEP 8
//Account for extra credit/
#define MAX_RECIPES_PT 8
#define MAX_TARGETS 128
#define MAX_PARM 32

//node type in linked list
typedef struct target_block {
	char *name;
	char *depend[MAX_DEP];
	char *recipe[MAX_RECIPES_PT];
	unsigned char dep_count;
	unsigned char recipe_count;
	//unsigned char visited;
	unsigned char executed;
	struct target_block* next;
} target;


target* NewTargetBlock();
char** read_file(char *fname, int* len);
int makeargv(const char *s, const char *delimiters, char ***argvp);
bool IsTargetLine(char *line);

target* process_file(char** lines, int len);
void update_block(target* tar, char *line, char *delim);
void display(target* head);

#endif
