/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: roomServer.c
 * Purpose: 
 * Usage: Usage: roomServer.exe <port> <registration server IP> <registration server port> <max connected clients> <TCP/UDP (<1/0>)>
 *
 * Limitations: 
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

// GLOBALS ##########################################################

// PROTOTYPES #######################################################
void usage();
int TCPConnection();
int UDPConnection();
int startTCPConnection();
void mainLoop();
void sendMessage();
void receiveMessage();
// MAIN #######################################################

int main(int argc, char* argv[]) {

    if (argc != 6) {
        usage();
        exit(1);
    }

    return 0;
}
//  #######################################################
void mainLoop(){

}


//  #######################################################
int TCPConnection(){

return 0;	
}

//  #######################################################
int UDPConnection(){

return 0;	
}

//  #######################################################
int startTCPConnection(){

return 0;	
}

//  #######################################################
void sendMessage(){
	
}

//  #######################################################
void receiveMessage(){
	
}

//  #######################################################
void usage() {
    printf("Usage: roomServer.exe <port> <registration server IP> <registration server port> <max connected clients> <TCP/UDP  (<1/0>)>\n");
}

