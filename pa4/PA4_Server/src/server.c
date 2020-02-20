
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include "../include/protocol.h"


int currentConn = 0;
int azList[ALPHABETSIZE] = {0};
int entrylen = 0;

// updateStatus[ID][0] - # of updates
// updateStatus[ID][1] - Check in(1) /out(0)
int updateStatus[MAX_MAPPER_PER_MASTER][3] = {0};
pthread_mutex_t mutex;

struct threadArg
{
	int clientfd;
	char *clientip;
	int clientport;
};


void sumWordCount(int request[28])
{
	//pthread_mutex_lock(&mutex);
	for(int i = 0; i < ALPHABETSIZE; i++)
	{
		azList[i] += request[i+2];
	}
	//pthread_mutex_unlock(&mutex);
}

int entry(int mapperID)
{
	for(int i = 0; i<MAX_MAPPER_PER_MASTER; i++)
	{
		if(updateStatus[i][US_MAPPER_PID] == mapperID)
			return i;
	}

	return -1;
}

// single thread
void *threadFunction(void *arg)
{
	struct threadArg *tArg = (struct threadArg *)arg;
	//printf("open connection from %s:%d\n", tArg->clientip, tArg->clientport);
	int request[28];
	int shortResponse[3]={0};
	int longResponse[28]={0};

	while(1)
	{
		//printf("get in loop\n");
		read(tArg->clientfd, request, sizeof(int)*28);

		int mapperID = request[RQS_MAPPER_PID];
		int command = request[RQS_COMMAND_ID];

		int entryIndex = entry(mapperID);

		// error checking
		if(command < 1 || command > 6 || (mapperID < 0 && mapperID != -1))
		{
			shortResponse[RSP_COMMAND_ID] = command;
			shortResponse[RSP_CODE]=RSP_NOK;
			write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
		}

		/*
		// deal with master process
		if(mapperID == -1)
		{
			if(command != 3 || command != 5)
            {
				shortResponse[RSP_CODE] = RSP_NOK;
				write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
            }
			else
			{
				if(command == 3)
				{
					longResponse[RSP_COMMAND_ID] = command;
					longResponse[RSP_CODE] = RSP_OK;

					for(int i= 0; i < ALPHABETSIZE; i++)
					{
						longResponse[i+2] = azList[i];
					}
					write(tArg->clientfd, longResponse, sizeof(int)*LONG_RESPONSE_MSG_SIZE);
					printf("[%d] GET_AZLIST\n", mapperID);
					close(tArg->clientfd);

					printf("close connection from %s:%d\n", tArg->clientip, tArg->clientport);
				}
				else if(command == 5)
				{
					int allUpdate = 0;
					for(int i = 0; i < entrylen; i++)
					{
						allUpdate += updateStatus[i][US_NUM_UPDATES];
					}

					shortResponse[RSP_DATA] = allUpdate;
					shortResponse[RSP_COMMAND_ID] = command;
					shortResponse[RSP_CODE]=RSP_OK;
					write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
					printf("[%d] GET_ALL_UPDATES\n", mapperID);
					close(tArg->clientfd);

					printf("close connection from %s:%d\n", tArg->clientip, tArg->clientport);
				}
				else
				{
					if(command == 1)
						printf("[%d] CHECKIN \n", mapperID);
					if(command == 4)
						printf("[%d] GET_MAPPER_UPDATES \n", mapperID);
					if(command == 6)
						printf("[%d] CHECKOUT \n", mapperID);

					printf("Mapper PID (-1) should be greater than 0\n");
					close(tArg->clientfd);
			
					printf("close connection from %s:%d\n", tArg->clientip, tArg->clientport);
				}
						
			}

		}
		*/


		//check in command
		if(request[RQS_COMMAND_ID] == CHECKIN)
		{

			// first check if entry is existed or not

			if(entryIndex == -1) // create the new entry
			{
				entryIndex = entrylen;
				entrylen++;
				updateStatus[entryIndex][US_MAPPER_PID]= mapperID;
				updateStatus[entryIndex][US_IS_CHECKEDIN] = CHECKIN;
				shortResponse[RSP_COMMAND_ID] = command;
				shortResponse[RSP_CODE]=RSP_OK;
				shortResponse[RSP_DATA] = mapperID;
				//printf("create new entry \n");
				write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);

				printf("[%d] CHECKIN\n", mapperID);
			}
			else // if already existed then update the check field
			{
				// error handling
				if(updateStatus[entryIndex][US_IS_CHECKEDIN] == CHECKIN)
				{
					shortResponse[RSP_COMMAND_ID] = command;
					shortResponse[RSP_CODE]=RSP_NOK;
					write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
				}
				else
				{
					updateStatus[entryIndex][US_IS_CHECKEDIN] = CHECKIN;
					shortResponse[RSP_COMMAND_ID] = command;
					shortResponse[RSP_CODE]= RSP_OK;
					shortResponse[RSP_DATA] = mapperID;
					write(tArg->clientfd, shortResponse, sizeof(int)*3);
					printf("[%d] CHECKIN\n", mapperID);
				}

			}
			//printf("mapper checkin : %d\n", request[RQS_MAPPER_PID]);

		}


		if(request[RQS_COMMAND_ID] == UPDATE_AZLIST)
		{
			pthread_mutex_lock(&mutex);
			// sum the word count result in the azList
			sumWordCount(request);
			
			// update the # of updates
			updateStatus[entryIndex][US_NUM_UPDATES]++;
			shortResponse[RSP_COMMAND_ID] = command;
			shortResponse[RSP_CODE]= RSP_OK;
			shortResponse[RSP_DATA] = mapperID;
			write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
			pthread_mutex_unlock(&mutex);
		}


		if(request[RQS_COMMAND_ID] == GET_AZLIST)
		{
			longResponse[RSP_COMMAND_ID] = command;
			longResponse[RSP_CODE] = RSP_OK;

			for(int i= 0; i < ALPHABETSIZE; i++)
			{
				longResponse[i+2] = azList[i];
			}
			write(tArg->clientfd, longResponse, sizeof(int)*LONG_RESPONSE_MSG_SIZE);
			printf("[%d] GET_AZLIST\n", mapperID);
		}


		if(request[RQS_COMMAND_ID] == GET_MAPPER_UPDATES)
		{
			shortResponse[RSP_COMMAND_ID] = command;
			shortResponse[RSP_CODE]=RSP_OK;
			shortResponse[RSP_DATA] = updateStatus[entryIndex][US_NUM_UPDATES];
			write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
			printf("[%d] GET_MAPPER_UPDATES\n", mapperID);
		}


		if(request[RQS_COMMAND_ID] == GET_ALL_UPDATES)
		{
			int allUpdate = 0;
			for(int i = 0; i < entrylen; i++)
			{
				allUpdate += updateStatus[i][US_NUM_UPDATES];
			}

			shortResponse[RSP_DATA] = allUpdate;
			shortResponse[RSP_COMMAND_ID] = command;
			shortResponse[RSP_CODE]=RSP_OK;
			write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
			printf("[%d] GET_ALL_UPDATES\n", mapperID);
		}

		//check in command
		if(request[RQS_COMMAND_ID] == CHECKOUT)
		{
			// if already checked out
			if( entryIndex == -1 || updateStatus[entryIndex][US_IS_CHECKEDIN] == CHECKOUT)
			{
				shortResponse[RSP_COMMAND_ID] = command;
				shortResponse[RSP_CODE]=RSP_NOK;
			}
			else
			{
				updateStatus[entryIndex][US_IS_CHECKEDIN] = CHECKOUT;
				shortResponse[RSP_COMMAND_ID] = command;
				shortResponse[RSP_CODE]=RSP_OK;
			}
			shortResponse[RSP_DATA] = mapperID;
			write(tArg->clientfd, shortResponse, sizeof(int)*RESPONSE_MSG_SIZE);
			printf("[%d] CHECKOUT\n", mapperID);

			close(tArg->clientfd);
			// clsoe the tcp connection between a client and server
			// print client ip, client port
			printf("close connection from %s:%d\n", tArg->clientip, tArg->clientport);
			break;
		}


	}


	free(tArg);
	pthread_mutex_lock(&mutex);
	currentConn--;
	pthread_mutex_unlock(&mutex);
	return NULL;
}


