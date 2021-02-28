/**
 * UDP Server in C for Windows
 * Author: Brently Maxwell
 *
 * This file contains the helper functions for the server.
 * The functions all relate to creating, sending and receiving
 * different packet types.
 */

#include "structs.h"
#include "functions.h"

/**
 * Initialize buffer for an ACK packet.
 */
dataBuffer* initACKBuffer() {
	dataBuffer* dBuffer = malloc(sizeof(ackPacket)*2);
	dBuffer->data = malloc(sizeof(ackPacket));
	dBuffer->size = sizeof(ackPacket);
	dBuffer->next = 0;
	return dBuffer;
}

/**
 * Initialize buffer for a reject packet.
 */
dataBuffer* initRejectBuffer() {
	dataBuffer* dBuffer = malloc(sizeof(rejectPacket)*2);
	dBuffer->data = malloc(sizeof(rejectPacket));
	dBuffer->size = sizeof(rejectPacket);
	dBuffer->next = 0;
	return dBuffer;
}

/**
 * Initialize ACK packet.
 */
ackPacket* initACKPacket(uint8_t recvClientID, uint8_t recvSegNum) {
	ackPacket* ackPacket = malloc(sizeof(ackPacket));
	ackPacket->startID = PACKET_START_ID;
	ackPacket->clientID = recvClientID;
	ackPacket->packetType = PACKET_TYPE_ACK;
	ackPacket->recvSegNum = recvSegNum;
	ackPacket->endID = PACKET_END_ID;
	return ackPacket;
}

/**
 * Initialize reject packet.
 */
rejectPacket* initRejectPacket(uint8_t recvClientID, uint8_t recvSegNum, uint16_t subCode) {
	rejectPacket* rejPacket = malloc(sizeof(rejectPacket));
	rejPacket->startID = PACKET_START_ID;
	rejPacket->clientID = recvClientID;
	rejPacket->packetType = PACKET_TYPE_REJECT;
	rejPacket->subCode = subCode;
	rejPacket->recvSegNum = recvSegNum;
	rejPacket->endID = PACKET_END_ID;
	return rejPacket;
}

/**
 * Send ACK packet to client.
 * Packet is serialized and stored in a buffer,
 * then sent to client using sendto().
 */
int sendACK(uint8_t recvClientID, uint8_t recvSegNum, int socket, SOCKADDR_IN clientAddr, int clientAddrLen) {
	ackPacket* ackPacket = initACKPacket(recvClientID, recvSegNum);
	dataBuffer *dBuffer = initACKBuffer();
	serializeACK(*ackPacket, dBuffer);

	if (sendto(socket, dBuffer->data, dBuffer->size, 0, (SOCKADDR*)&clientAddr, clientAddrLen) < 0) {
		printf("Failed to send ACK to client. Error: %1d.\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return -1;
	} else
		printf("ACK sent to client.\n");

	free(ackPacket);
	free(dBuffer);
	return 1;
}

/**
 * Send reject packet to client.
 * Packet is serialized and stored in a buffer,
 * then sent to client using sendto().
 */
int sendReject(uint8_t recvClientID, uint8_t recvSegNum, uint16_t subCode, int socket, SOCKADDR_IN clientAddr, int clientAddrLen) {
	rejectPacket* rejPacket = initRejectPacket(recvClientID, recvSegNum, subCode);
	dataBuffer *dBuffer = initRejectBuffer();
	serializeReject(*rejPacket, dBuffer);

	if (sendto(socket, dBuffer->data, dBuffer->size, 0, (SOCKADDR*)&clientAddr, clientAddrLen) < 0) {
		printf("Failed to send Reject code to client. Error: %1d.\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return -1;
	} else
		printf("Reject code sent to client.\n");

	free(rejPacket);
	free(dBuffer);
	return 1;
}

/**
 * Serialize ACK packet into a buffer.
 */
void serializeACK(ackPacket ackPacket, dataBuffer* dBuffer) {
	serializeInt16(ackPacket.startID, dBuffer);
	serializeInt8(ackPacket.clientID, dBuffer);
	serializeInt16(ackPacket.packetType, dBuffer);
	serializeInt8(ackPacket.recvSegNum, dBuffer);
	serializeInt16(ackPacket.endID, dBuffer);
}

/**
 * Serialize Reject packet a buffer.
 */
void serializeReject(rejectPacket rejPacket, dataBuffer* dBuffer) {
	serializeInt16(rejPacket.startID, dBuffer);
	serializeInt8(rejPacket.clientID, dBuffer);
	serializeInt16(rejPacket.packetType, dBuffer);
	serializeInt16(rejPacket.subCode, dBuffer);
	serializeInt8(rejPacket.recvSegNum, dBuffer);
	serializeInt16(rejPacket.endID, dBuffer);
}

/**
 * Serialize uint8_t into a buffer.
 */
void serializeInt8(uint8_t x, dataBuffer* dBuffer) {
	memcpy(((char*)dBuffer->data) + dBuffer->next, &x, sizeof(uint8_t));
	dBuffer->next += sizeof(uint8_t);
}

/**
 * Serialize uint16_t into a buffer.
 */
void serializeInt16(uint16_t x, dataBuffer* dBuffer) {
	memcpy(((char*)dBuffer->data) + dBuffer->next, &x, sizeof(uint16_t));
	dBuffer->next += sizeof(uint16_t);
}
