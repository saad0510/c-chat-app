#include "header.h"

struct clientInfo
{
	char fname[50];	   // first name
	char lname[50];	   // last name
	char username[50]; // usename
	char pass[50];	   // password
	char phone[50];	   // phone number
} user, read;

struct loginInfo
{
	char username[25], pass[25];
} userLog;

// prototypes...
void createAcc();
void login(SOCKET);
SOCKET listening(char[], char[]);

int main()
{

	printf("%20s", "\n MAIN SERVER STARTED\n\n");

	char ip[] = "127.0.0.1", port[] = "8080", ans;
	int c = 0;

	// the server is now Listening clients
	SOCKET server = listening(ip, port);

	puts("\n ACTIVITY:\n");

	fd_set set;
	FD_ZERO(&set);
	FD_SET(server, &set);
	SOCKET max = server; // the maximun specifier of all sockects

	// Waiting for connections...
	while (++c)
	{

		fd_set cpy; // using a copy as it is going to be destroyed by select()
		cpy = set;
		SOCKET CHECK = select(max + 1, &cpy, 0, 0, 0);
		if (CHECK < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
			return 1;
		}

		SOCKET i; // socket counter
		for (i = 1; i <= max; ++i)
		{ // iterating through all sockets of our set
			if (FD_ISSET(i, &cpy))
			{ // if 'i' is readible in the copy
				if (i == server)
				{ // if the readible socket is server (a connection has been requested to the server)

					struct sockaddr_storage addr; // address struct
					int len = sizeof(addr);

					SOCKET client = accept(server, (struct sockaddr *)&addr, &len); // accepting client socket
					if (!ISVALIDSOCKET(client))
					{
						fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
						return 1;
					}

					// including client in our original set...
					FD_SET(client, &set);
					//  maintaining the maximum server specifier...
					if (client > max)
						max = client;

					printf("\n %d. New connection established\n", c);
				}
				// if the readible socket is client (client has sent something)
				else
				{
					recv(i, &ans, 1, 0);

					switch (ans)
					{
					case 'a':
					case 'A':
						printf(" >>> Login-in...\n");
						login(i);
						break;
					case 'b':
					case 'B':
						printf(" >>> Creating account...\n");
						createAcc(i);
						break;
					}
					FD_CLR(i, &set); // removing the client from our set
					CLOSESOCKET(i);	 // closing the client
					puts(" >>> Client Removes");
				}
			}
		}
	}

	printf("\n Closing on port...\n");
	CLOSESOCKET(server);
	WSACleanup();
	printf(" Finished\n");
	return 0;
}

SOCKET listening(char ip[], char port[])
{
	// initlaizing winsock...
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		return 1;
	}

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
	if (!ISVALIDSOCKET(server))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}

	// Binding socket to local address...
	SOCKET bnd = bind(server, ptr->ai_addr, ptr->ai_addrlen);
	if (bnd)
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}

	freeaddrinfo(ptr);

	// Listening...
	SOCKET lstn = listen(server, 10);
	if (lstn < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}

	printf(" Listening at %s\n", port);
	return server;
}

void createAcc(SOCKET client)
{
	int err1, err2, err3, err4, err5, err6;
	char err;

	recv(client, user.fname, 50, 0);
	recv(client, user.lname, 50, 0);
	recv(client, user.username, 50, 0);
	recv(client, user.pass, 50, 0);
	recv(client, user.phone, 50, 0);

	FILE *fp = fopen("users.txt", "ab+");

	fwrite(&user, sizeof(struct clientInfo), 1, fp);
	fclose(fp);

	printf("  >>> An account created\n");
}

void login(SOCKET client)
{

	int rec1, rec2, i, flag = 0, n = 0;
	char err;
	// opening the users information file...
	FILE *fp = fopen("users.txt", "rb");
	// if not available, then the user should create an account

	// check for username
	while (1)
	{
		rec1 = recv(client, userLog.username, 50, 0);
		rec2 = recv(client, userLog.pass, 50, 0);

		// sending an error msg...
		if (fp == NULL || rec1 < 1 || rec2 < 1)
		{
			err = 'F';
			printf("  >>> An error occurred while logining\n");
			send(client, &err, 1, 0);
			return;
		}
		else
		{
			err = 'S';
			send(client, &err, 1, 0);
		}

		// reading the user's information
		rewind(fp);
		while (!feof(fp))
		{
			fread(&read, sizeof(struct clientInfo), 1, fp);
			if (!strcmp(read.username, userLog.username))
			{
				flag = 1;
				break;
			}
		}

		// if username is incorrect
		if (!flag)
		{
			err = 'F';
			puts("	>>> Username not found");
			send(client, &err, 1, 0);
		}
		else
		{
			err = 'S';
			send(client, &err, 1, 0);
			break;
		}
	}

	// checking pass...
	while (1)
	{
		if (!strcmp(read.pass, userLog.pass))
		{
			err = 'S';
			send(client, &err, 1, 0);
			printf("  >>> Correct Information\n");
			break;
		}
		else
		{
			err = 'F';
			send(client, &err, 1, 0);
			printf("	 >>> Incorrect Password\n");

			// to receive next password or not..
			recv(client, &err, 1, 0);
			if (err == 'F')
				break;

			// receving new password
			recv(client, userLog.pass, 50, 0);
		}
	}
	fclose(fp);
	puts("  >>> Login completed");
}
