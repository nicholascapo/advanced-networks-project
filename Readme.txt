1. STUDENT NAMES: Andy Aloia and Nicholas Capo

2. COURSE NAME: COSC 4653 - Advanced Networks

3. ASSIGNMENT: #8 - Two Unique Servers

4. APPLICATION DESCRIPTION:
	A set of applications for operating a series of chat rooms.
	Clients must only know the address and port of the central registration server in order to get a list of all available rooms, and connect to a room.
	Once a client has joined a room, he is then able to join the chat already in progress and can see messages sent from every other client.

	Clients may use the special message "/quit" to leave the room and exit the program.

	On the back end, a single Registration server must be setup to provide a "directory" service for Rooms.

	Rooms can be run by anyone on an accessible network, the room must register with a registration server and then clients will be able to connect to it.

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
		i.e. registrationserver.exe <port>
	2. Run 1-MAX_ROOMS (Currently 100) Room Servers, each with unique Port numbers and unique Room Names:
		i.e. roomServer.exe <room Port> <regSrv IP> <regSrv Port> <Room Name>
	3. Run 1-MAX_CLIENTS (Currently 32) Clients, providing the Address and port of the Registration Server, and a nickname for your user
		i.e. client.exe <regSrv IP> <regSrv Port> <Nickname>

8. OPERATIONAL STATUS: COMPLETE

	Registration Server: COMPLETE
		* It is possible for a Room Server to cause the Registration Server to block forever,
			this is because the room server cannot fork during a Registration or a De-registration Event. It does however fork for client connections.
			
	Room Server: COMPLETE
		* Multi-Homeing of Room Servers has not been tested and is not expected to be reliable.
			The Room Server IP Address it determined based on the Registration Connection from the Room Server to the Registration Server,
			multi-homeing may make this unreliable.
		* UDP Room Servers are Not supported at this time, although it should not be difficult to implement.

	Client: COMPLETE
		*
