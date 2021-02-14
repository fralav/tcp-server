#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define PORT 27015
#define QLEN 6

void ClearWinSock();

int main(void) {

	// Inizializzazione (solo su windows)
#ifdef WIN32
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr, "Error at WSAStartup().\n");
		return EXIT_FAILURE;
	}
#endif

	// Creazione socket
	int serverSocket;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		fprintf(stderr, "Socket creation failed.\n");
		ClearWinSock();
		return EXIT_FAILURE;
	}

	// Assegnazione indirizzo a socket
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddress.sin_port = htons(PORT);
	if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
		fprintf(stderr, "bind() failed.\n");
		closesocket(serverSocket);
		ClearWinSock();
		return EXIT_FAILURE;
	}

	// Settare socket all'ascolto
	if (listen(serverSocket, QLEN) < 0) {
		fprintf(stderr, "listen() failed.\n");
		closesocket(serverSocket);
		ClearWinSock();
		return EXIT_FAILURE;
	}

	// Connessione e interazione
	struct sockaddr_in clientAddress;
	int clientSocket;
	int clientLength;
	printf("In attesa di connessione da parte di un client...\n");

	while(1) {
		clientLength = sizeof(clientAddress);
		if ((clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientLength)) < 0) {
			fprintf(stderr, "accept() failed.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return EXIT_FAILURE;
		}
		printf("Connessione avvenuta: %s\n", inet_ntoa(clientAddress.sin_addr));

		char message[128] = "Connessione avvenuta!\n";
		if (send(clientSocket, message, sizeof(message), 0) != sizeof(message)) {
			fprintf(stderr, "send() failed.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return EXIT_FAILURE;
		}

		char str1[128];
		char str2[128];

		if (recv(clientSocket, str1, sizeof(str1), 0) < 0) {
			fprintf(stderr, "recv() failed.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return EXIT_FAILURE;
		}

		if (recv(clientSocket, str2, sizeof(str2), 0) < 0) {
			fprintf(stderr, "recv() failed.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return EXIT_FAILURE;
		}

		printf("\nStringa 1: %s\nStringa 2: %s\n", str1, str2);

		char *str1upper = str1;
		while(*str1upper) {
			*str1upper = toupper((unsigned char) *str1upper);
			str1upper++;
		}

		char *str2lower = str2;
		while(*str2lower) {
			*str2lower = tolower((unsigned char) *str2lower);
			str2lower++;
		}

		if (send(clientSocket, str1, sizeof(str1), 0) < 0) {
			fprintf(stderr, "recv() failed.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return EXIT_FAILURE;
		}

		if (send(clientSocket, str2, sizeof(str2), 0) < 0) {
			fprintf(stderr, "recv() failed.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

void ClearWinSock() {
#ifdef WIN32
	WSACleanup();
#endif
}
