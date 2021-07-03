#include <stdio.h> /* for printf() and fprintf() */
#include <stdlib.h> /* for atoi(), exit() and malloc()*/
#include <string.h> /* for memset() and strcmp() */
#include <unistd.h> /* for close() */

#include "protocol.h"

void FTPtransfer(int clntSock, char* servIP); /* file transfer Mode */
void DieWithError(char *errorMessage); /* Error handling function */

int main(int argc, char *argv[]) {
	if (argc > 1){ /* Test for correct number of arguments */
		printf("Too many arguments");
		exit(1);
	}

	unsigned short servPort;
	char servIP[16]; /* Server IP address (dotted quad) */
	memset(servIP, 16, sizeof(char));

	/* get Server IP address and port */
	printf("server IP : ");
	scanf("%s",servIP);
	printf("port number : ");
	scanf("%d",&servPort);
	
	struct sockaddr_in servAddr;

	#ifdef _WIN32
	WSADATA wsa;
	SOCKET sock;

	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
		DieWithError("WSA Startup Failed");
	
	/* Create a socket using TCP */
	if ((sock = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
		DieWithError("Fail To Create Socket");

	#elif __linux__
	int sock; /* Socket descriptor */

	/* Create a socket using TCP */
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) == NULL)
		DieWithError("Fail To Create Socket");
	#endif

	/* Construct the server address structure */
	memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
	servAddr.sin_family = AF_INET; /* Internet address family */
	servAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
	servAddr.sin_port = htons(servPort); /* Server port */

	/* Establish the connection to the chatting server */
	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		DieWithError("connect() failed");


	char *msgString; /* String to send to server */
	char msgBuffer[BUFSIZE]; /* Buffer for string */
	unsigned int msgStringLen; /* Length of string*/
	char MsgType;
	int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */
	
	msgString = malloc(sizeof(char*));
	for (;;) {
		/* get message */
		printf("msg-> ");
		scanf("%s",msgString);

		msgStringLen = strlen(msgString); /* Determine input length */

		/* Send the string to the server */
		if (send(sock, msgString, msgStringLen, 0) != msgStringLen)
			DieWithError("send() sent a different number of bytes than expected");
		
		if (!strcmp(msgString,"/quit"))
			break;
		else if (!strcmp(msgString,"/FTP")){ /* File Transfer Mode */
			FTPtransfer(sock,servIP);
			continue;
		}
		else { /* Echo Message Mode */
			/* Receive the same string back from the server */
			totalBytesRcvd = 0;
			printf("msg<- "); /* Setup to print the echoed string */
			while (totalBytesRcvd < msgStringLen) {
				/* Receive up to the buffer size (minus 1 to leave space for a null terminator) bytes from the sender */
				if ((bytesRcvd = recv(sock, msgBuffer, BUFSIZE - 1, 0)) <= 0)
					DieWithError("recv() failed or connection closed prematurely");
				totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
				msgBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
				printf(msgBuffer); /* Print the echo buffer */
			}
		}
		printf("\n"); /* Print a linefeed */
	}
	free(msgString);
	#ifdef _WIN32
	closesocket(sock);
	WSACleanup();
	#elif __linux__
	close(sock);
	#endif
	exit(0);
}