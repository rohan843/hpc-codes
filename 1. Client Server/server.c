/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once

/*
Usage:
	long len;
	char* ptr = getFileAsBytes("b.txt", &len);
*/
char *getFileAsBytes(char* name, long* len)
{
	FILE *fileptr;
	char *buffer;
	long filelen;
	size_t tmp;

	fileptr = fopen(name, "rb"); // Open the file in binary mode
	if(!fileptr) return NULL;
	if(fseek(fileptr, 0, SEEK_END) != 0) return NULL;		 // Jump to the end of the file
	if((filelen = ftell(fileptr)) == -1) return NULL;			 // Get the current byte offset in the file
	rewind(fileptr);					 // Jump back to the beginning of the file

	buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
	fread(buffer, filelen, 1, fileptr);				 // Read in the entire file
	fclose(fileptr);								 // Close the file
	*len = filelen;
	return buffer;
}

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while (1)
	{ // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
				  get_in_addr((struct sockaddr *)&their_addr),
				  s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork())
		{				   // this is the child process
			close(sockfd); // child doesn't need the listener
			// S1. Receive 'a' or 'b' from client.
			int numbytes;
			char fileName[MAXDATASIZE];
			int test;
			char* fileBytes;
			long len;

			if ((numbytes = recv(new_fd, fileName, MAXDATASIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
			}
			printf("Received request for %c.txt\n", fileName[0]);
			// S2. Get the corresponding file in memory buffer.
			if(fileName[0] == 'a')
			{
				fileBytes = getFileAsBytes("a.txt", &len);
				test = 0;
			}
			else
			{
				fileBytes = getFileAsBytes("b.txt", &len);
				test = 1;
			}
			// S3. Send the file to the client.
			if(test == 0)
			{
				int bytes_sent;
				if((bytes_sent = send(new_fd, fileBytes, len, 0)) == -1)
				{
					fprintf(stderr, "client: failed to send file\n");
					exit(2);
				}
			}
			else
			{
				int bytes_sent;
				if((bytes_sent = send(new_fd, fileBytes, len, 0)) == -1)
				{
					fprintf(stderr, "client: failed to send file\n");
					exit(2);
				}
			}
			close(new_fd);
			exit(0);
		}
		close(new_fd); // parent doesn't need this
	}

	return 0;
}
