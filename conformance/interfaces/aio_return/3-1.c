/*
 * Copyright (c) 2004, Bull SA. All rights reserved.
 * Created by:  Laurent.Vivier@bull.net
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this 
 * source tree.
 */

/*
 * assertion:
 *
 *	If the aiocbp is used to submit another asynchronous operation,
 *	then aio_return may be successfully used to retrieve the return status.
 *
 * method:
 *
 *	- open a file
 *	- fill in an aiocb for writing
 *	- call aio_write using this aiocb
 *	- call aio_return to get the aiocb status (number of bytes written)
 *	- reuse the aiocb for writing
 *	- call aio_write using this aiocb
 *	- call aio_return to get the aiocb status (number of bytes written)
 */

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <aio.h>

#include "posixtest.h"

#define TNAME "aio_return/3-1.c"

int main()
{
	char tmpfname[256];
#define BUF_SIZE 4096
	char buf[BUF_SIZE];
	int fd;
	struct aiocb aiocb;
	int retval;

#if _POSIX_ASYNCHRONOUS_IO != 200112L
	exit(PTS_UNSUPPORTED);
#endif

	snprintf(tmpfname, sizeof(tmpfname), "/tmp/pts_aio_return_3_1_%d", 
		  getpid());
	unlink(tmpfname);
	fd = open(tmpfname, O_CREAT | O_RDWR | O_EXCL,
		  S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		printf(TNAME " Error at open(): %s\n",
		       strerror(errno));
		exit(PTS_UNRESOLVED);
	}

	unlink(tmpfname);

	memset(buf, 0xaa, BUF_SIZE);
	memset(&aiocb, 0, sizeof(struct aiocb));
	aiocb.aio_fildes = fd;
	aiocb.aio_buf = buf;
	aiocb.aio_nbytes = BUF_SIZE;
	if (aio_write(&aiocb) == -1)
	{
		printf(TNAME " Error at aio_write(): %s\n",
		       strerror(errno));
		exit(PTS_FAIL);
	}

	do {
		retval = aio_error( &aiocb);
		if (retval == -1)
		{
			printf(TNAME " Error at aio_error(): %s\n",
				strerror(errno));
			exit(PTS_FAIL);
		}
	} while (retval == EINPROGRESS);

	retval = aio_return(&aiocb);
	if (retval != BUF_SIZE)
	{
		printf(TNAME " Error at aio_return(): %d, %s\n", retval,
		       strerror(errno));
		exit(PTS_FAIL);
	}

	memset(&aiocb, 0, sizeof(struct aiocb));
	aiocb.aio_fildes = fd;
	aiocb.aio_buf = buf;
	aiocb.aio_nbytes = BUF_SIZE/2;
	if (aio_write(&aiocb) == -1)
	{
		printf(TNAME " Error at aio_write(): %s\n",
		       strerror(errno));
		exit(PTS_FAIL);
	}

	do {
		retval = aio_error( &aiocb);
		if (retval == -1)
		{
			printf(TNAME " Error at aio_error(): %s\n",
				strerror(errno));
			exit(PTS_FAIL);
		}
	} while (retval == EINPROGRESS);

	close(fd);
	retval = aio_return(&aiocb);
	if (retval != BUF_SIZE / 2)
	{
		printf(TNAME " Error at aio_return(): %d, %s\n", retval,
		       strerror(errno));
		exit(PTS_FAIL);
	}
	
	printf ("Test PASSED\n");
	return PTS_PASS;
}
