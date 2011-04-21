/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: roomServer.c
 * Purpose: Create a chat room that will accept TCP and UDP connections
 * Usage: Usage: roomServer.exe <port> <registration server IP> <registration server port> <TCP/UDP (<1/0>)> <Room Name>
 *
 * Limitations: 
 * Development Computer: x86
 * Operating System: Ubuntu Linux 10.04
 * Integrated Development Environment (IDE): Netbeans 6.9.1
 * Compiler: gcc (Ubuntu 4.4.3-4ubuntu5) 4.4.3
 * Operational Status: Compiles, no functions yet.
 **/

//INCLUDES ##########################################################
#include "protocols.h"
#include "wrapperFunctions.c"

// CONSTANTS ########################################################
#define	SA struct sockaddr
// GLOBALS ##########################################################

//don't cunfuse this with socketList in wrapperFunctions.c, this is only for clients in the chatroom
int clientList[MAX_CLIENTS];

char* roomName;
int roomType;
int roomPort;

char* regServerAddress;
int regServerPort;

// PROTOTYPES #######################################################
void checkArgc(int argc);
void mainLoop(int listenfd);
void repeatMessage(int connfd);
void notifyRegServer(int message);
// MAIN #######################################################

int main(int argc, char* argv[]) {
    int listenfd;
    struct sockaddr_in serverAddress;

    //Check Argc for correct requirements
    checkArgc(argc);

    roomPort = atoi(argv[1]);
    regServerAddress = argv[2];
    regServerPort = atoi(argv[3]);
    if (atoi(argv[4]) == TRUE) {
        //TCP
        roomType = SOCK_STREAM;
    } else {
        //UDP
        roomType = SOCK_DGRAM;
    }//END if/else

    roomName = argv[5];

    //Setup and Bind to port and Listen
    listenfd = Socket(AF_INET, roomType, 0);
    bzero(&serverAddress, sizeof (serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(roomPort);

    Bind(listenfd, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
    Listen(listenfd, MAX_LISTEN_QUEUE_LENGTH);

    //Notify Registration Server
    notifyRegServer(REGISTER_REQUEST);

    //Testing Purposes
    fprintf(stderr, "press any key to continue\n"); // better form
    getchar();

    //mainLoop(listenfd);

    notifyRegServer(REGISTER_LEAVE);

    return 0;
}

//#############################################################################
// Checks ARGC for correct number of arguments, prints usage otherwise
//#############################################################################

void checkArgc(int argc) {
    if (argc != 6) {
        fprintf(stderr, "Usage: roomServer.exe <port> <registration server IP> <registration server port> <TCP/UDP  (<1/0>)> <\"roomName\">\n");
        exit(1);
    } // End if
}//end checkArgc()

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
    int i;
    fd_set select_fds; //file descriptor list for select()

    for (i = 0; i < MAX_CLIENTS; i++) {
        clientList[i] = SOCKET_NOT_CONNECTED;
    }//END for

    FD_ZERO(&select_fds);

    while (TRUE) {
        //SELECT

        //Handle connections from new clients (STATUS_JOIN)
        //Handle clients leaving (STATUS_LEAVE)

        //FORK
        //CHILD
        //      REPEAT_MESSAGE()
        //      This is exit() NOT cleanup() since we dont need to close any of the sockets
        //      exit(1);
        //PARENT

    }//END while

}//END mainLoop()

//#######################################################
//  Sends message to all connected clients
//#######################################################

void repeatMessage(int connfd) {
    ChatMessage message;
    int i;

    Read(connfd, &message, sizeof (message));

    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clientList[i] == SOCKET_NOT_CONNECTED) {
            continue;
        } else {
            Write(clientList[i], &message, sizeof (message));
        }//END if/else
    }//END for
}//END sendMessage()
