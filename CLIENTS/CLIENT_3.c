#include "header.h"

// necessary global variables
char myName[50], NAME[50],  server_file[50] = "server_chats_", priv_file[50] = "priv_chat_";

struct clientInfo {
	char fname[50];		// first name
	char lname[50];			// last name
	char username[50];		// usename
	char pass[50];			// password
	char phone[50];			// phone number
}user;					

// Fns prototypes..

int login(SOCKET);		// to login-in
void createAcc(SOCKET);	// to creating a new record

SOCKET connection(char ip[], char port[]);	// to connect to a listening server
void priv_chat(SOCKET);	//	to chat anonymously with only one client
void server_chat();		// to chat together with everyone available on the server 

int main() {
	
	// some frequently used variable...
	
	char ip[15], port[5], ans;	
	SOCKET server;
	fd_set set;		// a set of sockets
	int flag = 0, back = 0;
	
	while(!back) {
		
		system("cls");
		printf("%50s\n\n"," Welcome to the C-Chat program");
		printf(" A. Login\n B. Create an account\n");
		printf(" Your answer >>> ");
		fflush(stdin); ans = getchar();
		system("cls");
		
		// starting the 'main' server which is listening at port "8080"; it will prompt the user to a starting page
		strcpy(ip, "127.0.0.1");
		strcpy(port, "8080");
		server = connection(ip, port);
		
		send(server, &ans, sizeof(ans), 0);	// send the answer to 'main' server
		
		switch(ans) {
			case 'a':
			case 'A':
				back = login(server);		// break out of the loop if 1 is returned
				break;
			case 'b':
			case 'B':
				createAcc(server);
				break;
			default:
				puts("\n Not a valid input\n Press any key to try again\n");
				fflush(stdin); getchar();
		}
		
		CLOSESOCKET(server);	// closing the 'main' server
		WSACleanup();			// closing winsock APIS which were used in 'connection' Fn
	}
	
	
	// A new page...
	// some frequently used variable...
	char target[50], read[250], msg[250], MSG[250], err = 'O', req = 'O', ANS, id[50];
	int i, j;
	FILE *fp;
	
	while(back) {
		
		system("cls");
		printf("%50s\n\n","CHAT ROOMS");
		printf(" A. Chat Privately\n B. Chat in Server Group\n");
		printf(" Your answer >>> ");
		fflush(stdin); ans = getchar();
		system("cls");
		
		if(ans == 'A') {	// private chats
			
			printf("\n A >>> Create a new contact\n B >>> Open an existing one\n");
			printf("\n Your answer >>> ");
			fflush(stdin); ANS = getchar();
			system("cls");
			
			if (ANS == 'B') {
				printf("\n Enter the valid username of client >>> ");		// the username should already exist
				fflush(stdin); scanf("%s", target);
				
				
				printf("\n Connecting %s with %s\n", myName, target);	
				
				// creating the file name..
				
				char file[50];
				strcpy(file, priv_file);
				strcat(file, myName);
				strcat(file, "_");
				strcat(file, target);
				strcat(file, ".txt");
				
				// opening the file...
				fp = fopen(file, "r");
				if(fp == NULL) {
					puts("\n The contact does not exist\n Please create a new one and then open it\n");
					fflush(stdin); getchar();
				}
				else {
					puts(" Connected\n Remember to type \"xxx\" to exit\n Press any key to go to chats\n");
					fflush(stdin); getchar();
					system("cls");
					printf("%50s\n", "PRIVATE CHAT-ROOM\n\n");
					
					// printing previous chat records
					rewind(fp);
					while(!feof(fp)) {
						char ch = fgetc(fp);
						putc(ch, stdout);
					}
					fclose(fp);
					
					// opening for appending chats
					
					fopen(file, "a");
					// connecting to our private chat server which is litening at port 8082...
					strcpy(ip, "127.0.0.1");
					strcpy(port, "8082");
					server = connection(ip, port);
							
					while(1) {
						FD_ZERO(&set);			// zeroing out our socket set
						FD_SET(server, &set);	// including the server in the set
						
						// specifing a timeout for which the select() Fn will check our sockets (for readibility) in the set 
						struct timeval timeout;
						timeout.tv_sec = 0;			// in sec
						timeout.tv_usec = 100000;	// in micro sec
							
						SOCKET CHECK = select(server+1, &set, 0, 0, &timeout);		// clears those sockets from the set which are not available
						// printing the error if any...
						if (CHECK<0) { 
							fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
							CLOSESOCKET(server);
							WSACleanup();
						}	
						
						// WILL EXECUTE ONLY IF SERVER MAKES THE REQUEST...
					
						if (FD_ISSET(server, &set)) {	// if the server is available for readibility in our set
							recv(server, &req, 1, 0);	// receiving the msg from server
							
							if (req == 'A') {
								send(server, myName, 50, 0);	// send my ID
								
								recv(server, &req, 1, 0);		// receive another msg
								if (req == 'P' ) {
									int rec = recv(server, msg, 250, 0);	// receive the msg from other client
									
									// if no msg is received..
									if (rec < 1) {
										printf(" Connection closed by peer.\n");
										break;
									}
									printf(" %10s : %s\n", target, msg);
									fprintf(fp, " %10s : %s\n", target, msg);
								}
							}
						}
							
						// WILL EXECUTE ONLY IF INPUT FROM KEYBOARD IS DETECTED...
						
						if(_kbhit()) {
			
							printf("%10s : ", "SEND");
							fflush(stdin); gets(msg);
							
							// exit if input is "xxx...":
							if (!strncmp(msg, "xxx", 3)) break;
							
							//sending the target contact to server...
							send(server, target, 50, 0);
							
							// sending msg..						
							send(server, msg, 250, 0);
							
							fprintf(fp, " %10s : %s\n", "YOU", msg);
						}
					}
					fclose(fp);
					CLOSESOCKET(server);
					WSACleanup();
				}
			}
			else if (ANS == 'A') {
					
				printf("\n Enter the valid username of client >>> ");		// the username should already exist
				fflush(stdin); scanf("%s", target);
					
				// creating the file name..
				char file[50];
				strcpy(file, priv_file);
				strcat(file, myName);
				strcat(file, "_");
				strcat(file, target);
				strcat(file, ".txt");	
				
				fp = fopen(file, "w");
				printf("\n Creating a new contact...\n");
				printf(" Usename : %s\n Contact : %s\n", myName, target);
					
				if (fp == NULL) {
					printf(" Creation failed for some reason\n");
					return 1;
				}
				fprintf(fp, "%50s", "************************************************\n");
				fclose(fp);
				
				puts("\n Contact created\n");
				fflush(stdin); getchar();
			}
		}
		
		else if(ans == 'B') {	// public chats
				
			printf("%50s\n\n", "PUBLIC CHAT-ROOM");
			
			// creating file name..
			char file[50];
			strcpy(file, server_file);
			strcat(file, myName);
			strcat(file, ".txt");
			
			// printing previous chats	
			fp = fopen(file, "r");
			if(fp == NULL) {
				puts(" Unable to open server chats");
			}
			else {
				rewind(fp);
				while(!feof(fp)) {
					char ch = fgetc(fp);
					putc(ch, stdout);
				}
				fclose(fp);
				
				// opening for appending chats...
				fp = fopen(file, "a");
									
				// connecting to our public group chat server which is litening at port 8081...
				strcpy(ip, "127.0.0.1");
				strcpy(port, "8081");
				server = connection(ip, port);
				
				while(1) {
					FD_ZERO(&set);
					FD_SET(server, &set);
				
					struct timeval timeout;
					timeout.tv_sec = 0;
					timeout.tv_usec = 100000;
							
					SOCKET CHECK = select(server+1, &set, 0, 0, &timeout);
					if (CHECK<0) { 
							fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
							CLOSESOCKET(server);
							WSACleanup();
					}	
					
					if (FD_ISSET(server, &set)) {
						// receiving id of the sender
						recv(server, id, 50, 0);
						
						int rec = recv(server, msg, 250, 0);
						if (rec < 1) {
							printf(" Connection closed by peer.\n");
							break;
						}
						
						printf(" %10s : %s\n", id, msg);
						fprintf(fp, " %10s : %s\n", id, msg);
					}
					
					if(_kbhit()) {
						printf("%10s: ", "SEND");
						fflush(stdin); gets(msg);
						
						if (!strncmp(msg, "xxx", 3)) break;
										
						// sending id..
						send(server, myName, 50, 0);
						
						send(server, msg, 250, 0);
						fprintf(fp, " %10s : %s\n", "YOU", msg);
					}
				}
				fclose(fp);
				CLOSESOCKET(server);
				WSACleanup();
			}
		}
		
		// if user wants to exit...
		system("cls");
		printf("\n Press 'X' to exit OR any key to continue\n Exit (X) >>> ");
		fflush(stdin); ans = getchar();
		back = ans == 'X' ? 0 : 1;
	}

	return 0;
}

