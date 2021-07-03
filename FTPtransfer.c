#include <stdio.h> /* for printf() and fprintf() */
#include <stdlib.h> /* for atoi(), exit() and malloc()*/
#include <string.h> /* for memset() and strcmp() */

#include "protocol.h"

void DieWithError(char *errorMessage); /* Error handling function */
void sendFile(int port, char* fileName);
void recvFile(int port);
void FTPtransfer(int clntSock, char* servIP){
	char fileName[30];
	char cmd;
	char acksig;

	printf("Welcome to Socket FTP client!\n");

	for (;;){
		printf("ftmp command [p)ut g)et l)s r)ls e)xit ] ->");
		scanf("%s",&cmd);
		switch (cmd) {
			case 'p': /* send file */
				acksig = FileUpReq;
				send(clntSock,&acksig,sizeof(char),0); /* send cmd */
				recv(clntSock,&acksig,sizeof(char),0); /* wait for server ack */

				printf("filename to put to server -> ");
				scanf("%s",fileName);

				sendFile(clntSock,fileName);
				break;
	
			case 'g': /* get file */
				acksig = FileAck;
				send(clntSock,&acksig,sizeof(char),0); /* send cmd */
				recv(clntSock,&acksig,sizeof(char),0); /* wait for server ack */

				memset(fileName,0,sizeof(fileName));
				printf("filename to get to server -> ");
				scanf("%s",fileName);

				/* send file name */
				send(clntSock,fileName,sizeof(fileName),0);

				recvFile(clntSock);
				send(clntSock,&acksig,sizeof(char),0);
				break;
	
			case 'l': /* get client file list */
				system(LS);
				break;
	
			case 'r': /* get server file list */
				acksig = GetList;
				send(clntSock,&acksig,sizeof(char),0); /* send cmd */

				recvFile(clntSock);
				system("cat templs");
				break;
	
			case 'e': /* exit */
				acksig = EndConnection;
				send(clntSock,&acksig,sizeof(char),0); /* send cmd */
				printf("close FTP\n");
				return;

			default:
				printf("Invalid Input! Try Again\n");
		}
	}
	return;
}
