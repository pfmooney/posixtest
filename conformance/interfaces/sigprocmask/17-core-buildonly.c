/*   
 * Copyright (c) 2003, Intel Corporation. All rights reserved.
 * Created by:  salwan.searty REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this 
 * source tree.

   Testing passing an invalid signals to sighold().
   After sighold is called on an invalid signal, sigignore() should 
return -1 and set errno to EINVAL
   
   The invalid signal passed to sigignore() depends on the argument 
passed to this program. There are currently 4 invalid signals.
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include "posixtest.h"

int main(int argc, char *argv[])
{
	int r=rand();
	sigset_t set;

	sigaddset(&set, SIGABRT);
	
	if (sigprocmask(r, &set, NULL) == -1) {
		if (EINVAL == errno) {
			printf ("errno set to EINVAL\n");
			return PTS_PASS;
		} else {
			printf ("errno not set to EINVAL\n");
			return PTS_FAIL;
		}
	}
	
	printf("sighold did not return -1\n");
	return PTS_FAIL;
}