SOCKET connection(char ip[], char port[]) {
	// initlaizing winsock...
	WSADATA d;		// WINSOCK variable
	if (WSAStartup(MAKEWORD(2, 2), &d)) {	// specializing the version to be used (2.2) 
		fprintf(stderr, "Failed to initialize.\n");
		return 0;
	}
	
	// Configuring remote address...
	struct addrinfo addr;			// address struct
	memset(&addr, 0, sizeof(addr));	// setting it to zero
	addr.ai_socktype = SOCK_STREAM;	// socket type (TCP sockets)
	
	struct addrinfo *ptr;			// a ptr to addr
	
	if (getaddrinfo(ip, port, &addr, &ptr)) {	// errors if any
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 0;
	}
	
	// Creating the socket that will connect to the listening socket specified by the ip and port...
	SOCKET server = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);	// we'll call it 'server' for convinence
	
	if (!ISVALIDSOCKET(server)) {	// errors if any
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 0;
	}
		
	// Connecting...
	if (connect(server, ptr->ai_addr, ptr->ai_addrlen)) {	// errors if any
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 0;
	}
	
	freeaddrinfo(ptr);
	return server;
}

int login(SOCKET server) {
	char err;
	
	// loop for checking username...
	while (1) {
		system("cls");
		printf("\n%50s\n\n"," Login In");
		
		printf(" Username >>> ");
		fflush(stdin); scanf("%s", user.username);
		send(server, user.username, 50, 0);
		
		printf(" Password >>> ");
		fflush(stdin); scanf("%s", user.pass);
		send(server, user.pass, 50, 0);
		
		// receving any error alerts..
		recv(server, &err, 1, 0);
		if (err == 'F') {
			printf(" Login failed for an unexpected error\n Try creating a new account\n");
			fflush(stdin); getchar();
			return 0;
		}
		
		// checking username for its presence...
		recv(server, &err, 1, 0);			// receiving action from server
		if (err == 'F') {
			printf(" Username not found\n Go Back (X) >>> ");
			fflush(stdin);
			char ans = getchar();
			if (ans == 'X')
				return 0;
		}
		else 
			break;
	}
	
	// initializing some global variables to be used later...
	strcpy(myName, user.username);
	strcpy(NAME, user.fname);
	
	// loop for checking password...
	while (1) {
		recv(server, &err, sizeof(err), 0);		// receiving action from server
		
		if (err == 'S') {
			printf("\n >>> Login successful\n ");
			fflush(stdin); getchar();
			return 1;
		}
		else if (err == 'F') {
			printf(" Incorrect Password\n Go Back (X) >>> ");
			fflush(stdin);
			char ans = getchar();
			system("cls");
			
			if (ans == 'X') {
				err = 'F';
				send(server, &err, 1, 0);		// telling the server not to receive next password
				return 0;
			}
				
			else {
				err = 'S';
				send(server, &err, 1, 0);		// telling the server to receive next password
			}
			
			// entering again ..
			printf("\n%50s\n\n"," Login In");
			
			printf(" Username >>> %s\n", user.username);
			printf(" Password >>> ");
			fflush(stdin); scanf("%s", user.pass);
			
			// sending new password to server
			send(server, user.pass, sizeof(user.pass), 0);
		}
	}
}

void createAcc(SOCKET server) {
	char err;
	
	printf("\n%50s\n\n"," CREATE ACCOUNT");
	
	printf("\n First Name >>> ");
	fflush(stdin); scanf("%s", user.fname);
	send(server, user.fname, 50, 0);
	
	printf("\n Last Name >>> ");
	fflush(stdin); gets(user.lname);
	send(server, user.lname, 50, 0);
	
	printf("\n Username >>> ");
	fflush(stdin); gets(user.username);
	send(server, user.username, 50, 0);
	
	printf("\n Password >>> "); 
	fflush(stdin); gets(user.pass);
	send(server, user.pass, 50, 0);
	
	printf("\n Contact Number >>> ");
	fflush(stdin); gets(user.phone);
	send(server, user.phone, 50, 0);
	
	// creating a public chat file in advance...
	char file[50];
	strcpy(file, server_file);
	strcat(file, user.username);
	strcat(file, ".txt");
	
	FILE *fp = fopen(file, "w");
	fprintf(fp, "****************************************************\n");
	fclose(fp);
	
	puts("\n ACCOUNT CREATED \n");
	fflush(stdin);	getchar();
}







