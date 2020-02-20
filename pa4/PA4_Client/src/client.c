

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>
#include <stdarg.h>
#include "../include/protocol.h"

#define LINE_SIZE 256

FILE *logfp;

void createLogFile(void)
{
    pid_t p = fork();
    if (p == 0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", 0700);
    logfp = fopen("log/log_client.txt", "w");
    //fclose(logfp);
}

void display(int freq[], int num)
{
	for (int i = 0; i < num; i++)
	{
		printf("The letter is: %c, The frequency is: %d\n", i + 'a', freq[i]);
	}
    printf("\n");
}


void logWrite(char* format, ...)
{
    //printf("here?\n");
    char msg[256];
    va_list valist;
    va_start(valist, format);
    vsprintf(msg, format, valist);

    fprintf(logfp, "%s", msg);
    fflush(logfp);
    //fclose(logfp);
}

// deal with single file
void mapFile(char *fname, int *freq)
{
    FILE *fp = fopen(fname, "r");

    if (!fp)
    {
        printf("Failed to open the file\n");
    }

    char line[LINE_SIZE];

    while (fgets(line, LINE_SIZE, fp))
    {
        //printf("line is %s\n", line);
        char target = tolower(line[0]);
        if (target >= 'a' && target <= 'z')
            ++freq[target - 'a'];
    }
}

// check in command
void checkIn(int sockfd, int request[28], int shortResponse[3], int mapperId)
{
    request[RQS_COMMAND_ID] = CHECKIN;
    write(sockfd, request, sizeof(int) * 28);
    read(sockfd, shortResponse, sizeof(int) * 3);
    logWrite("[%d] CHECKIN: %d %d\n", mapperId, shortResponse[1], shortResponse[2]);
}

// command 2

void updateAzlist(int sockfd, int request[28], int shortResponse[3], int mapperId)
{
    // should loop through the length of Mapper_i.txt
    
    FILE *fp;
    char mapperCount[10];
    sprintf(mapperCount, "%d", mapperId);
    char mapInFileName[100] = "MapperInput/Mapper_";
    strcat(mapInFileName, mapperCount);
    strcat(mapInFileName, ".txt");
    
    fp = fopen(mapInFileName, "r");

    if (fp != NULL)
    {
        char line[LINE_SIZE] = {'\0'};
        int lineNum = 0;
        request[RQS_COMMAND_ID] = UPDATE_AZLIST;

        while (fgets(line, LINE_SIZE, fp)!= NULL) // read the mapper_i.txt file
        {
            int singleFreq[26] = {0};
            lineNum++;
            // update azList
            line[strlen(line)-1]=0;

            mapFile(line, singleFreq);
            
            for (int j = 0; j < ALPHABETSIZE; j++)
            {
                request[j + 2] = singleFreq[j];
            }
           

            write(sockfd, request, sizeof(int) * 28);
            read(sockfd, shortResponse, sizeof(int) * 3);
        }

        
        logWrite("[%d] UPDATE_AZLIST: %d\n", mapperId, lineNum);

        fclose(fp);

    }
    else
    {
        printf("fail to open the file\n");
    }

    
}


// command 3
void getAzlist(int sockfd, int request[28], int longResponse[28], int mapperId)
{
    request[RQS_COMMAND_ID] = GET_AZLIST;
    write(sockfd, request, sizeof(int) * 28);
    read(sockfd, longResponse, sizeof(int) * 28);

    fprintf(logfp, "[%d] GET_AZLIST: %d ", mapperId, longResponse[1]);

    for (int k = 0; k < 26; k++)
    {
        fprintf(logfp, "%d ", longResponse[k + 2]);
    }

    fprintf(logfp, "\n");
    fflush(logfp);
}

// command 4
void getMapperUpdate(int sockfd, int request[28], int shortResponse[3], int mapperId)
{
    request[RQS_COMMAND_ID] = GET_MAPPER_UPDATES;
    write(sockfd, request, sizeof(int) * 28);
    read(sockfd, shortResponse, sizeof(int) * 3);
    logWrite("[%d] GET_MAPPER_UPDATES: %d %d\n", mapperId, shortResponse[1], shortResponse[2]);

}

// command 5
void getAllUpdates(int sockfd, int request[28], int shortResponse[3], int mapperId)
{
    request[RQS_COMMAND_ID] = GET_ALL_UPDATES;
    write(sockfd, request, sizeof(int) * 28);
    read(sockfd, shortResponse, sizeof(int) * 3);
    logWrite("[%d] GET_ALL_UPDATES: %d %d\n", mapperId, shortResponse[1], shortResponse[2]);

}

// command 6
void checkout(int sockfd, int request[28], int shortResponse[3], int mapperId)
{
    request[RQS_COMMAND_ID] = CHECKOUT;
    write(sockfd, request, sizeof(int) * 28);
    read(sockfd, shortResponse, sizeof(int) * 3);
    logWrite("[%d] CHECKOUT: %d %d\n", mapperId, shortResponse[1], shortResponse[2]);
}


// tcp connection
void mapConnection(int mapperId, int serverPort, char *server_ip)
{

    // Create a TCP socket.
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Specify an address to connect to (we use the local host or 'loop-back' address).
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(serverPort);
    address.sin_addr.s_addr = inet_addr(server_ip);

    // Connect it.
    if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == 0) 
    {

        int request[28] = {0};
        
        int shortResponse[3] = {0};
        int longResponse[28] = {0};

            // send the following a fixed set of requests sequentially

        request[RQS_MAPPER_PID] = mapperId;

        logWrite("[%d] open connection\n", mapperId);

        //command 1
        checkIn(sockfd, request, shortResponse, mapperId);

        // If there are multiple file paths in the mapper file, this request should be sent
        // as many as the same numbers of file paths
        // if there's no files in the mapper file, mapper clients SHOULD NOT send this message
        updateAzlist(sockfd, request, shortResponse, mapperId);

        getAzlist(sockfd, request, longResponse, mapperId);

        getMapperUpdate(sockfd, request, shortResponse, mapperId);

        getAllUpdates(sockfd, request, shortResponse, mapperId);

        checkout(sockfd, request, shortResponse, mapperId);
    
    
        //close connection
        close(sockfd);
        logWrite("[%d] close connection\n", mapperId);

    }
    
    else
    {
        perror("Connection failed!");
    }
}

