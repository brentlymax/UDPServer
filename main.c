/**
 * UDP Server in C for Windows
 * Author: Brently Maxwell
 *
 * This file contains the main() function.
 * It starts the server using Winsock.
 * The server listens for client connections and issues
 * a response based on the validity of the client's packet.
 */

#include <time.h>
#include "structs.h"
#include "functions.h"

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;
	int serverPort = 5150;
	int clientAddrLen;
	dataPacket* recvPacket;
	ackPacket sendACKPacket;
	rejectPacket sendRejPacket;
	int checkSegNum = 1;
	int checkPayloadLen;
	int checkInputEndFlag;
	clientAddrLen = sizeof(SOCKADDR_IN);
	recvPacket = malloc(sizeof(dataPacket));
	printf("Welcome to the Server.\n");

	// Initialize Winsock 2.2.
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		printf("WSAStartup failed. Error: %1d.\n", WSAGetLastError());
		return -1;
	}
	else {
		printf("WSAStartup succeeded. Status: %s.\n", wsaData.szSystemStatus);
	}

	// Open socket.
	if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		printf("socket() failed. Error: %1d.\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else {
		printf("socket() succeeded.\n");
	}

	// Set up server address structure, then bind the server's address to the socket.
	memset((char*)&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr))) == SOCKET_ERROR) {
		printf("bind() failed. Error: %1d.\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}
	else {
		printf("bind() succeeded.\n");
	}

	// Loop and receive/send data from/to clients.
	printf("Server running. Press CTRL + C to quit.\n\n");
	while (1) {
		checkPayloadLen = 0;
		checkInputEndFlag = 0;

		// Receive packet and display contents.
		if (recvfrom(serverSocket, recvPacket, sizeof(dataPacket), 0, (SOCKADDR*)&clientAddr, &clientAddrLen) < 0) {
			printf("Failed to receive data from client. Error: %1d.\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return -1;
		}
		else {
			checkPayloadLen = recvPacket->payloadLen;
			// Case 1: Out of sequence
			if (checkSegNum < recvPacket->segNum) {
				printf("Packet out of sequence. Error: %1x.\n", REJECT_SUB1);
				if (sendReject(recvPacket->clientID, recvPacket->segNum, REJECT_SUB1, serverSocket, clientAddr, clientAddrLen) == 1) {
					printf("Expected segment number: %d\n", checkSegNum);
					printf("Received segment number: %d\n\n", recvPacket->segNum);
					checkSegNum++;
				}
				continue;
			}
			// Case 2: Length Mismatch
			else if ((recvPacket->payload[checkPayloadLen - 1] == '\0' ||
					recvPacket->payload[checkPayloadLen] != '\0') &&
					checkPayloadLen != MAX_PAYLOAD_LEN) {
				printf("Payload length mismatch. Error: %1x.\n", REJECT_SUB2);
				if (sendReject(recvPacket->clientID, recvPacket->segNum, REJECT_SUB2, serverSocket, clientAddr, clientAddrLen) == 1) {
					printf("payloadLen's value: %d\n\n", recvPacket->payloadLen);
					checkSegNum++;
				}
				continue;
			}
			// Case 3: End of packet missing
			else if (recvPacket->endID != PACKET_END_ID) {
				printf("End of packet missing. Error: %1x.\n", REJECT_SUB3);
				if (sendReject(recvPacket->clientID, recvPacket->segNum, REJECT_SUB3, serverSocket, clientAddr, clientAddrLen) == 1) {
					printf("Expected EndID: %x\n", PACKET_END_ID);
					printf("endID's value: %x\n\n", recvPacket->endID);
					checkSegNum++;
				}
				continue;
			}
			// Case 4: Duplicate packet
			else if (recvPacket->segNum < checkSegNum) {
				printf("Duplicate packet. Error: %1x.\n", REJECT_SUB4);
				if (sendReject(recvPacket->clientID, recvPacket->segNum, REJECT_SUB4, serverSocket, clientAddr, clientAddrLen) == 1) {
					printf("Expected segment number: %d\n", checkSegNum);
					printf("Received segment number: %d\n\n", recvPacket->segNum);
					checkSegNum++;
				}
				continue;
			}
			// Base Case: Correct packet
			else {
				printf("Data received successfully from client:\n");
				printf("Packet contents:\n");
				printf("Start ID: %x\n", recvPacket->startID);
				printf("Client ID: %x\n", recvPacket->clientID);
				printf("Packet Type: %x\n", recvPacket->packetType);
				printf("Segment Number: %d\n", recvPacket->segNum);
				printf("Payload Length: %d\n", recvPacket->payloadLen);
				printf("Payload Contents: \n");

				for (int i = 0; i < recvPacket->payloadLen; i++) {
					printf("%c", recvPacket->payload[i]);
				}
				printf("\n");
				printf("End ID: %x\n\n",recvPacket->endID);

				if (sendACK(recvPacket->clientID, recvPacket->segNum, serverSocket, clientAddr, clientAddrLen) == 1) {
					checkSegNum++;
				}
			}
		}
	}
	// When finished, clean up.
	if (closesocket(serverSocket) == SOCKET_ERROR) {
		printf("closesocket() failed. Error: %1d\n.", WSAGetLastError());
	}
	else {
		printf("closesocket() succeeded.\n");
		if (WSACleanup() == SOCKET_ERROR) {
			printf("WSACleanup() failed. Error: %1d\n.", WSAGetLastError());
		}
		else {
			printf("WSACleanup() succeeded.\n");
		}
		return 0;
	}
}
