/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: client.c
 * Purpose: Creates a TCP connection to the registration server to receive chat 
 * messages. Allows the user to send messages to the room
 * 
 * Copyright (C) 2011 Andy Aloia and Nicholas Capo
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 **/

//INCLUDES ##########################################################
#include "protocols.h"
#include "wrapperFunctions.c"

// CONSTANTS ########################################################

#define COMMAND_QUIT "/quit"
#define COMMAND_USER_QUERY "/who"

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

    memset(&room, 0, sizeof (room));

    //make a TCP connection to the registration server
    socketfd = makeConnection(SOCK_STREAM, ipAddr, port);

    roomCount = readRoomList(socketfd, roomList);

    Close(socketfd);

    room = roomList[getRoomChoice(roomList, roomCount)];

    //make a connection to the Room Server
    socketfd = makeConnection(room.type, room.address, room.port);

    sendStatus(socketfd, username, STATUS_JOIN);

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

    pid = Fork();

    if (pid == 0) {
        //CHILD
        userOutput(socketfd);
        debug("ERROR: Child Process Exited Prematurely!!\n");
        //Only the parent should call cleanup()
        exit(1);
    }//END if

    //PARENT
    userInput(socketfd, username);

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
    int n;

    while (TRUE) {
        n = Read(socketfd, &message, sizeof (message));
        if (n == 0) { //connection closed by client
            printf("Server closed Connection: Exiting\n");
            //We will always detect a remote close here sooner than in the parent,
            // thus when we catch it here, we send the parent a SIGTERM
            Kill(getppid(), SIGTERM);
            break;
        }
        printf("%s : %s", message.user, message.text);
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
        bzero(&text, sizeof (text));
        strncpy(message.user, username, MAX_USER_ID_LENGTH);
        message.status = STATUS_ONLINE;

        fgets(text, MAX_MESSAGE_TEXT, stdin);

        if (strncmp(text, COMMAND_QUIT, sizeof (COMMAND_QUIT - 1)) == 0) {
            debug("Got Quit Command");
            sendStatus(socketfd, username, STATUS_LEAVE);
            break;
        } else if (strncmp(text, COMMAND_USER_QUERY, sizeof (COMMAND_USER_QUERY - 1)) == 0) {
            printf("USER QUERY\n");
            message.status = STATUS_USER_QUERY;
        } else {
            //sprintf(message.text, "%s", text);
            strncpy(message.text, text, MAX_MESSAGE_TEXT);
        }//END if/else

        Write(socketfd, &message, sizeof (message));


    }//END while

}//END input()
