#include <stdio.h> /* for general io */
#include <stdlib.h> /* for atoi(), exit() and malloc()*/
#include <string.h> /* for memset() and strcmp() */

#include "protocol.h"

void DieWithError(char *errorMessage); /* Error handling function */
void recvFile(int port){
	FILE *getFile;
	int fileSize;
	int rcvSize;
	char fileName[BUFSIZE];
	char buffer[FILEBUF];

	memset(buffer,0,sizeof(char)*FILEBUF);

	/* get file size */
	recv(port,buffer,sizeof(buffer),0);
	fileSize = atoi(buffer);
	memset(buffer,0,sizeof(char)*FILEBUF);
	send(port,buffer,1,0);

	/* get file name */
	recv(port,buffer,sizeof(buffer),0);
	strcpy(fileName, buffer);
	memset(buffer,0,sizeof(char)*FILEBUF);

	/* create file */
	if((getFile = fopen(fileName,"wb")) == NULL){
		DieWithError("Fail to create File");
	}
	printf("receiving File name: %s(%d bytes)\n", fileName, fileSize);

	/* get file data */
	send(port,buffer,1,0); /* start signal */
	printf("Receiving => ");
	int counter = 0;
	while (counter < fileSize){
		rcvSize = recv(port,buffer,sizeof(buffer),0);
		printf("#");
		fwrite(buffer,sizeof(char),rcvSize,getFile);
		memset(buffer,0,sizeof(char)*FILEBUF);
		counter += rcvSize;

		send(port,buffer,1,0);
	}
	printf("\n%s(%d bytes) downloading success\n",fileName,fileSize);

	/* close File and End Transmisttion */
	fclose(getFile);

	return;
}
