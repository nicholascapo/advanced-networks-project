/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: client.c
 * Purpose: 
 * Usage: 
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
#define	MAX_LINE_LENGTH	256	      // max text line length 
#define	SA struct sockaddr
// GLOBALS ##########################################################

// PROTOTYPES #######################################################
void checkArgc(int argc);
char readInput(int requester);
void runMenu(int socketfd);
void printMenu();
int makeConnection(int conenctTCP,struct sockaddr_in serverAddress);

// MAIN #######################################################

int main(int argc, char* argv[]){

//Local Variables
int socketfd;
struct sockaddr_in serverAddress;
unsigned short port;
int connectTCP;
int status;
//Check Argc for correct requirements
checkArgc(argc);

//Setup Connection Specs
port = atoi(argv[2]);
connectTCP = atoi(argv[3]);
bzero(&serverAddress, sizeof(serverAddress));


//Setup Server Address
serverAddress.sin_family = AF_INET;
serverAddress.sin_port   = htons(port);
status = inet_pton(AF_INET, argv[1], &serverAddress.sin_addr);
if (status != 1){
	printf("Unable to resolve server IP");
	exit(1);
}

//make connection
socketfd = makeConnection(connectTCP, serverAddress);

//Run infinite loop menu
runMenu(socketfd);	

//should bever reach here...as exit(1); should occur
return 0;

}//end main

//#############################################################################
// Checks ARGC for correct number of arguments, prints usage otherwise
//#############################################################################
void checkArgc(int argc){
if (argc != 4){
   fprintf(stderr, "Usage: client <Server IP address> <Server Port> <TCP/UDP {1/0}>\n");
   exit(1);
   } // End if        
}//end checkArgc()

//#############################################################################
//Prints menu and performs action
//#############################################################################
void runMenu(int socketfd){
char character;
while(1){
	printMenu();
	character = readInput(1);
	switch (character){
		case '0':
			
		break;
		case '1':
			
		break;
		case '2':
			
		break;
		case '3':
			Close(socketfd);
			exit(1);
		break;
		default:
			printf("Unrecognized Command\n");
		break;
	}//end switch
	//Null
	character = '\0';
}//end while
}//end runmenu()

//#############################################################################
//Prints the intial menu
//#############################################################################
void printMenu(){
printf("****************************************\n");
printf("* This is a stub for the menu\n");
printf("* 3 - Exit Program\n");
printf("****************************************\n\n");
printf("Enter a command (0-3):\n");
}//end printmenu()

//#############################################################################
//Reads from the command line, if requester == 1 then return a single character
//#############################################################################
char readInput(int requester){
	char buffer[MAX_LINE_LENGTH];
	scanf ("%s", buffer);
	
	if(requester == 1){
		return buffer[0];
	}//end if
	return *buffer;
}//end readinput()

//#############################################################################
//Makes connection and returns the socket file descriptor
//#############################################################################
int makeConnection(int connectTCP, struct sockaddr_in serverAddress){
int socketfd;
int status;
if(connectTCP == 1){ 
	//TCP Setup
	socketfd = Socket(AF_INET,SOCK_STREAM,0);
	status = connect(socketfd, (SA *) &serverAddress, sizeof(serverAddress));
	if (status != 0){
		printf("Unable to connect server, exiting.");
		exit(1);
	}//end if
}else{
	//UDP Setup
	socketfd = Socket(AF_INET,SOCK_DGRAM,0);
}//end if
return socketfd;
}//end makeconnection
