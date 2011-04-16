/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: client.c
 * Purpose: Creates a TCP connection to the registration server to receive chat 
 * rooms. Connects to a chat room via a TCP or UDP connection.
 * Usage: client <Server IP address> <Server Port> <nickname>
 *
 * Limitations: 
 * Development Computer: x86
 * Operating System: Ubuntu Linux 10.04
 * Integrated Development Environment (IDE): Netbeans 6.9.1
 * Compiler: gcc (Ubuntu 4.4.3-4ubuntu5) 4.4.3
 * Operational Status: Compiles, not complete.
 **/

//INCLUDES ##########################################################
#include "protocols.h"
#include "wrapperFunctions.c"

// CONSTANTS ########################################################

// GLOBALS ##########################################################

// PROTOTYPES #######################################################
void checkArgc(int argc);
int makeConnection(int conenctTCP, char* ipAddress, int port);
int readRoomList(int socketfd, RoomRecord* list);
int getRoomChoice(const RoomRecord* roomList, int roomCount);
void chat(int socketfd, char* username);
void sendStatus(int socketfd, char* username, int status);
void userOutput(int socketfd);
void userInput(int socketfd, char* username);

// MAIN #######################################################

int main(int argc, char* argv[]) {

    int socketfd;
    int port;
    char* ipAddr;
    char* username;
    int roomCount;
    RoomRecord roomList[MAX_ROOMS];
    RoomRecord room;

    //Check Argc for correct requirements
    checkArgc(argc);

    //Setup Connection Specs
    ipAddr = argv[1];
    port = atoi(argv[2]);
    username = argv[3];

    //make a TCP connection to the registration server
    socketfd = makeConnection(TRUE, ipAddr, port);

    roomCount = readRoomList(socketfd, roomList);

    Close(socketfd);

    room = roomList[getRoomChoice(roomList, roomCount)];

    //make a connection to the Room Server
    socketfd = makeConnection(room.tcp, room.address, room.port);

    chat(socketfd, username);

    return 0;

}//end main

//#############################################################################
// Checks ARGC for correct number of arguments, prints usage otherwise
//#############################################################################

void checkArgc(int argc) {
    if (argc != 4) {
        fprintf(stderr, "Usage: client <Server IP address> <Server Port> <nickname>\n");
        exit(1);
    } // End if
}//end checkArgc()

//#############################################################################
//Makes connection and returns the socket file descriptor
//#############################################################################

int makeConnection(int connectTCP, char* ipAddress, int port) {
    int socketfd;
    int status;
    struct sockaddr_in serverAddress;

    bzero(&serverAddress, sizeof (serverAddress));

    //Setup Server Address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    status = inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr);
    if (status != 1) {
        printf("Unable to resolve server IP");
        exit(1);
    }

    if (connectTCP) {
        //TCP Setup
        socketfd = Socket(AF_INET, SOCK_STREAM, 0);
    } else {
        //UDP Setup
        socketfd = Socket(AF_INET, SOCK_DGRAM, 0);
    }//end if

    Connect(socketfd, (struct sockaddr *) & serverAddress, sizeof (serverAddress));

    return socketfd;
}//end makeconnection

//#############################################################################
//Reads an array of RoomRecords from the server, storing them in list
// Returns the number of rooms read
//#############################################################################

int readRoomList(int socketfd, RoomRecord* roomList) {

    RoomRecord tempRoom;
    RoomRecord nullRoom;
    int roomCount = 0;

    memset(&nullRoom, 0, sizeof (nullRoom));

    while (TRUE) {
        Read(socketfd, &tempRoom, sizeof (tempRoom));

        //check for EOF
        if (tempRoom.name == nullRoom.name && tempRoom.address == nullRoom.address) {
            break;
        } else {
            memcpy(&roomList[roomCount], &tempRoom, sizeof (tempRoom));
            roomCount++;
        }//END if/else

    }//END while

    if (roomCount == 0) {
        printf("ERROR: No Servers were Found, please try again later...\n");
        exit(1);
    }//END if

    return roomCount;
}//END getRoomList()

//#############################################################################
//displays the room list and asks the user for a room choice
//#############################################################################

int getRoomChoice(const RoomRecord* roomList, int roomCount) {
    int i;
    int choice;
    int validChoice = FALSE;

    while (!validChoice) {
        printf("------AVAILABLE ROOMS------\n");
        for (i = 0; i < roomCount; i++) {
            printf("%d : %s\n", i, roomList[i].name);
        }//END for

        printf("Please choose a room number:");
        scanf("%d", &choice);

        if (choice < roomCount && choice > -1) {
            validChoice = TRUE;
        }//END if

    }//END while

    return choice;
}//END getRoomChoice()

//#############################################################################
//Allows the User to interact with the chat room
//#############################################################################

void chat(int socketfd, char* username) {
    int pid;

    sendStatus(socketfd, username, STATUS_JOIN);

    pid = Fork();

    if (pid == 0) {
        //CHILD
        userOutput(socketfd);
    }//END if

    //PARENT
    userInput(socketfd, username);

    sendStatus(socketfd, username, STATUS_LEAVE);

    Kill(pid, SIGTERM);

}//END chat()

//#############################################################################
//Send an empty status message
//#############################################################################

void sendStatus(int socketfd, char* username, int status) {
    ChatMessage m;

    memset(&m, 0, sizeof (m));

    m.status = status;
    strncpy(m.user, username, MAX_USER_ID_LENGTH);

    Write(socketfd, &m, sizeof (m));

}//END sendJoin()

//#############################################################################
//Reads messages from the socket and prints them for the user
//#############################################################################

void userOutput(int socketfd) {
    ChatMessage message;

    while (TRUE) {
        Read(socketfd, &message, sizeof (message));
        printf("%s : %s", message.user, message.text);
    }//END while

}//END output()

//#############################################################################
//Reads messages from the user and writes them to the server
//#############################################################################

void userInput(int socketfd, char* username) {
    ChatMessage message;
    char* text = NULL;

    while (TRUE) {
        strncpy(message.user, username, MAX_USER_ID_LENGTH);
        message.status = STATUS_ONLINE;

        scanf("%s", text);
        strncpy(message.text, text, MAX_MESSAGE_TEXT);

        Write(socketfd, &message, sizeof (message));

    }//END while

}//END input()
