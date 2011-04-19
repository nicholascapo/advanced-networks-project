/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: registrationServer.c
 * Purpose: Create a TCP server that will keep track of chat room servers
 * Usage: registrationServer.exe <port>
 *
 * Limitations: The maximum number of rooms is limited at 100
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

RoomRecord roomList[MAX_ROOMS];
int roomCount = 0;

// PROTOTYPES #######################################################

void usage();
void processConnections(int listenfd);
void sendRoomList(int connfd);
void registerRoom(int connfd, RoomRecord* room);
void deregisterRoom(int connfd, RoomRecord* room);
int findEmptyIndex();
void debugPopulateRoomList();

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

    if (DEBUG) {
        debugPopulateRoomList();
    }//END if

    Listen(listenfd, MAX_LISTEN_QUEUE_LENGTH);

    printf("READY TO ACCEPT CLIENT CONNECTIONS\n");

    processConnections(listenfd);

    return 0;
}//END main()

//  #######################################################

void usage() {
    printf("Usage: registrationServer.exe <port>\n");
}//END usage()

//  #######################################################
//  Reads and preforms the action requested by request.type
//  #######################################################

void processConnections(int listenfd) {
    RegistrationMessage request;

    while (TRUE) {
        int connfd;
        connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL);

        //can't fork here because we might need to write to the global RoomList
        //maybe this is a bug, But I dont know how to fix it.
        //If I were to use pThreads I would need a thread-safe Array or storage of some kind.
        Read(connfd, &request, sizeof (request));

        printf("Read Request: %d, %s, %s, %d\n", request.type, request.record.name, request.record.address, request.record.port);

        if (request.type == ROOM_QUERY) {
            int pid;
            //only fork if we need to send the room list,
            // other operations should be almost atomic and not delay other clients
            pid = Fork();
            if (pid == 0) {
                printf("Child process #%d has accepted a TCP connection\n", getpid());
                Close(listenfd);
                displayConnectionInfo(connfd);
                sendRoomList(connfd);
                Close(connfd);
                exit(0);
            } else {
                //PARENT
                Close(connfd);
            }//END if/else
        } else if (request.type == REGISTER_REQUEST) {
            //can't fork here because we need to write to the global RoomList
            displayConnectionInfo(connfd);
            registerRoom(connfd, &request.record);
        } else if (request.type == REGISTER_LEAVE) {
            //can't fork here because we need to write to the global RoomList
            displayConnectionInfo(connfd);
            deregisterRoom(connfd, &request.record);
        } else {
            printf("ERROR: Command not Recognized: %d\n", request.type);
        }//END if/else
    }//END while

}//END processConnection()

//  #######################################################
// Sends the room list in reverse order to connfd. Room ID's are set to their index
// Client should detect last room by ID == 0
//  #######################################################

void sendRoomList(int connfd) {
    int index;
    RoomRecord nullRoom;

    for (index = 0; index < roomCount; index++) {
        Write(connfd, &roomList[index], sizeof (roomList[index]));
        if (DEBUG) {
            printf("Wrote Room: %d, %s, %s, %d\n", roomList[index].type, roomList[index].name, roomList[index].address, roomList[index].port);
        }//END if
    }//END for

    //EOF
    memset(&nullRoom, 0, sizeof (nullRoom));
    nullRoom.type = ROOM_QUERY_COMPLETE;
    Write(connfd, &nullRoom, sizeof (nullRoom));

}//END sendRoomList()

//  #######################################################
//  Adds the specified room to the roomList in the first available index
//  Send RegistrationMessge with type set to sucess or failue appropriately
//  #######################################################

void registerRoom(int connfd, RoomRecord* room) {
    RegistrationMessage message;

    memset(&message, 0, sizeof (message));

    if (roomCount == MAX_ROOMS) {
        printf("ERROR: Could not allocate space for new Room: Registration Refused!");
        message.type = REGISTER_FAILURE;
    } else {
        printf("Adding Room: %s\n", room->name);
        roomList[roomCount] = *room;
        roomCount++;
        message.type = REGISTER_SUCESS;
        message.record = *room;
    }//END if/else

    Write(connfd, &message, sizeof (message));

}//END registerRoom()

//  #######################################################
//  Removes the specified Room from roomList, Note: This does not result in a sparse array
//  #######################################################

void deregisterRoom(int connfd, RoomRecord* room) {
    int i;
    int sucess = FALSE;
    int deleteIndex = -1;

    for (i = 0; i < MAX_ROOMS; i++) {
        if (roomList[i].name == room->name && roomList[i].address == room->address) {
            deleteIndex = i;
            sucess = TRUE;
            break;
        }//END if
    }//END for

    if (sucess) {
        printf("Removing Room: %s, %s\n", roomList[deleteIndex].name, roomList[deleteIndex].address);
        roomCount--;
        roomList[deleteIndex] = roomList[roomCount];
    } else {
        printf("ERROR: Unable to find Room for Removal: %s, %s\n", roomList[i].name, roomList[i].address);
    }//END if

}//END deregisterRoom()

//  #######################################################
//  For Debugging Purposes: Adds dummy Rooms to the RoomList
//  #######################################################

void debugPopulateRoomList() {
    /*
     * fields in RoomRecord
     * int tcp;
     * char name[MAX_USER_ID_LENGTH];
     * char address[MAX_IPADDR_STRING_LENGTH];
     * int port;
     */
    int i;

    roomList[0] = (RoomRecord){TRUE, "Test Server 1", "111.111.111.111", 111};
    roomList[1] = (RoomRecord){TRUE, "Test Server 2", "122.122.122.122", 222};
    roomList[2] = (RoomRecord){TRUE, "Test Server 3", "133.133.133.133", 333};
    roomList[3] = (RoomRecord){TRUE, "Test Server 4", "144.144.144.144", 444};
    roomList[4] = (RoomRecord){TRUE, "Test Server 5", "155.155.155.155", 555};
    roomList[5] = (RoomRecord){TRUE, "Test Server 6", "166.166.166.166", 666};
    roomList[6] = (RoomRecord){TRUE, "Test Server 7", "177.177.177.177", 777};
    roomCount = 7;

    for (i = 0; i < roomCount; i++) {
        printf("Added Room: %s, %s, %d, %d\n", roomList[i].name, roomList[i].address, roomList[i].port, roomList[i].type);
    }//END for


}//END debugPopulateRoomList()
