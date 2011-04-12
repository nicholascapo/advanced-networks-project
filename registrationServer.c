/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: registrationServer.c
 * Purpose: 
 * Usage: registrationServer.exe <port>
 *
 * Limitations: The maximum number of rooms is limited at 100
 * Development Computer: x86
 * Operating System: Ubuntu Linux 10.04
 * Integrated Development Environment (IDE): Netbeans 6.9.1
 * Compiler: gcc (Ubuntu 4.4.3-4ubuntu5) 4.4.3
 * Operational Status: 
 **/

//INCLUDES ##########################################################
#include "protocols.h"
#include "wrapperFunctions.c"

// CONSTANTS ########################################################

#define LISTENQ 100

// GLOBALS ##########################################################

RoomRecord* roomList[MAX_ROOMS];
int roomCount = 0;

// PROTOTYPES #######################################################

void usage();
void processConnection(int connfd);
void sendRoomList(int connfd);
void registerRoom(int connfd, RoomRecord room);
void deregisterRoom(int connfd, RoomRecord room);
int findEmptyIndex();

// MAIN #######################################################

int main(int argc, char* argv[]) {

    int listenfd;
    int reuseaddr_value = 1;
    int port;

    if (argc != 2) {
        usage();
        exit(1);
    }//END if

    port = atoi(argv[1]);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    //Set Reuse Address to True
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_value, sizeof (reuseaddr_value));

    struct sockaddr_in serveraddr;

    //zero the struct
    memset(&serveraddr, 0, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockaddr *) &serveraddr, sizeof (serveraddr));

    Listen(listenfd, LISTENQ);

    printf("READY TO ACCEPT CLIENT CONNECTIONS\n");

    //this while is for all clients
    while (TRUE) {
        int connfd;
        int pid;

        connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL);

        pid = Fork();
        if (pid == 0) {
            printf("Child process #%d has accepted a TCP connection\n", getpid());

            Close(listenfd);

            displayConnectionInfo(connfd);

            processConnection(connfd);

            Close(connfd);

            exit(0);
        } else {
            //PARENT
            Close(connfd);
        }//END if/else

    }//END while

    return 0;
}//END main()

//  #######################################################

void usage() {
    printf("Usage: registrationServer.exe <port>\n");
}//END usage()

//  #######################################################
//  Reads and preforms the action requested by request.type
//  #######################################################

void processConnection(int connfd) {
    RegistrationMessage request;

    Read(connfd, &request, sizeof (request));

    if (request.type == ROOM_QUERY) {
        sendRoomList(connfd);
    } else if (request.type == REGISTER_REQUEST) {
        registerRoom(connfd, request.record);
    } else if (request.type == REGISTER_LEAVE) {
        deregisterRoom(connfd, request.record);
    } else {
        printf("ERROR: Command not Recognized: %d", request.type);
    }//END if/else

}//END processConnection()

//  #######################################################
// Sends the room list in reverse order to connfd. Room ID's are set to their index
// Client should detect last room by ID == 0
//  #######################################################

void sendRoomList(int connfd) {
    int index;
    for (index = 0; index < roomCount; index++) {
        Write(connfd, &roomList[index], sizeof (roomList[index]));
    }//END for

}//END sendRoomList()

//  #######################################################
//  Adds the new Room to the first empty index in roomList, and responds to roomServer with sucess or failure
//  #######################################################
//  Adds the specified room to the roomList in the first available index
//  Send RegistrationMessge with type set to sucess or failue appropriately
//  #######################################################

void registerRoom(int connfd, RoomRecord room) {
    RegistrationMessage message;

    memset(&message, 0, sizeof (message));

    if (roomCount == MAX_ROOMS) {
        printf("ERROR: Could not allocate space for new Room: Registration Refused!");
        message.type = REGISTER_FAILURE;
    } else {
        printf("Adding Room: %s", room.name);
        roomList[roomCount] = &room;
        roomCount++;
        message.type = REGISTER_SUCESS;
        message.record = room;
    }//END if/else

    Write(connfd, &message, sizeof (message));

}//END registerRoom()

//  #######################################################
//  Removes the specified Room from roomList, Note: This does not result in a sparse array
//  #######################################################

void deregisterRoom(int connfd, RoomRecord room) {
    int i;
    int sucess = FALSE;
    int deleteIndex = -1;

    for (i = 0; i < MAX_ROOMS; i++) {
        if (roomList[i]->name == room.name && roomList[i]->address == room.address) {
            deleteIndex = i;
            sucess = TRUE;
            break;
        }//END if
    }//END for

    if (sucess) {
        printf("Removing Room: %s, %s", roomList[deleteIndex]->name, roomList[deleteIndex]->address);
        roomCount--;
        roomList[deleteIndex] = roomList[roomCount];
        roomList[roomCount] = NULL;
    } else {
        printf("ERROR: Unable to find Room for Removal: %s, %s", roomList[i]->name, roomList[i]->address);
    }//END if

}//END deregisterRoom()
