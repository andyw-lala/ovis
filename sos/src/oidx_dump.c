/*
 * Copyright (c) 2012 Open Grid Computing, Inc. All rights reserved.
 * Copyright (c) 2012 Sandia Corporation. All rights reserved.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the U.S. Government.
 * Export of this program may require a license from the United States
 * Government.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the BSD-type
 * license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 *      Neither the name of Sandia nor the names of any contributors may
 *      be used to endorse or promote products derived from this software
 *      without specific prior written permission. 
 *
 *      Neither the name of Open Grid Computing nor the names of any
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission. 
 *
 *      Modified source versions must be plainly marked as such, and
 *      must not be misrepresented as being the original software.                                          
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Author: Tom Tucker tom at ogc dot us
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <getopt.h>
#include <sys/fcntl.h>

#include "oidx.h"
#include "../config.h"
#include "oidx_priv.h"

struct walk_s {
	FILE *fp;
};

char keybuf[256*3+1];
int obj_id_as_str(oidx_t oidx,
		  oidx_key_t key, size_t keylen,
		  uint64_t obj, void *context)
{
	static char str_buf[32];
	struct walk_s *w = context;
	unsigned char *cp = (unsigned char *)&obj;
	int i;
	keybuf[0] = '\0';
	for (i = 0; i < keylen; i++) {
		sprintf(str_buf, "%02X:", ((uint8_t*)key)[i]);
		strcat(keybuf, str_buf);
	}
	fprintf(w->fp,
		"%s \"%02x %02x %02x %02x %02x %02x %02x %02x\"\n", 
		keybuf,
		cp[7], cp[6], cp[5], cp[4],
		cp[3], cp[2], cp[1], cp[0]);
	return 0;
}

#define FMT "qz"
void usage(int argc, char *argv[])
{
	printf("usage: %s [-q] [-z] <filename>\n"
	       "          -q    Quiet output.\n"
	       "          -z    Show free space data from object store.\n",
	       argv[0]);
	exit(1);
}

oidx_t oidx;
int main(int argc, char *argv[])
{
	oidx_t oidx;
	int show_free = 0;
	int quiet = 0;
	int op;

	while ((op = getopt(argc, argv, FMT)) != -1) {
		switch (op) {
		case 'q':
			quiet = 1;
			break;
		case 'z':
			show_free = 1;
			break;
		case '?':
		default:
			usage(argc, argv);
		}
	}
	if (optind == argc)
		usage(argc, argv);

	for (op = optind; op < argc; op++) {
		oidx = oidx_open(argv[op], O_RDWR | O_CREAT, 0660);
		if (!oidx) {
			printf("No OIDX file found for %s\n", argv[0]);
			continue;
		}
		struct walk_s walk;
		walk.fp = stdout;
		oidx_walk(oidx, obj_id_as_str, &walk);
		if (show_free)
			ods_dump(oidx->ods, stdout);
	}
	return 0;
}

