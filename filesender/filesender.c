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
int sfd, cfd;

void handler(int sig) {
	close(sfd);
	close(cfd);
	_exit(1);
}

int main(int argc, char** argv) {
	if (argc != 3) goto ERROR;

	struct sigaction act;
	sigset_t set;
	memset(&act, 0, sizeof(act));
	act.sa_handler = handler;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	act.sa_mask = set;
	sigaction(SIGINT, &act, 0);

	struct addrinfo hints;
	struct addrinfo* res, *rp;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol=IPPROTO_TCP;
	hints.ai_flags=AI_PASSIVE;

	if (getaddrinfo("localhost", argv[1], &hints, &res) != 0) {
		perror("Get addr info");
		goto ERROR;
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (sfd == -1)
			continue;

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		if (close(sfd) < 0) {
			perror("Close");
			goto ERROR;
		}

	}

	if (rp == NULL) {
		perror("Socket list");
		goto ERROR;
	}

	if (listen(sfd, 100) < 0) {
		perror("Listen");
		close(sfd);
		goto ERROR;
	}

	struct sockaddr_un peer_addr;
	socklen_t peer_addr_size;
	peer_addr_size = sizeof(struct sockaddr_un);

	while ((cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size)) >= 0) {
		pid_t child;
		if ((child = fork()) == 0) {
			int file;
			if ((file = open(argv[2], O_RDONLY)) < 0) goto CHILD_ERROR;
			
			buf_t* buffer = buf_new(BUF_SIZE);
			int res;
			while ((res = buf_fill(file, buffer, BUF_SIZE)) > 0) {
				if (buf_flush(cfd, buffer, (buf_size(buffer))) < 0) {
					close(file);
					goto CHILD_ERROR;
				}
			}
			close(file);
			close(cfd);
			close(sfd);
			
			_exit(0);
CHILD_ERROR:
			perror("");
			close(cfd);
			close(sfd);
			_exit(EXIT_FAILURE);
		}
		if (child < 0) {
			close(cfd);
			close(sfd);
			goto ERROR;
		}
		if (close(cfd) < 0) {
			close(sfd);
			goto ERROR;
		}
	}
	close(sfd);
	return 0;
ERROR:
	return 1;
}
