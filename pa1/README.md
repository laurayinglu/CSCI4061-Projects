
###### test machine: CSEL-KH4250-47
###### date: 10/09/2019
###### name: Ying Lu, Junjie Ma
###### x500: lu000097, ma000098
***
* The purpose of our program:
    * The purpose of this program is to create a program to parse the makefile and  
    * to show its inner graph which describes this makefile's data structure. This
    * program also execute processes and run each recipes based on the order of
    * graph reading before this executing.

***

* How to compile the program:
    * In terminal, just do "gcc -o mymake main.c graph.c dfs_stack.c"
    * If you want to do the test, you can also run "make test[i]" (i represents which Makefile you want to test)

***


* What exactly the program does:
    * First, this program reads the makefile, separate each blocks into three main
    * parts: target, dependency and recipe. Then the program print it to show details.
    * The commend line need include "-p" flag to let this part runs.
    *
    * Second, this program will obtain the order of each blocks in this makefile.
    * For example, for target "all", it has some dependencies, the program runs
    * recursively to find the deepest one and print its recipe, and all recipes
    * will be printed based on the dependencies order finally.
    * The commend line need include "-r" flag ti let this part runs.
    *
    * Third, this program will execute the recipes by forking a new process per recipe.
    * Those processes should make sure the order of each part, wait for the child processes
    * and run all processes finally. Specially, when the commend line include exact dependency
    * name, just print and execute this dependency's recipe.

***

* Induvidual contributions:
  * We work together to figure out which data structures to use and implement together.


***


* Assumptions outside the document:
    * We use a helper function called "makeargv", which is from the textbook http://usp.cs.utsa.edu/usp/programs/chapter02/ .
    * Assume each target only execute once.

***

* For extra credit:
    * For the 1.1, we use linked list to store all recipes into array by checking if the next line is target line or not.
    * For 1.2, we Implement a directed graph to store the data from file and store the multiple recipes using the same technique as 1.1.
