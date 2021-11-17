#include <stdio.h> /* for printf() and fprintf() */
#include <unistd.h> /* for close() */
#include <string.h> /* for memset() */
#include <stdlib.h> /* for system() */

#include "protocol.h"

void DieWithError(char *errorMessage); /* Error handling function */
void sendFile(int port, char* fileName);
void recvFile(int port);
void HandleClient(int clntSocket, char* clntIP) {
	char fileName[30];
	char acksig;
	char buffer[FILEBUF]; /* Buffer for echo string */
	int recvMsgSize; /* Size of received message */

	/* Receive message from client */
	do{
		/* Initialize buffer */
		memset(buffer,0,sizeof(char)*FILEBUF);

		if ((recvMsgSize = recv(clntSocket, buffer, FILEBUF, 0)) < 0)
			DieWithError("recv() failed");
		
		if (!strcmp(buffer,"/FTP")) {
			for (;;) {
				printf("File Transfer Mode\n");
				memset(buffer,0,sizeof(char)*FILEBUF);
				recv(clntSocket, buffer, FILEBUF, 0); /* wait for cmd */
				if(buffer[0] == FileUpReq){
					printf("get file from client\n");
					acksig = FileUpReq;

					/* get file */
					send(clntSocket,&acksig,sizeof(char),0);
					recvFile(clntSocket);
					send(clntSocket,&acksig,sizeof(char),0);

					continue;
				}
				else if (buffer[0] == FileAck){
					printf("send file to client\n");
					acksig = FileAck;
					memset(buffer,0,sizeof(char)*FILEBUF);
	
					/* get file name */
					send(clntSocket,&acksig,sizeof(char),0);
					recv(clntSocket,buffer,sizeof(char)*FILEBUF,0);
					strcpy(fileName, buffer);

					/* send file */
					sendFile(clntSocket,fileName);
	
					continue;
				}
				else if (buffer[0] == GetList){
					printf("send server file ls\n");
					acksig = GetList;
					system("ls >templs");

					/* send temperary ls */
					sendFile(clntSocket,"templs");
					continue;
				}
				else if (buffer[0] == EndConnection) {
					printf("Exit\n");
					break;
				}
			}
		}
		else if(!strcmp(buffer,"/quit")){
			printf("Client left server\n");
			recvMsgSize = -1;
			break;
		}
		else {
			/* print msg in Server */
			printf("msg<- %s\n", buffer);

			/* Echo message back to client */
			if (send(clntSocket, buffer, recvMsgSize, 0) != recvMsgSize)
				DieWithError("send() failed");
	
			/* print msg in Server */
			printf("msg-> %s\n",buffer);

		}
	}while(recvMsgSize > 0);
	
	close(clntSocket); /* Close client socket */
}
