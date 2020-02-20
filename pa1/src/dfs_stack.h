#ifndef __DFS_STACK_H
#define __DFS_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

#define MAX_CHILDREN 8
#define MAX_RECIPES_PT 8

// //Implement a stack for DFS

typedef struct TargetNode {
	char *name;
	bool visited;
	int childNum;
	int recipe_num;
	char *recipe[MAX_RECIPES_PT];
	struct TargetNode *child[MAX_CHILDREN];
} node;

node* NewNode(char* name);
node* search(char* name, node* root);
node* Build(char** lines, int len);
void PostOrderPrint(node* root);

void PostOrderExecute(node *root);
void Execute(char *recipe);
void ExecuteTarget(char *target, node *root);

#endif
