#include "header.h"

// prototypes...
SOCKET listening(char[], char[]);

int main() {
	
	char ip[] = "127.0.0.1",  port[] = "8081";
	int close_server = 0;
	
	SOCKET server = listening(ip, port);
	
	printf("%20s","\n PUBLIC CHATS STARTED\n");
	printf(" LISTENING AT PORT %s\n\n", port);
	puts("\n ACTIVITY:\n");
	
	fd_set set;
	FD_ZERO(&set);
	FD_SET(server, &set);
	SOCKET max = server;

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
					
					char msg[250], ID[50];
					
					recv(i, ID, 50, 0);
					
					int rec = recv(i, msg, 250, 0);
					if (rec < 1)  { FD_CLR(i, &set); CLOSESOCKET(i); continue; }
									
					// closing or not...
					close_server = !strncmp(msg, "yyy", 3)? 1: 0;	
					
					if (!close_server) {
						SOCKET all;
						for (all = 1; all <= max; ++all) {
							if (FD_ISSET(all, &set)) {
								if (all == server || all == i)
										continue;
								else {
									send(all, ID, 50, 0);
									send(all, msg, 250, 0);
								}
									
							}
						}	
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



















