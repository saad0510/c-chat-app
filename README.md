
# C-Chat app

A messenger-like chatting application in which clients can exchange messages with each other. There are two modes of chatting:

1. [Private Mode](./SERVERS/MAIN_SERVER.c) - clients can talk to each other privately.
2. [Public Mode](SERVERS/PUBLIC_SERVER.c) - clients can exchange messages with anyone on the server.

There is also a login system which makes sure to identify each user.


## Dependencies

The project has used the windows api of socket programming - `Winsock2` library. Therefore, the project can only be compiled to windows machines that support this library. Also, make sure to link the object file with winsock modules.

## Features

The project is divided into 2 independent parts - the client and the server. The `CLIENTS/` directory contains the code which should run on the client machine, while the `SERVERS/` directory contains the code which should run on the server.

### Clients

You can create as many copies of clients as you want.

- [CLIENT.c](./CLIENTS/CLIENT.c) - contains the client code.
- [header.h](./CLIENTS/header.c) - contains necessary headers and definitions used in project.

### Servers

All servers need to be running at the same time in order to serve the client(s).

- [MAIN_SERVER.c](./SERVERS/MAIN_SERVER.c) - the server responsible for login session of client(s).
- [PRIVATE_SERVER.c](./SERVERS/PRIVATE_SERVER.c) - the server responsible for private conversations of clients with each other.
- [PUBLIC_SERVER.c](./SERVERS/PUBLIC_SERVER.c) - the server responsible for public group chats of clients.
- [header.h](./SERVERS/header.c) - contains necessary headers and definitions used in project.


The conversations and user data are stored in text and  binaryfiles.
## References

- Book : Hands-on Network Programming with C, Lewis Van Winkle, www.packt.com
## Last Updated

_December, 2020_
## Authors

- github : [@saad0510](https://www.github.com/saad0510)
- email  : k200161@nu.edu.pk or ayyansaad46@gmail.com

