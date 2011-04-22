/**
 * Name: Andy Aloia and Nicholas Capo
 * Course: COSC 4653 - Advanced Networks
 * Assignment: PROJECT - Multiserver Chat Rooms in TCP and UDP
 * File name: wrapperFunctions.c
 * Purpose: Provides transparant error checking wrapper functions for use in a C networking project
 * Usage: None
 *
 * Limitations: 
 * Development Computer: x86
 * Operating System: Ubuntu Linux 10.04
 * Integrated Development Environment (IDE): Netbeans 6.9.1
 * Compiler: gcc (Ubuntu 4.4.3-4ubuntu5) 4.4.3
 * Operational Status: Compiles
 **/

//##############################################################

#define TRUE 1
#define FALSE 0
#define DEBUG TRUE

// GLOBALS ##############################################################

int socketList[MAX_SOCKETS];

//##############################################################

#include	<sys/types.h>	// basic system data types
#include	<sys/socket.h>	// basic socket definitions
#include	<netinet/in.h>	// sockaddr_in{} and other Internet defns
#include	<arpa/inet.h>	// inet(3) functions
#include	<errno.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<sys/wait.h>

#include "protocols.h"

//##############################################################

typedef void Sigfunc(int);

//##############################################################

void debug(char * message);
void cleanup();
void SigAction(int signum, Sigfunc* handler);
pid_t Fork();
int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
void Close(int connfd);
void displayConnectionInfo(int socket);
int Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void handleSigTerm(int signo);
void useStandardSignalHandlers();
void handleSigChld(int signo);
void handleSigPipe(int signno);

//##############################################################

void SigAction(int signum, Sigfunc* handler) {
    int status;
    struct sigaction act;

    debug("SigAction");

    act.sa_handler = handler;

    sigemptyset(&act.sa_mask);

    act.sa_flags = 0;
    if (signum == SIGALRM) {
        //Cygwin compatibility
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        act.sa_flags |= SA_RESTART;
    }//END if/else

    status = sigaction(signum, &act, NULL);

    if (status < 0) {
        perror("Signal Action Error");
        cleanup();
    }//END if
}//END Signal()

//##############################################################

void Close(int socket) {
    int status;
    socketList[socket] = SOCKET_NOT_CONNECTED;
    status = close(socket);
    debug("Close");
    if (status < 0) {
        perror("Close Error");
        //NOT CLEANUP() SINCE CLEANUP() CALLS CLOSE()
        exit(1);
    }//END if
}//END Close()

//##############################################################

void Listen(int sockfd, int backlog) {
    int status;

    debug("Listen");

    while (TRUE) {
        status = listen(sockfd, backlog);
        if (errno == EINTR) {
            continue;
        } else {
            break;
        }//END if/else
    }//END while

    if (status < 0) {
        perror("Listen Error");
        cleanup();
    } else {
        return;
    }//END if/else
}//END Listen

//##############################################################

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int connfd;

    debug("Accept");

    while (TRUE) {
        connfd = accept(sockfd, addr, addrlen);
        if (errno == EINTR) {
            continue;
        } else {
            break;
        }//END if/else
    }//END while

    if (connfd < 0) {
        perror("Accept Error");
        cleanup();
    }

    socketList[connfd] = connfd;

    return connfd;


}//END Accept()

//##############################################################

int Socket(int domain, int type, int protocol) {
    int socketfd;

    debug("Socket");

    while (TRUE) {
        socketfd = socket(domain, type, protocol);
        if (errno == EINTR) {
            continue;
        } else {
            break;
        }//END if/else
    }//END while

    if (socketfd < 0) {
        perror("Socket Error");
        cleanup();
    }

    socketList[socketfd] = socketfd;

    return socketfd;
}//END Socket()

//##############################################################

pid_t Fork() {
    int pid;

    debug("Fork");

    while (TRUE) {
        pid = fork();
        if (errno == EINTR) {
            continue;
        } else {
            break;
        }//END if/else
    }//END while

    if (pid < 0) {
        perror("Fork Error");
        cleanup();
    }//END if/else

    return pid;

}//END Fork()

//##############################################################

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int status;

    debug("Bind");

    while (TRUE) {
        status = bind(sockfd, addr, addrlen);
        if (errno == EINTR) {
            continue;
        } else {
            break;
        }//END if/else
    }//END while

    if (status < 0) {
        perror("Bind Error");
        cleanup();
    } else {
        return;
    }//END if/else

}//END Bind()

//##############################################################

ssize_t Write(int fd, const void *buf, size_t count) {
    int size;

    debug("Write");

    while (TRUE) {
        size = write(fd, buf, count);
        if (errno == EINTR) {
            continue;
        } else if (errno == ECONNRESET) {
            printf("Caught ECONNRESET: Trying again...\n");
        } else {
            break;
        }//END if/else
    }//END while

    if (errno == EPIPE) {
        printf("Caught EPIPE: Exiting\n");
        cleanup();
    } else if (errno == ETIMEDOUT) {
        printf("Caught ETIMEDOUT: Exiting\n");
        cleanup();
    } else if (errno == EHOSTUNREACH) {
        printf("Caught EHOSTUNREACH: Exiting\n");
        cleanup();
    } else if (errno == ENETUNREACH) {
        printf("Caught ENETUNREACH: Exiting\n");
        cleanup();
    } else if (size < 0) {
        perror("Read Error");
        cleanup();
    }//END if/else

    return size;

}//END Write()

//##############################################################

