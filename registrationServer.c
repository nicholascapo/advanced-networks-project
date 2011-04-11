/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: registrationServer.c
 * Purpose: 
 * Usage: registrationServer.exe <port>
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
// MAIN #######################################################

int main(int argc, char* argv[]) {

    if (argc != 2) {
        usage();
        exit(1);
    }

    return 0;
}

//  #######################################################

void usage() {
    printf("Usage: registrationServer.exe <port>\n");
}

//  #######################################################