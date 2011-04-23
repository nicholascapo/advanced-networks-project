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

#define QUIT_COMMAND "/quit"

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

    useStandardSignalHandlers();

    //Setup Connection Specs
    ipAddr = argv[1];
    port = atoi(argv[2]);
    username = argv[3];

    //make a TCP connection to the registration server
    socketfd = makeConnection(SOCK_STREAM, ipAddr, port);

    roomCount = readRoomList(socketfd, roomList);

    Close(socketfd);

    room = roomList[getRoomChoice(roomList, roomCount)];

    //make a connection to the Room Server
    socketfd = makeConnection(room.type, room.address, room.port);

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
//Reads an array of RoomRecords from the server, storing them in roomList
// Returns the number of rooms read
//#############################################################################

int readRoomList(int socketfd, RoomRecord* roomList) {

    RoomRecord tempRoom;
    RegistrationMessage request;
    int index = -1;

    memset(&request, 0, sizeof (request));

    //Send RoomList Query
    request.type = ROOM_QUERY;
    Write(socketfd, &request, sizeof (request));

    while (TRUE) {
        Read(socketfd, &tempRoom, sizeof (tempRoom));

        if (DEBUG) {
            printf("Read Room: %s, %s, %d, %d\n", tempRoom.name, tempRoom.address, tempRoom.port, tempRoom.type);
        }//END if

        //check for EOF
        if (tempRoom.type == ROOM_QUERY_COMPLETE) {
            debug("Read EOF Room\n");
            break;
        } else {
            index++;
            memcpy(&roomList[index], &tempRoom, sizeof (tempRoom));

        }//END if/else

    }//END while

    if (index < 0) {
        printf("ERROR: No Servers were Found, please try again later...\n");
        exit(1);
    }//END if

    if (DEBUG) {
        printf("Received %d Rooms from RegistrationServer", index + 1);
    }//END if
    return index + 1;
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
        printf("ERROR: Child Process Exited Prematurely!!\n");
        //Only the parent should call cleanup()
        exit(1);
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
    ChatMessage message;

    memset(&message, 0, sizeof (message));

    message.status = status;
    strncpy(message.user, username, MAX_USER_ID_LENGTH);

    Write(socketfd, &message, sizeof (message));

}//END sendJoin()

//#############################################################################
//Reads messages from the socket and prints them for the user
//#############################################################################

void userOutput(int socketfd) {
    ChatMessage message;
    fd_set rset;
    fd_set allset;
    int n;
    FD_SET(socketfd, &allset);

    while (TRUE) {
        rset = allset;
        if (FD_SET(socketfd, &rset)) {
            n = Read(socketfd, &message, sizeof (message));
            if (n == 0) { //connection closed by client
                printf("Server closed Connection: Exiting\n");
                //We will always detect a remote close here sooner than in the parent,
                // thus when we catch that here, we send the parent a SIGTERM
                Kill(getppid(), SIGTERM);
                break;
            }
            printf("%s : %s\n", message.user, message.text);
        }
        bzero(&message, sizeof (message));
    }//END while

}//END output()

//#############################################################################
//Reads messages from the user and writes them to the server
//#############################################################################

void userInput(int socketfd, char* username) {
    ChatMessage message;
    char text[MAX_MESSAGE_TEXT];

    while (TRUE) {
        strncpy(message.user, username, MAX_USER_ID_LENGTH);
        message.status = STATUS_ONLINE;

        fgets(text,MAX_MESSAGE_TEXT,stdin);


        if (strncmp(text, QUIT_COMMAND, sizeof (QUIT_COMMAND)) == 0) {
            debug("Got Quit Command");
            break;
        }//END if
        sprintf(message.text, "%s\n", text);
        //strncpy(message.text, text, MAX_MESSAGE_TEXT);

        Write(socketfd, &message, sizeof (message));
        bzero(&text, sizeof (text));

    }//END while

}//END input()
