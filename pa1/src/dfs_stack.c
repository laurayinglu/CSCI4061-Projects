#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "dfs_stack.h"
#include "graph.h"

//create a new N-ary tree Node, set all children to be NULL initially
node *NewNode(char *name)
{
    node *temp = (struct TargetNode *)malloc(sizeof(struct TargetNode));
    //temp->child = malloc(MAX_CHILDREN * sizeof(struct TargetNode *));
    temp->name = name;
    temp->childNum = 0;
    temp->visited = false;
    temp->recipe_num = 0;

    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        temp->child[i] = NULL;
    }

    for (int i = 0; i < MAX_RECIPES_PT; i++)
    {
        temp->recipe[i] = NULL;
    }

    return temp;
}

// search the graph and return the targeted node
node *search(char *name, node *root)
{
    node *n = NULL;
    if (root == NULL)
    {
        return NULL;
    }
    if (strcmp(root->name, name) == 0)
    {
        return root;
    }
    if (root->childNum == 0)
    {
        return NULL;
    }
    for (int i = 0; i < root->childNum; i++)
    {
        n = search(name, root->child[i]);
        if (n)
            return n;
    }
    return NULL;
}

// wrtie info from tar into n-ary tree
node *Build(char **lines, int len)
{
    //char** lines = read_file(fname, &len);
    node *root = NULL;
    node *target_node = NULL;
    // length(eachLine)-1 = childNum
    for (int i = 0; i < len; i++)
    {
        if (IsTargetLine(lines[i]))
        {
            char *delim = " :\n";
            char **components = NULL;
            int num_components = makeargv(lines[i], delim, &components);
            char *target = components[0];
            if (!root)
            {
                root = target_node = NewNode(target);
            }
            else
            {
                target_node = search(target, root);
            }
            if (!target_node)
            {
                target_node = NewNode(target);
                root->child[root->childNum++] = target_node;
            }
            for (int i = 1; i < num_components; i++)
            {
                node *child_node = search(components[i], root);
                if (!child_node)
                {
                    child_node = NewNode(components[i]);
                }
                target_node->child[target_node->childNum++] = child_node;
            }
        }
        else
        { // deal with receipe line

            target_node->recipe[target_node->recipe_num] = lines[i];
            target_node->recipe_num++;
        }
    }
    return root;
}

// should use dfs: postorder traversal
void PostOrderPrint(node *root)
{
    if (root == NULL)
    {
        return;
    }

    for (int i = 0; i < root->childNum; i++)
    {
        PostOrderPrint(root->child[i]);
    }

    if (!root->visited)
    {
        for (int i = 0; i < root->recipe_num; i++)
        {
            printf("%s\n", root->recipe[i]);
        }
        root->visited = true;
    }
}

// execute all targets
void PostOrderExecute(node *root)
{
    if (root == NULL)
    {
        return;
    }

    for (int i = 0; i < root->childNum; i++)
    {
        PostOrderExecute(root->child[i]);
    }

    if (!root->visited)
    {
        for (int i = 0; i < root->recipe_num; i++)
        {
            // execute each child
            Execute(root->recipe[i]);
        }
        root->visited = true;
    }
}
// ls -l
// execv("ls", argv)
void Execute(char *recipe)
{
    pid_t child_pid = fork();

    if (child_pid == 0) //child process
    {
        char **myargv;
        char *delim = " \t\n";
        int num = makeargv(recipe, delim, &myargv);
        //printf("%s \n", recipe);
        execvp(myargv[0], myargv);
        int err = errno;
       
    }
    else // parent process
    {
        int status = 0;
        wait(&status);
    }
}

void ExecuteTarget(char *target, node *root)
{
    node *target_node = root;

    if (target)
    {
        target_node = search(target, root);
    }

    PostOrderExecute(target_node);
}
