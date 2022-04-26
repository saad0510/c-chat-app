#include "header.h"

// prototypes...
SOCKET listening(char[], char[]);

int main() {
	
	char ip[] = "127.0.0.1",  port[] = "8082";
	int close_server = 0;
	static char target[25];
	char check[50], msg[250], req;
	
	SOCKET server = listening(ip, port);
	SOCKET TARGET;
	
	printf("%20s","\n PRIVATE CHAT STARTED\n");
	printf("\n LISTENING AT %s\n\n", port);
	puts(" ACTIVITY:\n");
	
	
	fd_set set;
	FD_ZERO(&set);
	FD_SET(server, &set);
	SOCKET max = server;
	int c = 0;
	
	// Waiting for connections...
	while(1) {
		fd_set cpy;
		cpy = set;
		SOCKET CHECK = select(max+1, &cpy, 0, 0, 0);
		if (CHECK<0) {
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
			return 1;
		}

		SOCKET i;
		for(i = 1; i <= max; ++i) {
			if (FD_ISSET(i, &cpy)) {
				if (i == server) {
					struct sockaddr_storage addr;
					int len = sizeof(addr);
									
					SOCKET client = accept(server, (struct sockaddr*) &addr, &len);
					if (!ISVALIDSOCKET(client)) { fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO()); close_server = 1; }
									
					// including client in our set...			
					FD_SET(client, &set);
					if (client > max) max = client;
											
					printf(" New connection established\n");
				}
				else {
					
					// receiving the target id
					recv(i, target, 50, 0);
					
					// receiving msg
					int rec = recv(i, msg, 250, 0);
					if (rec < 1)  { FD_CLR(i, &set); CLOSESOCKET(i); continue; }
					
					// closing or not...
					close_server = !strncmp(msg, "yyy", 3)? 1: 0;
										
					if (!close_server) {
						printf("\n MSG + TARGET >>> %s + %s\n", msg, target);
						
						// iterate through all sockets in set
						for(TARGET = 1; TARGET<= max; ++TARGET) {
							// if a socket is available
							if (FD_ISSET(TARGET, &set)) {
								// and it is not the server itself or the client that has sent the message	
								if (TARGET != server && TARGET != i) {
									
									// requesting for IDS
									req = 'A';
									send(TARGET, &req, 1, 0);
									
									// receiving a mandatory id
									recv(TARGET, check, 50, 0);
									
									printf(" => ID received >>> %s\n", check);
									
									// if the received ID is the target...									
									if (!strcmp(check, target) /*&& TARGET != i*/) {
										req = 'P';
										
										// Requesting to receive the msg
										send(TARGET, &req, 1, 0);
										
										// sending msg
										send(TARGET, msg, 250, 0);						
										break;
									}
									else {
										// Requesting to ignore
										req = 'Q';
										send(TARGET, &req, 1, 0); 
									}
								}
							}
						}
					}
					else if (close_server) {
						FD_CLR(i, &set);
						CLOSESOCKET(i);
						puts(" Client Removed\n");
					}
				}
			}
		if (close_server) break;	
		}
	if (close_server) break;
	}

	printf(" Closing...\n");
	CLOSESOCKET(server);
	WSACleanup();
	printf(" Finished\n");
	return 0;
}

SOCKET listening(char ip[], char port[]) {
	// initlaizing winsock...
	WSADATA d; if (WSAStartup(MAKEWORD(2, 2), &d)) { fprintf(stderr, "Failed to initialize.\n"); return 1; }
	
	// Configuring local address...
	struct addrinfo addr;
	memset(&addr, 0, sizeof(addr));
	addr.ai_family = AF_INET; 
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_flags = AI_PASSIVE;
	
	struct addrinfo *ptr;
	getaddrinfo(ip, port, &addr, &ptr);
	
	// Creating socket...
	SOCKET server = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (!ISVALIDSOCKET(server)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}
	
	// Binding socket to local address...
	SOCKET bnd = bind(server, ptr->ai_addr, ptr->ai_addrlen);
	if (bnd) {
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}
	
	freeaddrinfo(ptr);
	
	// Listening...
	SOCKET lstn = listen(server, 10);
	if (lstn < 0) {
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}
	return server;
}














