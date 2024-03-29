/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: protocols.h
 * Purpose: Provides structs for communication types
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
#define STATUS_USER_QUERY 3
#define STATUS_USER_QUERY_COMPLETE 4

#define REGISTER_REQUEST 5
#define REGISTER_LEAVE 6

#define REGISTER_SUCESS 7
#define REGISTER_FAILURE 8
#define REGISTER_FAILURE_NO_MEM 9
#define REGISTER_FAILURE_NAME_EXISTS 10

#define ROOM_QUERY 11
#define ROOM_QUERY_COMPLETE 12

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

typedef struct {
    int valid;
    int socket; //Only used in TCP Room Server
    char name[MAX_USER_ID_LENGTH]; //used in both TCP and UDP
    char address[INET_ADDRSTRLEN]; //Only used in UDP Room Server
    int port; //Only used in UDP Room Server
} ClientRecord;

#endif 
