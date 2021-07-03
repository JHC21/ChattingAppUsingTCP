#include <stdio.h> /* for printf() and fprintf() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#include "protocol.h"

/* Maximum outstanding connection requests */
void DieWithError(char *errorMessage); /* Error handling function */
void HandleClient(int clntSocket); /* TCP client handling function */

int main(int argc, char *argv[]) {
	if (argc != 2) { /* Test for correct number of arguments */
		fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
		exit(1);
	}

	unsigned short servPort; /* Server port */
	servPort = atoi(argv[1]); /* First arg: local port */

	#ifdef _WIN32
	WSADATA wsa;
	SOCKET servSock; /* Socket descriptor for server */
	SOCKET clntSock; /* Socket descriptor for client */

	int clntLen; /* Length of client address data structure */

	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
		DieWithError("WSA Startup Failed");

	/* Create a socket using TCP */
	if ((servSock = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
		DieWithError("Fail To Create Socket");

	#elif __linux__
	int servSock; /* Socket descriptor */
	int clntSock; /* Socket descriptor for client */
	unsigned int clntLen; /* Length of client address data structure */

	/* Create socket for incoming connections */
	if ((servSock = socket(PF_INET, SOCK_DGRAM, 0)) == NULL)
		DieWithError("Fail To Create Socket");
	#endif

	struct sockaddr_in servAddr; /* Local address */
	struct sockaddr_in clntAddr; /* Client address */

	/* Construct local address structure */
	memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
	servAddr.sin_family = AF_INET; /* Internet address family */
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	servAddr.sin_port = htons(servPort); /* Local port */

	/* Bind to the local address */
	if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		DieWithError("bind() failed");

	/* Mark the socket so it will listen for incoming connections */
	if (listen(servSock, MAXPENDING) < 0)
		DieWithError("listen() failed");

	printf("Start Server at [%d]\n", servPort);
	for (;;) /* Run forever */
		{
			/* Set the size of the in-out parameter */
			clntLen = sizeof(clntAddr);

			/* Wait for a client to connect */
			if ((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0)
				DieWithError("accept() failed");

			/* clntSock is connected to a client! */
			printf("client ip : %s\n", inet_ntoa(clntAddr.sin_addr));
			printf("port : %d\n",clntSock);
			HandleClient(clntSock);
		}
	/* NOT REACHED */
}
