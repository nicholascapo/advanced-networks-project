/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: protocols.h
 * Purpose: Provides structs for communication types
 * Usage: None
 *
 * Limitations: 
 * Development Computer: x86
 * Operating System: Ubuntu Linux 10.04
 * Integrated Development Environment (IDE): Netbeans 6.9.1
 * Compiler: gcc (Ubuntu 4.4.3-4ubuntu5) 4.4.3
 * Operational Status: 
 **/
 
#ifndef PROTOCOLS
#define PROTOCOLS

#define MAX_USER_ID_LENGTH 30
#define MAX_MESSAGE_TEXT 160
#define MAX_IPADDR_STRING_LENGTH 15

#define STATUS_JOIN 0
#define STATUS_ONLINE 1
#define STATUS_LEAVE 2
#define REGISTER_NONE 3
#define REGISTER_REQUEST 4
#define REGISTER_CONFIRM 5
#define REGISTER_LEAVE 6

 // Used for Registration of Room Server
 // and room lists sent to client by Registration Server
 typedef struct {
 	char name[MAX_USER_ID_LENGTH];
  	char address[MAX_IPADDR_STRING_LENGTH];
 	int request;
 }Room;
 
 // Normal Chat Messages
 typedef struct{
 	char user[MAX_USER_ID_LENGTH];
 	int status;
 	char text[MAX_MESSAGE_TEXT];
 }ChatMessage;


#endif 

