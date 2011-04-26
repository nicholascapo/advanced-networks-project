

build: client roomServer registrationServer

client: client.c wrapperFunctions.c protocols.h
	gcc -Wall client.c -o client.exe

roomServer: roomServer.c wrapperFunctions.c protocols.h
	gcc -Wall roomServer.c -o roomServer.exe

registrationServer: registrationServer.c wrapperFunctions.c protocols.h
	gcc -Wall registrationServer.c -o registrationServer.exe

clean:
	rm registrationServer.exe
	rm roomServer.exe
	rm client.exe
