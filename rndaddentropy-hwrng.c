/*
 * Copyright (c) 2021 Jean Raby <jean@raby.sh>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/random.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define ENTSIZE 128
#define DEFAULT_EC 2048 // entropy count - enough to allow ssh-keygen

int current_entropy(int fd) {
	int entropy_count;
	if (ioctl(fd, RNDGETENTCNT, &entropy_count) != 0) {
		err(1, "RNDGETENTCNT");
	}
	return entropy_count;
}

int main(int argc, char **argv) {
	struct {
		int entropy_count;
		int buf_size;
		__u32 buf[ENTSIZE];
	} ent;

	int random_fd, hwrng_fd;
	int d;
	int target_ec;
	char *target_ec_env;

	target_ec_env = getenv("TARGET_EC");
	if (!target_ec_env) {
		target_ec = DEFAULT_EC;
	} else {
		errno = 0;
		char *endptr;
		target_ec = strtol(target_ec_env, &endptr, 10);
		if (endptr == target_ec_env || errno != 0 || target_ec < 0 || target_ec > 4096) {
			errx(1, "Invalid TARGET_EC value");
		}
	}

	if ((hwrng_fd = open("/dev/hwrng", O_RDONLY)) == -1) {
		err(1, "Error opening /dev/hwrng");
	}
	if ((random_fd = open("/dev/urandom", O_RDWR)) == -1) {
		err(1, "Error opening /dev/urandom");
	}

	ent.entropy_count = ENTSIZE * 8; /* in the hwrng we trust */
	ent.buf_size = ENTSIZE;
	while (current_entropy(random_fd) < target_ec) {
		if (d = read(hwrng_fd, ent.buf, ENTSIZE) != ENTSIZE) {
			errx(1, "Short read from hwrng");
		} else if (d == -1) {
			err(1, "Failed reading from hwrng");
		}
		if (ioctl(random_fd, RNDADDENTROPY, &ent) != 0) {
			err(1, "failed to add entropy");
		}
	}
}
