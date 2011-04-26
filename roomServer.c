/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: roomServer.c
 * Purpose: Provides a chat room for multiple clients
 **/

//INCLUDES ##########################################################
#include "protocols.h"
#include "wrapperFunctions.c"

// CONSTANTS ########################################################
#define	SA struct sockaddr
// GLOBALS ##########################################################

//don't confuse this with socketList in wrapperFunctions.c, this is only for clients in the chatroom
ClientRecord clientList[MAX_CLIENTS];

char* roomName;
int roomType;
int roomPort;

char* regServerAddress;
int regServerPort;

// PROTOTYPES #######################################################
void checkArgc(int argc);
void mainLoop(int listenfd);
void repeatMessage(ChatMessage message);
void notifyRegServer(int message);
int createConnection(char* argv[]);
struct sockaddr_in setupAddress();
void handleSigTermWithDereg(int signo);
void sendUserList(int socketfd);
// MAIN #######################################################

int main(int argc, char* argv[]) {
    int listenfd;
    struct sockaddr_in serverAddress;
    int reuseaddr_value = 1;

    //Check Argc for correct requirements
    checkArgc(argc);

    SigAction(SIGTERM, handleSigTermWithDereg);
    SigAction(SIGCHLD, handleSigChld);
    SigAction(SIGPIPE, handleSigPipe);

    listenfd = createConnection(argv);
    serverAddress = setupAddress();

    //Set Reuse Address to True
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_value, sizeof (reuseaddr_value));

    Bind(listenfd, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
    if (roomType == SOCK_STREAM) {
        Listen(listenfd, MAX_LISTEN_QUEUE_LENGTH);
    }//END if

    //Notify Registration Server
    notifyRegServer(REGISTER_REQUEST);

    memset(clientList, 0, sizeof (clientList));

    mainLoop(listenfd);

    notifyRegServer(REGISTER_LEAVE);

    return 0;
}

//#############################################################################
// Checks ARGC for correct number of arguments, prints usage otherwise
//#############################################################################

void checkArgc(int argc) {
    if (argc != 6) {
        fprintf(stderr, "Usage: roomServer.exe <port> <registration server IP> <registration server port> <TCP/UDP (1/0)> <\"roomName\">\n");
        exit(1);
    } // End if
}//end checkArgc()

//  #######################################################
// Creates connection and returns a FD.
//  #######################################################

int createConnection(char* argv[]) {
    int socketfd;
    int x;
    roomPort = atoi(argv[1]);
    regServerAddress = argv[2];
    regServerPort = atoi(argv[3]);
    //UDP is not supported at this time
    if (atoi(argv[4]) == TRUE) {//TCP
        roomType = SOCK_STREAM;
    } else {//UDP
        roomType = SOCK_DGRAM;
    }//END if/else

    roomName = argv[5];

    //Setup and Bind to port and Listen
    socketfd = Socket(AF_INET, roomType, 0);
    x = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, x | O_NONBLOCK);

    return socketfd;
}
//  #######################################################
// Creates the serverAddress struct
//  #######################################################

struct sockaddr_in setupAddress() {
    struct sockaddr_in serverAddress;

