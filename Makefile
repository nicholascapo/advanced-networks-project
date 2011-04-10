

build: client roomServer registrationServer

client: client.c
	gcc -Wall client.c -o client.exe

roomServer: roomServer.c
	gcc -Wall roomServer.c -o roomServer.exe

registrationServer: registrationServer.c
	gcc -Wall registrationServer.c -o registrationServer.exe

clean:
	rm registrationServer.exe
	rm roomServer.exe
	rm client.exe
