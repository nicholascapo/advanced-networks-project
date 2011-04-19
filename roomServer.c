/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: roomServer.c
 * Purpose: Create a chat room that will accept TCP and UDP connections
 * Usage: Usage: roomServer.exe <port> <registration server IP> <registration server port> <max connected clients> <TCP/UDP (<1/0>)>
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

// PROTOTYPES #######################################################
void checkArgc(int argc);
void mainLoop();
void sendMessage();
void receiveMessage();
void notifyRegServer(int notify, char* argv[], int sockType);
// MAIN #######################################################

int main(int argc, char* argv[]) {
    int listenfd;
    int regServerFD;
    fd_set select_fds; //file descriptor list for select()
    struct sockaddr_in serverAddress;
    unsigned short listenPort = atoi(argv[1]);

    //Check Argc for correct requirements
    checkArgc(argc);

    //Setup and Bind to port and Listen
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serverAddress, sizeof (serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(listenPort);
    Bind(listenfd, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
    Listen(listenfd, MAX_LISTEN_QUEUE_LENGTH);

    //Notify Registration Server
    notifyRegServer(REGISTER_REQUEST, argv, SOCK_STREAM);

    //Testing Purposes
    fprintf(stderr, "press any key to continue\n"); // better form
    getchar();

    notifyRegServer(REGISTER_LEAVE, argv, SOCK_STREAM);
    //Enter Main Server Loop

    return 0;
}

//#############################################################################
// Checks ARGC for correct number of arguments, prints usage otherwise
//#############################################################################

void checkArgc(int argc) {
    if (argc != 7) {
        fprintf(stderr, "Usage: roomServer.exe <port> <registration server IP> <registration server port> <max connected clients> <TCP/UDP  (<1/0>)> <\"roomName\">\n");
        exit(1);
    } // End if
}//end checkArgc()

//  #######################################################
// Sends notification to Registration server
//  #######################################################

void notifyRegServer(int notify, char* argv[], int sockType) {
    //Connect to registration sever
    unsigned short port = atoi(argv[3]);
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddress;
    int status;
    ssize_t size;
    int nbrBytesRead;

    bzero(&serverAddress, sizeof (serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    RegistrationMessage statusUpdate;

    status = inet_pton(AF_INET, argv[2], &serverAddress.sin_addr);
    if (status != 1) {
        printf("Unable to resolve Registration Server IP\n");
        exit(1);
    }
    status = connect(socketfd, (SA *) & serverAddress, sizeof (serverAddress));
    if (status != 0) {
        printf("Unable to connect to Registration Server.\n");
        exit(1);
    }

    statusUpdate.type = notify;
    //CHANGE THIS IN  FUTURE to reflect both TCP and UDP
    statusUpdate.record.type = sockType;
    //CHANGE THIS IN  FUTURE to reflect both TCP and UDP
    sprintf(statusUpdate.record.name, "%s", argv[6]);
    statusUpdate.record.port = atoi(argv[1]);

    //Send the data to the Registration Server
    size = Write(socketfd, &statusUpdate, sizeof (statusUpdate)); //sent to server

    //Obtain Reply from registration server
    size = Read(socketfd, &statusUpdate, sizeof (statusUpdate));
    if (statusUpdate.type == REGISTER_SUCESS) {
        printf("Room Registration Suceeded\n");
    } else if (statusUpdate.type == REGISTER_FAILURE) {
        printf("Room Registration Failed\n");
    } else {
        printf("Room Registration Responded, Unknown Error\n");
    }

}// End notifyRegServer

//#######################################################
// Main server loop
//#######################################################

void mainLoop() {

}

//#######################################################
//  Sends message to all connected clients
//#######################################################

void sendMessage() {

}

//#######################################################
//  Receive message from a client
//#######################################################

void receiveMessage() {

}

