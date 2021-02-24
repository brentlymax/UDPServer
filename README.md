# UDP_Server
 UDP Server written in C for Windows

This server works in conjunction with my UDP_Client program.
Upon execution, the server listens for packets sent from the client.
Upon receiving a packet, it checks for validity then sends a response back.

Requires the following 3 libraries:
Ws2_32.lib
Mswsock.lib
AdvApi32.lib