    bzero(&serverAddress, sizeof (serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(roomPort);

    return serverAddress;
}

//  #######################################################
// Sends notification to Registration server
//  #######################################################

void notifyRegServer(int message) {
    int socketfd;
    RegistrationMessage statusUpdate;

    //Reg Server is always TCP
    socketfd = makeConnection(SOCK_STREAM, regServerAddress, regServerPort);

    statusUpdate.type = message;
    statusUpdate.record.type = roomType;
    sprintf(statusUpdate.record.name, "%s", roomName);
    statusUpdate.record.port = roomPort;

    //Send the data to the Registration Server
    Write(socketfd, &statusUpdate, sizeof (statusUpdate)); //sent to server

    //Obtain Reply from registration server
    Read(socketfd, &statusUpdate, sizeof (statusUpdate));
    if (statusUpdate.type == REGISTER_SUCESS) {
        printf("Room Registration/Deregistration Suceeded\n");
    } else if (statusUpdate.type == REGISTER_FAILURE_NO_MEM) {
        printf("Room Registration Failed: Too Many Servers\n");
        cleanup();
    } else if (statusUpdate.type == REGISTER_FAILURE_NAME_EXISTS) {
        printf("Room Registration Failed: Room Name Already Registered, Choose Another Name\n");
        cleanup();
    } else if (statusUpdate.type == REGISTER_FAILURE) {
        printf("Room Registration/Deregistration Failed, Unknown Error\n");
        cleanup();
    } else {
        printf("Room Registration/Deregistration Responded, Unknown Error\n");
        cleanup();
    }//END if/else

    Close(socketfd);
}// End notifyRegServer

//#######################################################
// Main server loop
//#######################################################

void mainLoop(int listenfd) {
    //code from page 178
    int clientfd;
    int socketfd;
    int i;
    int nready;
    fd_set rset;
    fd_set allset;
    struct sockaddr_in clientAddress;
    socklen_t clientLength = sizeof (clientAddress);
    ssize_t n;
    int maxfd = listenfd;
    int maxi = -1;
    ChatMessage message;

    //initialize socket list
    for (i = 0; i < MAX_CLIENTS; i++) {
        clientList[i].socket = SOCKET_NOT_CONNECTED;
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    FD_SET(listenfd, &rset);
    //code from book page 179
    debug("Entering While Loop");

    while (TRUE) {
        rset = allset;

        debug("SELECT");
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                printf("Select Error\n");
            }
        }

        debug("ISSET listenfd");
        if (FD_ISSET(listenfd, &rset)) {//check for new client connection

            if (roomType == SOCK_STREAM) {
                clientfd = accept(listenfd, (SA*) & clientAddress, &clientLength);
            } else {
                struct sockaddr_in socket_addr;
                socklen_t client_addr_size = sizeof (clientAddress);
                socklen_t socket_addr_size = sizeof (socket_addr_size);
                RoomRecord tempRoom;


                if (DEBUG) {
                    printf("UDP Message on listenfd\n");
                }//END if

                memset(&clientAddress, 0, sizeof (clientAddress));
                memset(&tempRoom, 0, sizeof (tempRoom));

                //recvfrom() to get ClientRecord
                Recvfrom(listenfd, &message, sizeof (message), 0, (struct sockaddr *) &clientAddress, &client_addr_size);

                clientfd = Socket(AF_INET, roomType, 0);

                // Connect to "bind" it to the client address (allows Read() and Write())
                Connect(clientfd, (struct sockaddr *) &clientAddress, client_addr_size);

                //Get ephemeral port number and create temp room record
                Getsockname(clientfd, (struct sockaddr *) &socket_addr, &socket_addr_size);
                strncpy(tempRoom.name, roomName, sizeof (tempRoom.name));
                tempRoom.port = ntohs(socket_addr.sin_port);

                if (DEBUG) {
                    printf("Listenfd Port: %d\n", roomPort);
                    printf("New Clientfd Port: %d\n", tempRoom.port);
                }//END if

                //update client with ephemeral port number
                Sendto(listenfd, &tempRoom, sizeof (tempRoom), 0, (struct sockaddr *) &clientAddress, clientLength);
                //Write(clientfd, &tempRoom, sizeof (tempRoom));

            }//END if/else

            //store clientfd in the first available index
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clientList[i].socket == SOCKET_NOT_CONNECTED) { //store FD in the next available
                    ClientRecord c;
                    c.socket = clientfd; //save connection descriptor
                    strncpy(c.name, message.user, sizeof (c.name));
                    clientList[i] = c;
                    break; //break for loop
                }//END IF
            }//END FOR

            if (i == MAX_CLIENTS) {
                printf("Server is full, cannot connect another\n");
            }//END IF

            if (i > maxi) {
                maxi = i; //max index in client[]
            }

            FD_SET(clientfd, &allset); //add FD to set

            if (clientfd > maxfd) {
                maxfd = clientfd; //for select
            }

            if (--nready <= 0) {
                continue; //no more readable FDs
            }
        }//END if ISSET listenfd

