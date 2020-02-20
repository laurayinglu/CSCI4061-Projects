
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include "graph.h"

int targetNum = 0;
target *head;

int makeargv(const char *s, const char *delimiters, char ***argvp)
{
    int error;
    int i;
    int numtokens;
    const char *snew;
    char *t;

    if ((s == NULL) || (delimiters == NULL) || (argvp == NULL))
    {
        errno = EINVAL;
        return -1;
    }
    *argvp = NULL;
    snew = s + strspn(s, delimiters); /* snew is real start of string */
    if ((t = malloc(strlen(snew) + 1)) == NULL)
        return -1;
    strcpy(t, snew);
    numtokens = 0;
    if (strtok(t, delimiters) != NULL) /* count the number of tokens in s */
        for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++)
            ;

    /* create argument array for ptrs to the tokens */
    if ((*argvp = malloc((numtokens + 1) * sizeof(char *))) == NULL)
    {
        error = errno;
        free(t);
        errno = error;
        return -1;
    }
    /* insert pointers to tokens into the argument array */
    if (numtokens == 0)
        free(t);
    else
    {
        strcpy(t, snew);
        **argvp = strtok(t, delimiters);
        for (i = 1; i < numtokens; i++)
            *((*argvp) + i) = strtok(NULL, delimiters);
    }
    *((*argvp) + numtokens) = NULL; /* put in final NULL pointer */
    return numtokens;
}

// instead, we can write a NewBlock() function to create and initialize in it
target *NewTargetBlock()
{
    struct target_block *tar = (struct target_block *)malloc(sizeof(struct target_block));
    tar->name = NULL;
    for (int i = 0; i < MAX_DEP; i++)
    {
        tar->depend[i] = NULL;
    }

    for (int i = 0; i < MAX_RECIPES_PT; i++)
    {
        tar->recipe[i] = NULL;
    }

    tar->dep_count = 0;
    tar->recipe_count = 0;
    //tar->visited = false;
    tar->executed = false;
    tar->next = NULL;
    return tar;
}

// update depdencies and name
void update_block(target *tar, char *line, char *delim)
{
    int numtokens;
    char **myargv;

    numtokens = makeargv(line, delim, &myargv);
    //printf("%d \n\n", numtokens);
    tar->name = myargv[0];

    if (numtokens > 1)
    {
        for (int i = 1; i < numtokens; i++)
        {
            tar->depend[i - 1] = myargv[i];
            tar->dep_count++;
        }
    }
    else
    {
        tar->dep_count = 0;
    }
}

// check if it's the line containing target
bool IsTargetLine(char *line)
{
    return (strstr(line, ":"));
}

// read in from makefile
char **read_file(char *fname, int *len)
{
    char line[LINE_SIZE];
    int i = 0;
    char **lines = (char **)malloc(MAX_LINES * sizeof(char *));
    for (int i = 0; i < MAX_LINES; i++)
    {
        lines[i] = (char *)malloc(LINE_SIZE);
    }
    *len = 0;
    FILE *fp = fopen(fname, "r");

    if (!fp)
    {
        printf("Failed to open the file: %s \n", fname);
        return NULL;
    }

    while (fgets(line, LINE_SIZE, fp))
    {
        int begin = 0, end = strlen(line) - 1;
        while (isspace((int)line[begin]))
            begin++;
        while ( (end > begin) && isspace((int)line[end]))
            line[end--] = (char)0;
        if( end > begin)
        {
        // only read in nonempty lines
            strcpy(lines[i++], &line[begin]);
            *len += 1;
        }
    }

    return (char **)lines;
}

// Parse the input makefile to determine targets, dependencies, and recipes
target *process_file(char **lines, int len)
{
    target *head = NULL;
    target *tail = NULL;
    target *current = NULL;

    for (int i = 0; i < len; i++)
    {
        if (IsTargetLine(lines[i]))
        {

            if (head == NULL)
            {
                head = tail = current = NewTargetBlock();
            }
            else
            {
                tail->next = current = NewTargetBlock();
                tail = tail->next;
            }
            char *delim = " :\n";
            update_block(current, lines[i], delim);
        }
        else
        {
            current->recipe[current->recipe_count++] = strtok(lines[i], "\t\n");
            //current->recipe_count++;
        }
    }
    return head;
}

// Output format for ./mymake -p makefile_in
void display(target *head)
{
    while (head != NULL)
    {
        printf("target '%s' has %d dependencies and %d recipe \n", head->name, head->dep_count, head->recipe_count);

        if (head->dep_count == 0)
            printf("This target has no dependency.\n");
        else
        {
            for (int i = 0; i < head->dep_count; i++)
            {
                printf("Dependency %d is %s\n", i, head->depend[i]);
            }
        }
        //printf("Dependency %d is %s", head->dep_count-1, head->depend[head->dep_count-1]);

        if (head->recipe_count == 0)
            printf("This target has no recipe.\n");
        else
        {
            for (int i = 0; i < head->recipe_count; i++)
            {
                printf("Recipe %d is %s\n", i, head->recipe[i]);
            }
        }

        printf("\n");
        head = head->next;
    }
}
