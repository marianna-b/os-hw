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
#include <poll.h>

#define _GNU_SOURCE

const size_t BUF_SIZE = 4096;

struct buffs {
	buf_t* buf[2];
	int mask[2];
};

int init_buffs(struct buffs* b) {
	b->buf[0] = buf_new(BUF_SIZE);
	if (b->buf[0] == NULL)
		return -1;
	b->mask[0] = 0;
	b->buf[1] = buf_new(BUF_SIZE);
	if (b->buf[1] == NULL) {
		buf_free(b->buf[0]);
		return -1;
	}
	b->mask[1] = 0;
	return 0;
}

void free_buffs(struct buffs* b) {
	buf_free(b->buf[0]);
	buf_free(b->buf[1]);
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
			return -1;
		}
	}
	freeaddrinfo(res);
	if (rp == NULL) {
		return -1;
	}
	return sfd;
}

struct pollfd fds[256];
int valid[256];
nfds_t n = 0;
struct buffs p[127];

void process_buff(int i, int idx1, int idx2, int m) {
	if (p[i].mask[m] == 0 && fds[idx1].revents & POLLIN) {
		int res;
		if ((res = buf_fill(fds[idx1].fd, p[i].buf[m], 1)) <= 0) {
			if (res == 0 || (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
				shutdown(fds[idx1].fd, SHUT_RD);
				if (buf_size(p[i].buf[m]) == 0) {
					shutdown(fds[idx2].fd, SHUT_WR);
					p[i].mask[m] = -1;
				} else {
					p[i].mask[m] = 1;
				}
			}
		}
	}
	if (p[i].mask[m] != -1 && fds[idx2].revents & POLLOUT) {
		int res;
		if ((res = buf_flush(fds[idx2].fd, p[i].buf[m], 1)) <= 0) {
			if (res == 0 || (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
				shutdown(fds[idx1].fd, SHUT_RD);
				shutdown(fds[idx2].fd, SHUT_WR);
				p[i].mask[m] = -1;
			}
		}
		if (buf_size(p[i].buf[m]) == 0 && p[i].mask[m] == 1)
			p[i].mask[m] = -1;
	}
	int size = buf_size(p[i].buf[m]);
	if (p[i].mask[m] != -1 && size != 0)
		fds[idx2].events |= POLLOUT;
	
	if (p[i].mask[m] == 0 && size != BUF_SIZE)
		fds[idx1].events |= POLLIN;
}


void swap_with_last(int i) {
	struct pollfd tmp = fds[i];
	fds[i] = fds[n - 2];
	fds[n - 2] = tmp;
	close(fds[n - 2].fd);

	tmp = fds[i + 1];
	fds[i + 1] = fds[n - 1];
	fds[n - 1] = tmp;
	close(fds[n - 1].fd);

	free_buffs(&p[(i - 2) / 2]);
	p[(i - 2) / 2] = p[(n - 4) / 2];
	n -= 2;
}

int add_pipe(int f1, int f2) {
	int idx = (n - 2)/ 2;
	if (init_buffs(&p[idx]) < 0)
		return -1;
	fds[n].fd = f1;
	fds[n].events = POLLIN;
	fds[n].revents = 0;
	n++;

	fds[n].fd = f2;
	fds[n].events = POLLIN;
	fds[n].revents = 0;
	n++;
	return 0;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		return 1;
	}

	struct addrinfo hints;
	struct addrinfo * res1, *res2;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int sfd1, sfd2, cfd1, cfd2;

	if (getaddrinfo("localhost", argv[1], &hints, &res1) != 0) {
		perror("Get addr info");
		return 1;
	}
	sfd1 = getfd(res1);
	if (sfd1 < 0) {
		perror("Get socket");
		return 1;
	}
	if (listen(sfd1, 100) < 0) {
		perror("Listen");
		close(sfd1);
		return 1;
	}
	
	if (getaddrinfo("localhost", argv[2], &hints, &res2) != 0) {
		perror("Get addr info");
		return 1;
	}
	sfd2 = getfd(res2);
	if (sfd2 < 0) {
		perror("Get socket");
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

	fds[0].fd = sfd1;
	fds[1].fd = -1 * sfd2;
	fds[0].events = fds[1].events = POLLIN;
	n = 2;

	int timeout = 200;
	int res;
	while (1) {
		res = poll(fds, n, timeout);
		if (res == 0)
			continue;
		if (res < 0) {
			if (errno == EINTR)
				continue;
		}

		int i;
		for (i = 0; i < (n - 2) / 2; i++) {
			int idx = 2 * i + 2;
			fds[idx].events = 0;
			fds[idx + 1].events = 0;
			process_buff(i, idx, idx + 1, 0);
			process_buff(i, idx + 1, idx, 1);
		}
		for (i = 0; i < (n - 2) / 2; i++) {
			int idx = 2 * i + 2;
			if (p[i].mask[0] == -1 && p[i].mask[1] == -1) {
				swap_with_last(idx);
			}
		}

		if (n < 256) {
			if (fds[0].revents & POLLIN) {
				fds[0].fd *= -1;
				fds[1].fd *= -1;
				cfd1 = accept4(sfd1, (struct sockaddr *) &peer_addr, &peer_addr_size, SOCK_NONBLOCK);
				if (cfd2 == -1) {
					perror("Accept");
					goto ERROR;
				}
			} else if (fds[1].revents & POLLIN) {
				fds[0].fd *= -1;
				fds[1].fd *= -1;

				cfd2 = accept4(sfd2, (struct sockaddr *) &peer_addr, &peer_addr_size, SOCK_NONBLOCK);
				if (cfd2 == -1) {
					perror("Accept");
					goto ERROR;
				}
				if (add_pipe(cfd1, cfd2) < 0) {
					if (close(cfd1) < 0)
						perror("Close");
					if (close(cfd2) < 0)
						perror("Close");
				}
			}
			if (n == 256)
				fds[0].fd *= -1;
			if (n < 256 && fds[0].fd < 0 && fds[1].fd < 0)
				fds[0].fd *= -1;
		}
	}
	perror("Poll");
	int i;
ERROR:
	for (i = 0; i < n; i++) {
		close(fds[i].fd);
	}
	return 1;
}
