/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: protocols.h
 * Purpose: Provides structs for communication types
 **/

#ifndef PROTOCOLS
#define PROTOCOLS

#include <netinet/in.h>

#define SOCKET_NOT_CONNECTED -1

#define MAX_USER_ID_LENGTH 30
#define MAX_MESSAGE_TEXT 160
#define MAX_ROOMS 100
#define MAX_CLIENTS 32
#define MAX_SOCKETS MAX_CLIENTS * 2

#define	MAX_LISTEN_QUEUE_LENGTH 10

#define STATUS_JOIN 0
#define STATUS_ONLINE 1
#define STATUS_LEAVE 2

#define REGISTER_REQUEST 4
#define REGISTER_LEAVE 5

#define REGISTER_SUCESS 6
#define REGISTER_FAILURE 7
#define REGISTER_FAILURE_NO_MEM 8
#define REGISTER_FAILURE_NAME_EXISTS 9

#define ROOM_QUERY 10
#define ROOM_QUERY_COMPLETE 11

typedef struct {
    int type; //Must be a valid sock type (e.g. SOCK_STREAM or SOCK_DGRAM)
    char name[MAX_USER_ID_LENGTH];
    char address[INET_ADDRSTRLEN];
    int port;
} RoomRecord;

// Used for Registration of Room Server
// and room lists sent to client by Registration Server

typedef struct {
    int type;
    RoomRecord record;
} RegistrationMessage;

// Normal Chat Messages

typedef struct {
    char user[MAX_USER_ID_LENGTH];
    int status;
    char text[MAX_MESSAGE_TEXT];
} ChatMessage;

#endif 