ssize_t Read(int fd, void *buf, size_t count) {
    int size;

    debug("Read");

    while (TRUE) {
        size = read(fd, buf, count);
        if (errno == EINTR) {
            continue;
        } else if (errno == ECONNRESET) {
            printf("Caught ECONNRESET: Trying again...\n");
        } else {
            break;
        }//END if/else
    }//END while

    if (errno == EPIPE) {
        printf("Caught EPIPE: Exiting\n");
        cleanup();
    } else if (errno == ETIMEDOUT) {
        printf("Caught ETIMEDOUT: Exiting\n");
        cleanup();
    } else if (errno == EHOSTUNREACH) {
        printf("Caught EHOSTUNREACH: Exiting\n");
        cleanup();
    } else if (errno == ENETUNREACH) {
        printf("Caught ENETUNREACH: Exiting\n");
        cleanup();
    } else if (size < 0) {
        perror("Read Error");
        cleanup();
    }//END if/else

    return size;


}//END Read()

//##############################################################

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int status;

    debug("Connect");

    while (TRUE) {
        status = connect(sockfd, addr, addrlen);
        if (errno == EINTR) {
            continue;
        } else if (errno == ECONNRESET) {
            printf("Caught ECONNRESET: Trying again...\n");
        } else {
            break;
        }//END if/else
    }//END while

    if (errno == EPIPE) {
        printf("Caught EPIPE: Exiting\n");
        cleanup();
    } else if (errno == ETIMEDOUT) {
        printf("Caught ETIMEDOUT: Exiting\n");
        cleanup();
    } else if (errno == EHOSTUNREACH) {
        printf("Caught EHOSTUNREACH: Exiting\n");
        cleanup();
    } else if (errno == ENETUNREACH) {
        printf("Caught ENETUNREACH: Exiting\n");
        cleanup();
    } else if (status < 0) {
        perror("Read Error");
        cleanup();
    }//END if/else

    return status;

}//END Read()

//##############################################################

void cleanup() {
    int i;
    //start at index 3 so as not to close stdin, stdout, and stderr
    for (i = 3; i < MAX_SOCKETS; i++) {
        if (socketList[i] == SOCKET_NOT_CONNECTED) {
            continue;
        } else {
            Close(socketList[i]);
        }//END if/else
    }//END for
    for (i = 3; i < MAX_SOCKETS; i++) {
        if (socketList[i] == SOCKET_NOT_CONNECTED) {
            continue;
        } else {
            printf("ERROR: Socket not closed after call to cleanup!!");
        }//END if/else
    }//END for

    exit(0);
}//END cleanup()

//##############################################################

int Kill(pid_t pid, int sig) {
    int status;

    debug("Kill");

    while (TRUE) {
        status = kill(pid, sig);
        if (errno == EINTR) {
            continue;
        } else if (errno == ECONNRESET) {
            printf("Caught ECONNRESET: Trying again...\n");
        } else {
            break;
        }//END if/else
    }//END while

    if (status < 0) {
        perror("Kill Error");
        cleanup();
    }//END if

    return status;
}//END Kill()

void debug(char * message) {
    if (DEBUG) {
        printf("DEBUG: %s\n", message);
    }//END if
}//END debug();

//##############################################################

void displayConnectionInfo(int socket) {
    struct sockaddr_in sa;
    socklen_t len;

    len = sizeof (sa);

    Getsockname(socket, (struct sockaddr *) &sa, &len);
    printf("(Local Node) IP Address: %s, Port: %d\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));

    Getpeername(socket, (struct sockaddr *) &sa, &len);
    printf("(Foreign Node) IP Address: %s, Port: %d\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));

}//END displayConnectionInfo()

//##############################################################

int Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    socklen_t len;
    int status;

    len = sizeof (addr);

    status = getsockname(sockfd, addr, &len);
    if (status < 0) {
        perror("Get Sock Name Error\n");
        cleanup();
    }//END if

    return status;
}//END Getsockname()

//##############################################################

int Getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    socklen_t len;
    int status;

    len = sizeof (addr);

    status = getpeername(sockfd, addr, addrlen);
    if (status < 0) {
        perror("Get Peer Name Error\n");
        cleanup();
    }//END if

    return status;
}//END Getpeername()

//#############################################################################
//Makes connection and returns the socket file descriptor
//#############################################################################

int makeConnection(int sockType, char* ipAddress, int port) {
    int socketfd;
    int status;
    struct sockaddr_in serverAddress;

    bzero(&serverAddress, sizeof (serverAddress));

    //Setup Server Address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    status = inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr);
    if (status != 1) {
        printf("Unable to resolve server IP");
        cleanup();
    }

    socketfd = Socket(AF_INET, sockType, 0);

    Connect(socketfd, (struct sockaddr *) & serverAddress, sizeof (serverAddress));

    return socketfd;
}//end makeconnection

//#############################################################################

void handleSigTerm(int signo) {
    printf("Caught SIGTERM: Exiting\n");
    cleanup();
}//END handleSigTerm()

//#############################################################################

void handleSigChld(int signo) {
    pid_t pid;
    int stat;
    pid = waitpid(-1, &stat, WNOHANG);

    while (pid > 0) {
        printf("Child #%d Terminated\n", pid);
        return;
    }//END while
}//END handlerSigChld()

//#############################################################################

void handleSigPipe(int signno) {
    printf("Broken Pipe: Exiting\n");
    cleanup();
}//END handleSigPipe()

//#############################################################################

void useStandardSignalHandlers() {
    SigAction(SIGTERM, handleSigTerm);
    SigAction(SIGCHLD, handleSigChld);
    SigAction(SIGPIPE, handleSigPipe);
}//END registerStandardHandlers()


//#############################################################################
