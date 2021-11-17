#include <stdio.h> /* for general io */
#include <string.h> /* for memset() and strcmp() */

#include "protocol.h"

void DieWithError(char *errorMessage); /* Error handling function */
void sendFile(int port, char* fileName){
	FILE *sendFile;
	int fileSize;
	int readSize;
	char buffer[FILEBUF];

	if((sendFile = fopen(fileName,"rb")) == NULL){
		DieWithError("Fail to open File");
	}

	memset(buffer,0,sizeof(char)*FILEBUF);

	/* set file size */
	fseek(sendFile,0,SEEK_END);
	fileSize = ftell(sendFile);
	fseek(sendFile,0,SEEK_SET);	

	/* send File Size */
	sprintf(buffer,"%d",fileSize);
	send(port,buffer,sizeof(buffer),0);
	recv(port,buffer,1,0);

	memset(buffer,0,sizeof(char)*FILEBUF);

	/* send File Name */
	strcpy(buffer, fileName);
	send(port,buffer,sizeof(buffer),0);

	/* Send File Data */
	recv(port,buffer,1,0); /* start signal */
	printf("Sending => ");
	while(!feof(sendFile)){
		memset(buffer,0,sizeof(char)*FILEBUF);
		readSize = fread(buffer,sizeof(char),FILEBUF,sendFile);

		send(port,buffer,sizeof(char)*FILEBUF,0);
		printf("#");
		recv(port,buffer,1,0);
	}
	printf("\n%s(%d bytes) uploading success\n",fileName,fileSize);

	/* close File and End Transmisttion */
	fclose(sendFile);

	return;
}
