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

char* roomName;
int roomType;
int roomPort;

char* regServerAddress;
int regServerPort;

// PROTOTYPES #######################################################
void checkArgc(int argc);
void mainLoop(int listenfd);
void sendMessage();
void receiveMessage();
void notifyRegServer(int message);
// MAIN #######################################################

int main(int argc, char* argv[]) {
    int listenfd;
    fd_set select_fds; //file descriptor list for select()
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
    } else if (statusUpdate.type == REGISTER_FAILURE) {
        printf("Room Registration/Deregistration Failed\n");
    } else {
        printf("Room Registration Responded, Unknown Error\n");
    }

}// End notifyRegServer

//#######################################################
// Main server loop
//#######################################################

void mainLoop(int listenfd) {

}//END mainLoop()

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