void reducer(int serverPort)
{
	pthread_t threads[MAX_CONCURRENT_CLIENTS];
	int count = 0;
	pthread_mutex_init(&mutex, NULL);

	// Create a TCP socket.
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	int optval = 1;
	int r = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (r != 0)
	{
    	printf("Cannot enable SO_REUSEADDR option.\n");
	}

	// Bind it to a local address.
	struct sockaddr_in servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons(serverPort);
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sock, (struct sockaddr *)&servAddress, sizeof(servAddress));

	// We must now listen on this port.
	listen(sock, MAX_CONCURRENT_CLIENTS);
	// when the server is ready to listening
	printf("server is listening\n");

	// A server typically runs infinitely, with some boolean flag to terminate.
	while (1)
	{

		// Now accept the incoming connections.
		struct sockaddr_in clientAddress;

		socklen_t size = sizeof(struct sockaddr_in);
		int clientfd = accept(sock, (struct sockaddr *)&clientAddress, &size);


		struct threadArg *arg = (struct threadArg *)malloc(sizeof(struct threadArg));

		arg->clientfd = clientfd;
		arg->clientip = inet_ntoa(clientAddress.sin_addr);
		arg->clientport = clientAddress.sin_port;

		// establish the connection between a client and server
		//printf("open connection from %s:%d\n", arg->clientip, arg->clientport);

		//		Handle the accepted connection by passing off functionality to a thread
		//      Up to MAX_CONCURRENT threads can be running simultaneously, so you will
		//      have to decide how to ensure that this condition holds.

		pthread_mutex_lock(&mutex);

		if (currentConn == MAX_CONCURRENT_CLIENTS)
		{
			printf("Server is too busy\n");
			close(clientfd);
			free(arg);
			pthread_mutex_unlock(&mutex);
			continue;
		}
		else
		{
			currentConn++;
			printf("open connection from %s:%d\n", arg->clientip, arg->clientport);
			pthread_create(&threads[count], NULL, threadFunction, arg);
			count++;
			if (count >= MAX_CONCURRENT_CLIENTS)
			{
				count = 0;
			}
			pthread_mutex_unlock(&mutex);
		}
	}

	// Close the socket.
	close(sock);
}


int main(int argc, char *argv[]) {

    int server_port;

	// initialization
	for(int i = 0; i<ALPHABETSIZE; i++) {azList[i] = 0;}
	for(int i = 0; i<MAX_MAPPER_PER_MASTER; i++) {updateStatus[i][0]= updateStatus[i][1] = 0;}

    if (argc == 2) { // 1 arguments
        server_port = atoi(argv[1]);
    } else {
        printf("Invalid or less number of arguments provided\n");
        printf("./server <server Port>\n");
        exit(0);
    }

    // Server (Reducer) code
    reducer(server_port);

    return 0;
}
