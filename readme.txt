1. STUDENT NAMES: Andy Aloia and Nicholas Capo

2. COURSE NAME: COSC 4653 - Advanced Networks

3. ASSIGNMENT: #8 - Two Unique Servers

4. APPLICATION DESCRIPTION:
	****Two or more paragraphs describing what your application does overall and what each client and server program does.****

5. SOURCE CODE FILES:
	1. registrationServer.c
	2. roomServer.c
	3. client.c
	4. wrapperFunctions.c
	5 protocols.h

6. BUILD DIRECTIONS:
	See Makefile

7. RUN DIRECTIONS:
	1. Run the Registration Server:
		registrationserver.exe <port>
	2. Run 1-n Room Servers, each with unique Port numbers and unique Room Names:
		roomServer.exe <room Port> <regSrv IP> <regSrv Port> <Room Name>
	3. Run 1-n Clients, providing the Address and port of the Registration Server, and a nickname for your user
		client.exe <regSrv IP> <regSrv Port> <Nickname>

8. OPERATIONAL STATUS:****STATUS****

	Registration Server: ****STATUS****
		****BUGS****
		* It is possible for a Room Server to cause the Registration Server to block forever, this is because the room server cannot fork during a Registration or a De-registration Event. It does however fork for client connections.
		*
			
	Room Server:****STATUS****
		* Multi-Homeing of Room Servers has not been tested and is not expected to be reliable. The Room Server IP Address it determined based on the Registration Connection from the Room Server to the Registration Server, multi-homeing may make this unreliable.
		* 
	Client:****STATUS****
		*
		
		
