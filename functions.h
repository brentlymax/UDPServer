/**
 * UDP Server in C for Windows
 * Author: Brently Maxwell
 *
 * This is the header file for functions.c.
 */

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

void validateDataPacket(dataPacket* dPacket);
dataBuffer* initACKBuffer();
dataBuffer* initRejectBuffer();
ackPacket* initACKPacket(uint8_t recvClientID, uint8_t recvSegNum);
rejectPacket* initRejectPacket(uint8_t recvClientID, uint8_t recvSegNum, uint16_t subCode);
int sendACK(uint8_t recvClientID, uint8_t recvSegNum, int socket, SOCKADDR_IN clientAddr, int clientAddrLen);
int sendReject(uint8_t recvClientID, uint8_t recvSegNum, uint16_t subCode, int socket, SOCKADDR_IN clientAddr, int clientAddrLen);
void serializeACK(ackPacket ackPacket, dataBuffer* dBuffer);
void serializeReject(rejectPacket rejPacket, dataBuffer* dBuffer);

#endif