        for (i = 0; i <= maxi; i++) { //Check all clients for data
            socketfd = clientList[i].socket;
            if (socketfd < 0) {
                continue;
            }

            if (FD_ISSET(socketfd, &rset)) {
                n = read(socketfd, &message, sizeof (message));
                if (n == 0) { //connection closed by client
                    debug("CLIENT CLOSED CONNECTION 1");
                    Close(socketfd);
                    FD_CLR(socketfd, &allset);
                    clientList[i].socket = SOCKET_NOT_CONNECTED;
                    message.status = STATUS_LEAVE;
                }

                switch (message.status) {
                    case STATUS_JOIN:
                        sprintf(message.text, "Welcome %s\n", message.user);
                        sprintf(message.user, "SERVER");
                        debug("CLIENT JOINED SERVER");
                        repeatMessage(message);
                        break;
                    case STATUS_USER_QUERY:
                        debug("CLIENT REQUESTED USER LIST");
                        sendUserList(socketfd);
                        break;
                    case STATUS_ONLINE:
                        debug("CLIENT SENDING MESSAGE");
                        repeatMessage(message);
                        break;
                    case STATUS_LEAVE:
                        if (n != 0) {
                            sprintf(message.text, "Goodbye %s\n", message.user);
                            sprintf(message.user, "SERVER");
                            repeatMessage(message);
                            debug("CLIENT LEFT SERVER");
                            Close(socketfd);
                            FD_CLR(socketfd, &allset);
                            clientList[i].socket = SOCKET_NOT_CONNECTED;
                        }//END if
                        break;
                    default:
                        break;
                }//END switch

                bzero(&message, sizeof (message));
                message.status = -1;
                if (--nready <= 0) {
                    break;
                }//no more readable FDs

            }//END IF
        }//END FOR

    }//END while

}//END mainLoop()

//#######################################################
//  Sends message to all connected clients
//#######################################################

void repeatMessage(ChatMessage message) {
    int i;
    int childpid;

    childpid = fork();
    if (childpid == 0) {
        //CHILD
        printf("LOG: %s: %s", message.user, message.text);
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clientList[i].socket != SOCKET_NOT_CONNECTED) {
                write(clientList[i].socket, &message, sizeof (message));
            }//END if/else
        }//END for
        exit(1);
    } else {
        //PARENT
        return;
    }//END if/else
}//END sendMessage()

//#######################################################
//  Handles SIGTERM and de-registers the Room
//#######################################################

void handleSigTermWithDereg(int signo) {
    printf("Caught SIGTERM: Exiting\n");
    notifyRegServer(REGISTER_LEAVE);
    cleanup();
}//END handleSigTerm()

//######################################################
//  Sends a list of users to the client
//######################################################

void sendUserList(int socketfd) {
    int childpid;

    childpid = fork();
    if (childpid == 0) {
        //CHILD
        int i;

        ChatMessage message;

        memset(&message, 0, sizeof (message));

        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clientList[i].name == NULL) {
                continue;
            } else {
                message.status = STATUS_USER_QUERY;
                strncpy(message.user, "USER LIST", sizeof ("USER LIST"));
                strncpy(message.text, clientList[i].name, sizeof (message.text));
                Write(socketfd, &message, sizeof (message));
            }//END if/else
        }//END for

        memset(&message, 0, sizeof (message));

        message.status = STATUS_USER_QUERY_COMPLETE;
        Write(socketfd, &message, sizeof (message));

        exit(1);
    } else {
        //PARENT
        return;
    }//END if/else
}//END sendUserList()


