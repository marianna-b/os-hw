#include <helpers.h>
#include <bufio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>

const size_t BUF_SIZE = 4096;
int sfd1, sfd2, cfd1, cfd2;

void closenexit(int st) {
	close(sfd1);
	close(sfd2);
	close(cfd1);
	close(cfd2);
	_exit(st);
}

void handler(int sig) {
	closenexit(1);
}

int getfd(struct addrinfo* res) {
	int sfd;
	struct addrinfo* rp;
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (sfd == -1)
			continue;

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		if (close(sfd) < 0) {
			freeaddrinfo(res);
			perror("Close");
			closenexit(1);
		}
	}
	freeaddrinfo(res);
	if (rp == NULL) {
		perror("Socket list");
		closenexit(1);
	}
	return sfd;
}

void forward(int f1, int f2) {

	buf_t* buffer = buf_new(BUF_SIZE);
	if (buffer == NULL)
		closenexit(EXIT_FAILURE);
	
	int res;
	while ((res = buf_fill(f1, buffer, 1)) > 0) {
		if (buf_flush(f2, buffer, (buf_size(buffer))) < 0) {
			perror("Buffer");
			buf_free(buffer);
			closenexit(EXIT_FAILURE);
		}
	}
	buf_free(buffer);
	if (res < 0) {
		perror("Buffer");
		closenexit(1);
	}
	closenexit(0);
}



int main(int argc, char** argv) {
	if (argc != 3) {
		return 1;
	}

	struct sigaction act;
	sigset_t set;
	memset(&act, 0, sizeof(act));
	act.sa_handler = handler;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	act.sa_mask = set;
	sigaction(SIGINT, &act, 0);

	struct addrinfo hints;
	struct addrinfo * res1, *res2;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo("localhost", argv[1], &hints, &res1) != 0) {
		perror("Get addr info");
		return 1;
	}
	sfd1 = getfd(res1);
	if (getaddrinfo("localhost", argv[2], &hints, &res2) != 0) {
		perror("Get addr info");
		return 1;
	}
	sfd2 = getfd(res2);

	if (listen(sfd1, 100) < 0) {
		perror("Listen");
		close(sfd1);
		return 1;
	}

	if (listen(sfd2, 100) < 0) {
		perror("Listen");
		close(sfd2);
		return 1;
	}

	struct sockaddr_un peer_addr;
	socklen_t peer_addr_size;
	peer_addr_size = sizeof(struct sockaddr_un);

	while ((cfd1 = accept(sfd1, (struct sockaddr *) &peer_addr, &peer_addr_size)) >= 0) {
		if ((cfd2 = accept(sfd2, (struct sockaddr *) &peer_addr, &peer_addr_size)) < 0)
			break;

		pid_t child1, child2;
		if ((child1 = fork()) == 0) {
			forward(cfd1, cfd2);
		}
		if ((child2 = fork()) == 0) {
			forward(cfd2, cfd1);
		}
		if (child1 < 0 || child2 < 0) {
			perror("fork");
			closenexit(1);
		}
		if (close(cfd1) < 0 || close(cfd2) < 0) {
			perror("close");
			goto ERROR;
		}
	}
	perror("Accept");
	close(cfd1);
	close(cfd2);
ERROR:
	close(sfd1);
	close(sfd2);
	return 1;
}