// fork
void mapClient(int mappers, int serverPort, char *server_ip)
{
    pid_t child_pid;
    pid_t child_pids[mappers];

    int i;
    for (i = 0; i < mappers; i++)
    {
        child_pid = fork();
        if (child_pid == -1)
        {
            perror("fork");
            break;
        }

        if (child_pid == 0)  // mapper clients
        {
		    int mapperId = i+1;
            mapConnection(mapperId, serverPort, server_ip);
            break;
        }
        else
        {
            child_pids[i] = child_pid;
        }
    }

    // wait for finishing all child processes
    for(int j = 0; j < i; j++)
    {
        waitpid(child_pids[j], NULL, 0);
    }

}

/*
void mapMaster(int server_port, char *server_ip)
{
    // master process dynamically request handling
    // initialize a new TCP connection

    int request[28] = {0};
    int shortResponse[3] = {0};
    int longResponse[28] = {0};

    request[RQS_MAPPER_PID] = -1;

    // read the command.txt as request command
    FILE* fpt = fopen("commands.txt", "r");

    if(fpt == NULL)
    {
        printf("fail to open the commands.txt file\n");
        return;
    }
        
    char line[2] = {'\0'};

    while (fgets(line, LINE_SIZE, fpt)!= NULL) // read the mapper_i.txt file
    {
        int command =  line[0] - 'a';
        // Create a new TCP socket.
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        // Specify an address to connect to (we use the local host or 'loop-back' address).
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(server_port);
        address.sin_addr.s_addr = inet_addr(server_ip);

        // Connect it.
        if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == 0) 
        {
            logWrite("[-1] open connection\n");
           
            request[RQS_COMMAND_ID] = command;
            write(sockfd, request, sizeof(int) * 28);
            
            if(command == 1)
            {
                read(sockfd, shortResponse, sizeof(int) * 3);
                logWrite("[-1] CHECKIN: %d %d\n", shortResponse[1], shortResponse[2]);
            }
            else if(command == 3)
            {
                request[RQS_COMMAND_ID] = GET_AZLIST;
                read(sockfd, longResponse, sizeof(int) * 28);

                fprintf(logfp, "[-1] GET_AZLIST: %d ", longResponse[1]);

                for (int k = 0; k < 26; k++)
                {
                    fprintf(logfp, "%d ", longResponse[k + 2]);
                }

                fprintf(logfp, "\n");
                fflush(logfp);
            }
            else if(command == 4)
            {
                read(sockfd, shortResponse, sizeof(int) * 3);
                logWrite("[-1] GET_MAPPER_UPDATES: %d %d\n", shortResponse[1], shortResponse[2]);
            }
            else if(command == 5)
            {
                read(sockfd, shortResponse, sizeof(int) * 3);
                logWrite("[-1] GET_ALL_UPDATES: %d %d\n", shortResponse[1], shortResponse[2]);
            }
            else if(command == 6)
            {
                read(sockfd, shortResponse, sizeof(int) * 3);
                logWrite("[-1] CHECKOUT: %d %d\n", shortResponse[1], shortResponse[2]);
            }
            else
                logWrite("[-1] wrong command\n");

            //close connection
            close(sockfd);
            logWrite("[%d] close connection\n");     
        }
        else
            perror("COnnection failed\n");

    }


}
*/

int main(int argc, char *argv[])
{
    int mappers;
    char folderName[100] = {'\0'};
    char *server_ip;
    int server_port;

    if (argc == 5)
    { // 4 arguments
        strcpy(folderName, argv[1]);
        mappers = atoi(argv[2]);
        server_ip = argv[3];
        server_port = atoi(argv[4]);
        if (mappers > MAX_MAPPER_PER_MASTER)
        {
            printf("Maximum number of mappers is %d.\n", MAX_MAPPER_PER_MASTER);
            printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
            exit(1);
        }
    }
    else
    {
        printf("Invalid or less number of arguments provided\n");
        printf("./client <Folder Name> <# of mappers> <server IP> <server Port>\n");
        exit(1);
    }

    // create log file
    createLogFile();

    // phase1 - File Path Partitioning
    traverseFS(mappers, folderName);

    // Phase2 - Mapper Clients's Deterministic Request Handling

    mapClient(mappers, server_port, server_ip);
 


    // Phase3 - Master Client's Dynamic Request Handling (Extra Credit)
    //mapMaster(server_port, server_ip);

    fclose(logfp);
    return 0;
}
