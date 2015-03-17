/* -*- c-basic-offset: 8 -*-
 * Copyright (c) 2015 Open Grid Computing, Inc. All rights reserved.
 * Copyright (c) 2015 Sandia Corporation. All rights reserved.
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

/**
 * \file bassoc.c
 * \brief Baler association rule mining utility
 * \author Narate Taerat (narate at ogc dot us)
 */

/**
 * \page bassoc Baler association rule mining utility
 *
 * \section desc DESCRIPTION
 * This is a Baler's association rule mining utility. It can initiate workspace,
 * extract images information from balerd's store, and mine association rules
 * from the extracted images. Please see \ref example for usage examples.
 *
 * \section synopsis SYNOPSIS
 * create workspace:
 * \par
 * \code{.sh}
 *     bassoc -c -w WORKSPACE [-t NUMBER] [-n NUMBER]
 * \endcode
 *
 * extract images:
 * \par
 * \code{.sh}
 *     bassoc -w WORKSPACE -s BALERD_STORE [-B TS] [-E TS] [-H IDS] -R RECIPE_FILE
 *
 *     bassoc -w WORKSPACE -s BALERD_STORE [-B TS] [-E TS] [-H IDS] -r RECIPE1 -r RECIPE2 ...
 * \endcode
 *
 * mine for associations:
 * \par
 * \code{.sh}
 *     bassoc -w WORKSPACE [-o NUMBER] -m TARGET_LIST
 *
 *     bassoc -w WORKSPACE [-o NUMBER] -M TARGET_FILE
 * \endcode
 *
 * \section options OPTIONS
 *
 * \b NOTE: The order of the parameters are not important.
 *
 * \par -h,--help
 * Show help message.
 *
 * \par -i,--info
 * Show information of the workspace.
 *
 * \par -c,--create
 * Create and initialize workspace (-w). If the workspace existed, the program
 * will exit with error.
 *
 * \par -w,--workspace WORKSPACE_DIR
 * Workspace directory. With '-i', the WORKSPACE_DIR will be created, or the
 * program exits with error if WORKSPACE_DIR existed. Without '-i', the program
 * will exit with error if the workspace does not exist or not in a good state.
 *
 * \par -t,--sec-per-pixel NUMBER
 * The number of seconds in a pixel of the image. This option is used only with
 * create mode ('-c'). If not specified, the default value is 3600.
 *
 * \par -n,--node-per-pixel NUMBER
 * The number of nodes in a pixel of the image. This option is used only with
 * create mode ('-c'). If not specified, the default value is 1.
 *
 * \par -x,--extract
 * This option will make bassoc run in image extraction mode.
 *
 * \par -s,--store STOR_DIR
 * This is a path to balerd's store. This option is needed for image extraction.
 *
 * \par -B,--ts-begin TIME_STAMP
 * The beginning timestamp for the image extraction (-x). TIME_STAMP can be
 * either in 'seconds since Epoch' or 'yyyy-mm-dd HH:MM:SS'. If not specified,
 * the earliest time in the store is used.
 *
 * \par -E,--ts-end TIME_STAMP
 * The ending timestamp for the image extraction (-x). TIME_STAMP can be either
 * in 'seconds since Epoch' or 'yyyy-mm-dd HH:MM:SS'. If not specified, the
 * latest timestamp in the database is used.
 *
 * \par -H,--host-ids HOST_ID_LIST
 * This option is for image extraction (-x). It is a comma separated list of
 * ranges of host IDs (e.g. '2,4,6-10,29'). If not specified, all host IDs will
 * be included.
 *
 * \par -r,--recipe RECIPE
 * A RECIPE is described as 'NAME:PTN_ID_LIST'. This option is used with extract
 * (-x) option. An image, of name NAME will be created with occurrences of
 * patterns specified in PTN_ID_LIST. The PTN_ID_LIST is in the same format as
 * HOST_ID_LIST above. The NAME is [A-Za-z0-9._]+ and must be unique in the
 * workspace. This option is repeatable, i.e. if you give -r a:555 -r
 * b:556-570 -r c:600 in the command-line arguments, three images a, b, and c
 * will be created from pattern ID 555, 556-570 and 600 respectively.
 *
 * \par
 * \b NOTE: This is useful if you have only a handful of images to generate. If
 * you have more images to extract, it is advisable to use '-R' option instead.
 *
 * \par -R,--recipe-file IMG_RECIPE_FILE
 * IMG_RECIPE_FILE is a text file, each line of which contains a recipe to
 * create an image. The format of each line is as following:
 *
 * \par
 * \code{.sh}
 *     NAME:PTN_ID_LIST
 *     # COMMENT
 * \endcode
 *
 * \par
 * The PTN_ID_LIST is in the same format as HOST_ID_LIST (list or
 * comma-separated ranges). The NAME is [A-Za-z0-9._]+ and must be unique in the
 * workspace. If the image of the same name should also not exist in the
 * existing workspace. The line beginning with '#' will be ignored. The images
 * will be created according to sec/pixel and node/pixel information in the
 * workspace.
 *
 * \par -o,--offset NUMBER
 * The number of PIXEL to be offset when comparing the causes to the effect.
 *
 * \par -m,--mine-target TARGET_LIST
 * Mine the association rules that have target in the TARGET_LIST. TARGET_LIST
 * is a comma-separated list of the names of the target images.
 *
 * \par -M,--mine-target-file TARGET_FILE
 * Mine the association rules that have target in the TARGET_FILE. The
 * TARGET_FILE is a text file each line of which contains target image name. The
 * line begins with '#' will be ignored.
 *
 * \section example EXAMPLES
 *
 * To initialize workspace that works with 1-hour-1-node pixel images:
 * \par
 * \code{.sh}
 *     bassoc -w workspace -c -S 3600 -N 1
 * \endcode
 *
 * To extract images from balerd's store with the data from '2015-01-01
 * 00:00:00' to current:
 * \par
 * \code{.sh}
 *     bassoc -w workspace -x -s balerd_store -B '2015-01-01 00:00:00' -R recipe
 * \endcode
 *
 * Example content in the recipe file
 * \par
 * \code{.sh}
 *     ev1: 128,129
 *     ev2: 150
 *     ev3: 151
 * \endcode
 *
 * In the above recipe example, the image 'ev1' is created from patterns 128 and
 * 129, while the rest are straighforwardly defined by a single pattern.
 *
 * To extract more images from balerd's store that are not specified in the
 * recipe file:
 * \par
 * \code{.sh}
 *     bassoc -w workspace -x -s balerd_store -B '2015-01-01 00:00:00' \\
 *            -r 'ev4:200' -r 'ev5:600'
 * \endcode
 *
 * The above example will create ev4 and ev5 images from pattern IDs 200 and 600
 * respectively (under the same time constrain as recipe file example).
 *
 * To mine rules with ev3 and ev5 being targets, with time-axis shifting to the
 * left by 1 pixel:
 * \par
 * \code{.sh}
 *     bassoc -w workspace -m ev3,ev5 -o -1
 * \endcode
 *
 * or:
 * \par
 * \code{.sh}
 *     bassoc -w workspace -M target_file
 * \endcode
 *
 * where \e target_file contains the following content:
 * \par
 * \code{.sh}
 *     ev3
 *     # Comment is OK
 *     ev5
 * \endcode
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <wordexp.h>

#include "bassoc.h"
#include "../baler/bhash.h"
#include "../baler/butils.h"
#include "../baler/barray.h"
#include "../query/bquery.h"

/***** OPTIONS *****/
const char *short_opt = "hicw:t:n:xs:B:E:H:r:R:o:m:M:z:K:S:D:v:?";
struct option long_opt[] = {
	{"help",                   0,  0,  'h'},
	{"info",                   0,  0,  'i'},
	{"create",                 0,  0,  'c'},
	{"workspace",              1,  0,  'w'},
	{"sec-per-pixel",          1,  0,  't'},
	{"node-per-pixel",         1,  0,  'n'},
	{"extract",                0,  0,  'x'},
	{"store",                  1,  0,  's'},
	{"ts-begin",               1,  0,  'B'},
	{"ts-end",                 1,  0,  'E'},
	{"host-ids",               1,  0,  'H'},
	{"recipe",                 1,  0,  'r'},
	{"recipe-file",            1,  0,  'R'},
	{"offset",                 1,  0,  'o'},
	{"mine-target",            1,  0,  'm'},
	{"mine-target-file",       1,  0,  'M'},
	{"threads",                1,  0,  'z'},
	{"confident-threshold",    1,  0,  'K'},
	{"significant-threshold",  1,  0,  'S'},
	{"different-threshold",    1,  0,  'D'},
	{"verbose",                1,  0,  'v'},
	{0,                        0,  0,  0},
};

/***** GLOBAL VARIABLES *****/
enum {
	RUN_MODE_CREATE   =  0x1,
	RUN_MODE_EXTRACT  =  0x2,
	RUN_MODE_MINE     =  0x4,
	RUN_MODE_INFO     =  0x8,
} run_mode_flag = 0;

struct ptrlistentry {
	void *ptr;
	LIST_ENTRY(ptrlistentry) entry;
};

LIST_HEAD(ptrlist, ptrlistentry);

const char *workspace_path = NULL;
uint32_t spp = 3600;
uint32_t npp = 1;
const char *store_path = NULL;
const char *ts_begin = NULL;
const char *ts_end = NULL;
const char *host_ids = NULL;
const char *recipe_file_path = NULL;
double confidence = 0.75;
double significance = 0.10;
double difference = 0.15;
int offset = 0;
const char *mine_target_file_path = NULL;

struct bassoc_conf_handle *conf_handle;

struct bhash *ptn2imglist;

struct {
	struct bdstr *conf;
	struct bdstr *img_dir;
} paths;

struct barray *cli_recipe = NULL;

struct barray *cli_targets = NULL;

struct barray *images = NULL;
struct bhash *images_hash = NULL;

struct barray *target_images = NULL;
struct bhash *target_images_hash = NULL;

struct bassoc_rule_q {
	TAILQ_HEAD(, bassoc_rule) head;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	uint64_t refcount;
	enum {
		BASSOC_RULE_Q_STATE_ACTIVE = 0,
		BASSOC_RULE_Q_STATE_DONE,
	} state;
};

struct bassoc_rule_q rule_q;
pthread_t *miner = NULL;
int miner_threads = 1;
struct bassoc_rule_index *rule_index = NULL;

/***** FUNCTIONS *****/

void usage()
{
	printf(
"SYNOPSIS: \n"
"	creating a workspace: \n"
"		bassoc -w WORKSPACE -c [-t NUMBER] [-s NUMBER]\n"
"\n"
"	extracting images: \n"
"		bassoc -w WORKSPACE -x -s BALERD_STORE [-B TS] [-E TS]\n"
"					[-H IDS] -R RECIPE_FILE\n"
"\n"
"		bassoc -w WORKSPACE -x -s BALERD_STORE [-B TS] [-E TS] \n"
"					[-H IDS] -r RECIPE1 -r RECIPE2 ...\n"
"\n"
"	mine for associations: \n"
"		bassoc -w WORKSPACE [-o NUMBER] -m TARGET_LIST\n"
"\n"
"		bassoc -w WORKSPACE [-o NUMBER] -M TARGET_FILE\n"
"\n"
"See bassoc(3) for more information."
	);
}

static
int img_addptn(struct bassocimg *img, int ptn_id)
{
	int rc = 0;
	struct ptrlist *list;
	struct ptrlistentry *lent;
	struct bhash_entry *ent = bhash_entry_get(ptn2imglist,
					(void*)&ptn_id, sizeof(ptn_id));
	if (!ent) {
		list = malloc(sizeof(*list));
		if (!list) {
			rc = ENOMEM;
			goto out;
		}
		LIST_INIT(list);
		ent = bhash_entry_set(ptn2imglist, (void*)&ptn_id,
					sizeof(ptn_id), (uint64_t)(void*)list);
	}
	list = (void*)ent->value;
	lent = malloc(sizeof(*lent));
	if (!lent){
		rc = ENOMEM;
		goto out;
	}
	lent->ptr = img;
	LIST_INSERT_HEAD(list, lent, entry);
out:
	return rc;
}

static
int handle_recipe(const char *recipe)
{
	int rc = 0;
	int len;
	char *path = malloc(4096);
	char *str;
	int a, b, n, i;
	struct bassocimg *img;
	if (!path) {
		rc = ENOMEM;
		goto out;
	}
	str = strchr(recipe, ':');
	if (!str) {
		rc = EINVAL;
		berr("bad recipe: %s", recipe);
		goto out;
	}

	len = snprintf(path, 4096, "%s/%.*s", paths.img_dir->str,
						(int)(str-recipe), recipe);
	if (len >= 4096) {
		/* path too long */
		rc = ENAMETOOLONG;
		goto out;
	}

	if (bfile_exists(path)) {
		/* Image existed */
		rc = EEXIST;
		goto out;
	}

	img = bassocimg_open(path, 1);
	if (!img) {
		berr("Cannot create image: %s", path);
		rc = errno;
		goto out;
	}

	/* str points at ':', move it */
	str++;
	while (*str) {
		n = sscanf(str, "%d%n - %d%n", &a, &len, &b, &len);
		switch (n) {
		case 1:
			b = a;
			break;
		case 2:
			/* do nothing */
			break;
		default:
			berr("Bad recipe: %s", recipe);
			rc = EINVAL;
			goto out;
		}
		str += len;
		for (i = a; i <= b; i++) {
			/* add img ref for ptn i */
			rc = img_addptn(img, i);
			if (rc)
				goto out;
		}
		while (*str && *str == ',')
			str++;
	}

out:
	if (path)
		free(path);
	return 0;
}

void handle_args(int argc, char **argv)
{
	char c;
	int rc;
loop:
	c = getopt_long(argc, argv, short_opt, long_opt, NULL);
	switch (c) {
	case -1:
		goto end;
		break;
	case 'i':
		run_mode_flag |= RUN_MODE_INFO;
		break;
	case 'c':
		run_mode_flag |= RUN_MODE_CREATE;
		break;
	case 'w':
		workspace_path = optarg;
		break;
	case 't':
		spp = atoi(optarg);
		break;
	case 'n':
		npp = atoi(optarg);
		break;
	case 'x':
		run_mode_flag |= RUN_MODE_EXTRACT;
		break;
	case 's':
		store_path = optarg;
		break;
	case 'B':
		ts_begin = optarg;
		break;
	case 'E':
		ts_end = optarg;
		break;
	case 'H':
		host_ids = optarg;
		break;
	case 'r':
		rc = barray_append(cli_recipe, &optarg);
		if (rc) {
			berr("barray_append() error, rc: %d", rc);
			exit(-1);
		}
		break;
	case 'R':
		recipe_file_path = optarg;
		break;
	case 'o':
		offset = atoi(optarg);
		break;
	case 'm':
		run_mode_flag |= RUN_MODE_MINE;
		rc = barray_append(cli_targets, &optarg);
		if (rc) {
			berr("barray_append() error, rc: %d", rc);
			exit(-1);
		}
		break;
	case 'M':
		run_mode_flag |= RUN_MODE_MINE;
		mine_target_file_path = optarg;
		break;
	case 'z':
		miner_threads = atoi(optarg);
		break;
	case 'K':
		confidence = atof(optarg);
		break;
	case 'S':
		significance = atof(optarg);
		break;
	case 'D':
		difference = atof(optarg);
		break;
	case 'v':
		rc = blog_set_level_str(optarg);
		if (rc) {
			berr("Invalid verbosity level: %s", optarg);
			exit(-1);
		}
		break;
	case 'h':
	default:
		usage();
		exit(-1);
	}
	goto loop;

end:
	if (workspace_path == NULL) {
		berr("workspace path (-w) is needed");
		exit(-1);
	}

	size_t len = strlen(workspace_path);
	paths.conf = bdstr_new(512);
	paths.img_dir = bdstr_new(512);

	if (!paths.conf || !paths.img_dir) {
		berr("Out of memory");
		exit(-1);
	}

	bdstr_append_printf(paths.conf, "%s/conf", workspace_path);
	bdstr_append_printf(paths.img_dir, "%s/img", workspace_path);

	return;
}

static
void __create_dir(const char *dir, mode_t mode)
{
	int rc = bmkdir_p(dir, mode);
	if (rc) {
		berr("Cannot create dir '%s', rc: %d", dir, rc);
		exit(-1);
	}
}

void bassoc_conf_close(struct bassoc_conf_handle *handle)
{
	int rc;
	if (handle->conf) {
		rc = munmap(handle->conf, sizeof(*handle->conf));
		if (rc)
			bwarn("munmap() rc: %d, errno(%d): %m", rc, errno);
	}
	if (handle->fd) {
		rc = close(handle->fd);
		if (rc)
			bwarn("close() rc: %d, errno(%d): %m", rc, errno);
	}
	free(handle);
}

struct bassoc_conf_handle *bassoc_conf_open(const char *path, int flags, ...)
{
	off_t off;
	ssize_t sz;
	struct bassoc_conf_handle *handle = malloc(sizeof(*handle));
	if (!handle)
		return NULL;
	handle->fd = -1;
	handle->conf = 0;
	va_list ap;
	mode_t mode;

	if (flags & O_CREAT) {
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
		handle->fd = open(path, flags, mode);
		if (handle->fd < 0) {
			berr("Cannot open file %s, err(%d): %m", path, errno);
			goto err;
		}
		off = lseek(handle->fd, sizeof(*handle->conf) - 1, SEEK_SET);
		if (off == -1) {
			berr("seek(%s) error(%d): %m", path, errno);
			goto err;
		}
		sz = write(handle->fd, "", 1);
		if (sz == -1) {
			berr("write(%s) error(%d): %m", path, errno);
			goto err;
		}
	} else {
		handle->fd = open(path, flags);
	}
	handle->conf = mmap(NULL, sizeof(*handle->conf), PROT_READ|PROT_WRITE,
			MAP_SHARED, handle->fd, 0);
	if (handle->conf == MAP_FAILED) {
		berr("mmap(%s) error(%d): %m", path, errno);
		goto err;
	}
	return handle;
err:
	bassoc_conf_close(handle);
	return NULL;
}

static
void open_bassoc_conf_routine()
{
	conf_handle = bassoc_conf_open(paths.conf->str, O_RDWR);
	if (!conf_handle) {
		berr("Cannot open bassoc configuration");
		exit(-1);
	}
}

void create_routine()
{
	int rc;
	static char buff[4096];
	if (bfile_exists(workspace_path)) {
		berr("workspace '%s' existed", workspace_path);
		exit(-1);
	}

	__create_dir(workspace_path, 0755);
	__create_dir(paths.img_dir->str, 0755);

	conf_handle = bassoc_conf_open(paths.conf->str, O_RDWR|O_CREAT, 0666);
	if (!conf_handle) {
		berr("Cannot open-create bassoc configuration");
		exit(-1);
	}
	conf_handle->conf->npp = npp;
	conf_handle->conf->spp = spp;
	bassoc_conf_close(conf_handle);
}

int process_recipe_line(char *line, void *ctxt)
{
	int rc;
	rc = handle_recipe(line);
	if (rc) {
		/* Detail of the error should be printed in
		 * handle_recipe() function */
		berr("Recipe error ... exiting");
		exit(-1);
	}
	return 0;
}

static
void process_recpies_routine()
{
	int i, n, rc;
	const char *recipe;
	FILE *fin;
	char *buff = malloc(4096);

	if (!buff) {
		berr("Out of memory (%s:%s())", __FILE__, __func__);
		exit(-1);
	}

	/* Iterate through recipes from CLI */
	n = (cli_recipe)?(barray_get_len(cli_recipe)):(0);
	for (i = 0; i < n; i++) {
		barray_get(cli_recipe, i, &recipe);
		rc = handle_recipe(recipe);
		if (rc) {
			/* Detail of the error should be printed in
			 * handle_recipe() function */
			berr("Recipe error ... exiting");
			exit(-1);
		}
	}

	if (!recipe_file_path) {
		return;
		/* no need to continue */
	}

	/* Iterate through recipes from RECIPE_FILE */
	rc = bprocess_file_by_line_w_comment(recipe_file_path,
						process_recipe_line, NULL);
	if (rc) {
		berr("Process recipe file '%s' error, rc: %d",
							recipe_file_path, rc);
		exit(-1);
	}
}

struct __best_img {
	int spp;
	int npp;
};

static
void __bq_imgiter_cb(const char *name, void *ctxt)
{
	struct __best_img *best = ctxt;
	int _npp, _spp, n;
	n = sscanf(name, "%d-%d", &_spp, &_npp);
	if (n != 2) {
		bwarn("Unexpected image name: %s", name);
		return;
	}
	if (_npp > conf_handle->conf->npp)
		/* not fine enough */
		return;
	if (_spp > conf_handle->conf->spp)
		/* not fine enough */
		return;
	if (best->spp && (best->npp*best->spp >= _npp*spp))
		/* finer than needed */
		return;
	/* better image, update the 'best' */
	best->npp = _npp;
	best->spp = _spp;
}

int get_closest_img_store(struct bq_store *bq_store, struct bdstr *bdstr)
{
	struct __best_img best = {0, 0};
	int rc = bq_imgstore_iterate(bq_store, __bq_imgiter_cb, &best);
	if (rc)
		return rc;
	if (!best.spp)
		return ENOENT;
	bdstr_reset(bdstr);
	bdstr_append_printf(bdstr, "%d-%d", best.spp, best.npp);
	return 0;
}

static inline
int __pxl_cmp(uint32_t ts0, uint32_t c0, uint32_t ts1, uint32_t c1)
{
	if (ts0 < ts1)
		return -1;
	if (ts0 > ts1)
		return 1;
	if (c0 < c1)
		return -1;
	if (c0 > c1)
		return 1;
	return 0;
}

static
void extract_routine_by_msg(struct bq_store *bq_store)
{
	berr("Extracting images by messages is not yet implemented.");
	exit(-1);
}

static
void extract_routine_by_img(struct bq_store *bq_store, const char *img_store_name)
{
	int i, n, rc;
	struct bimgquery *bq;
	struct bpixel pixel;
	struct bassocimg_pixel bassoc_pixel;
	struct bhash_entry *hent;
	struct ptrlist *list;
	struct ptrlistentry *lent;
	bq = bimgquery_create(bq_store, host_ids, NULL, ts_begin, ts_end,
							img_store_name, &rc);
	if (!bq) {
		berr("cannot create bquery, rc: %d", rc);
		exit(-1);
	}
	rc = bq_first_entry((void*)bq);
	if (rc) {
		berr("bq_first_entry() error, rc: %d", rc);
		exit(-1);
	}

loop:
	bq_img_entry_get_pixel(bq, &pixel);
	hent = bhash_entry_get(ptn2imglist, (void*)&pixel.ptn_id, sizeof(pixel.ptn_id));
	if (!hent)
		goto next;
	list = (void*)hent->value;
	LIST_FOREACH(lent, list, entry) {
		bassoc_pixel.sec = pixel.sec;
		bassoc_pixel.comp_id = pixel.comp_id;
		bassoc_pixel.count = pixel.count;
		rc = bassocimg_add_count(lent->ptr, &bassoc_pixel);
		if (rc) {
			berr("bassocimg_add_count() error, rc: %d", rc);
			exit(-1);
		}
	}

next:
	/* next entry */
	rc = bq_next_entry((void*)bq);
	switch (rc) {
	case 0:
		goto loop;
	case ENOENT:
		break;
	default:
		berr("bq_next_entry() error, rc: %d", rc);
	}
}

static
void extract_routine()
{
	int i, n, rc;
	struct bq_store *bq_store;
	struct bimgquery *bq;
	struct bdstr *bdstr;
	struct bpixel pixel;

	if (!store_path) {
		berr("store path (-s) is needed for image extraction");
		exit(-1);
	}
	ptn2imglist = bhash_new(65521, 11, NULL);
	if (!ptn2imglist) {
		berr("bhash_new() error(%d): %m", errno);
		exit(-1);
	}
	bq_store = bq_open_store(store_path);
	if (!bq_store) {
		berr("Cannot open baler store (%s), err(%d): %m", store_path, errno);
		exit(-1);
	}
	bdstr = bdstr_new(128);
	if (!bdstr) {
		berr("Out of memory (in %s() %s:%d)", __func__, __FILE__, __LINE__);
		exit(-1);
	}
	process_recpies_routine();
	rc = get_closest_img_store(bq_store, bdstr);
	switch (rc) {
	case 0:
		extract_routine_by_img(bq_store, bdstr->str);
		break;
	case ENOENT:
		extract_routine_by_msg(bq_store);
		break;
	default:
		berr("get_closest_img_store() error, rc: %d", rc);
		exit(-1);
	}
}

void info_routine()
{
	struct bdstr *bdstr;
	wordexp_t wexp;
	int rc, i;
	struct bassoc_conf *conf = conf_handle->conf;
	printf("Configuration:\n");
	printf("\tseconds per pixel: %d\n", spp);
	printf("\tnodes per pixel: %d\n", npp);
	bdstr = bdstr_new(PATH_MAX);
	if (!bdstr) {
		berr("Out of memory");
		exit(-1);
	}
	rc = bdstr_append_printf(bdstr, "%s/img/*", workspace_path);
	if (rc) {
		berr("Out of memory");
		exit(-1);
	}
	rc = wordexp(bdstr->str, &wexp, 0);
	if (rc) {
		berr("wordexp() error, rc: %d", rc);
		exit(-1);
	}
	printf("Images:\n");
	for (i = 0; i < wexp.we_wordc; i++) {
		const char *name = strrchr(wexp.we_wordv[i], '/') + 1;
		printf("\t%s\n", name);
	}
	bdstr_free(bdstr);
}

void bassoc_rule_free(struct bassoc_rule *rule)
{
	if (rule->formula) {
		bdstr_free(rule->formula);
	}
	free(rule);
}

struct bassoc_rule *bassoc_rule_new()
{
	struct bassoc_rule *rule = calloc(1, sizeof(*rule));
	if (!rule)
		return NULL;
	rule->formula = bdstr_new(128);
	if (!rule->formula) {
		bassoc_rule_free(rule);
		return NULL;
	}
	return rule;
}

void bassoc_rule_add(struct bassoc_rule_q *q, struct bassoc_rule *r)
{
	pthread_mutex_lock(&q->mutex);
	TAILQ_INSERT_TAIL(&q->head, r, entry);
	q->refcount++;
	pthread_cond_signal(&q->cond);
	pthread_mutex_unlock(&q->mutex);
}

void bassoc_rule_put(struct bassoc_rule_q *q, struct bassoc_rule *rule)
{
	pthread_mutex_lock(&q->mutex);
	bassoc_rule_free(rule);
	q->refcount--;
	if (!q->refcount) {
		/* DONE */
		q->state = BASSOC_RULE_Q_STATE_DONE;
		pthread_cond_broadcast(&q->cond);
	}
	pthread_mutex_unlock(&q->mutex);
}

struct bassoc_rule *bassoc_rule_get(struct bassoc_rule_q *q)
{
	struct bassoc_rule *r;
	pthread_mutex_lock(&q->mutex);
	if (q->state == BASSOC_RULE_Q_STATE_DONE) {
		r = NULL;
		goto out;
	}
	while (TAILQ_EMPTY(&q->head)) {
		pthread_cond_wait(&q->cond, &q->mutex);
		if (q->state == BASSOC_RULE_Q_STATE_DONE) {
			r = NULL;
			goto out;
		}
	}
	r = TAILQ_FIRST(&q->head);
	TAILQ_REMOVE(&q->head, r, entry);
out:
	pthread_mutex_unlock(&q->mutex);
	return r;
}

void bassoc_rule_print(struct bassoc_rule *rule, const char *prefix)
{
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	int *formula = (int*)rule->formula->str;
	size_t formula_len = rule->formula->str_len / sizeof(int);
	int i;
	struct bassocimg *img;
	const char *name;
	pthread_mutex_lock(&mutex);
	printf("%s: (%lf, %lf) {", prefix, rule->conf, rule->sig);
	for (i = 1; i < formula_len; i++) {
		img = NULL;
		barray_get(images, formula[i], &img);
		assert(img);
		name = strrchr(bassocimg_get_path(img), '/') + 1;
		if (i == 1)
			printf("%s", name);
		else
			printf(",%s", name);
	}
	img = NULL;
	barray_get(target_images, formula[0], &img);
	assert(img);
	name = strrchr(bassocimg_get_path(img), '/') + 1;
	printf("}->{%s}\n", name);
	pthread_mutex_unlock(&mutex);
}

void bassoc_rule_debug(struct bassoc_rule *rule, const char *prefix)
{
	if (blog_get_level() > BLOG_LV_DEBUG)
		return;
	bassoc_rule_print(rule, prefix);
}

struct bassoc_rule_index *bassoc_rule_index_new(size_t hash_size)
{
	struct bassoc_rule_index *index = calloc(1, sizeof(*index));
	if (!index)
		goto out;
	index->hash = bhash_new(hash_size, 11, NULL);
	if (!index->hash)
		goto err0;
	pthread_mutex_init(&index->mutex, NULL);

	goto out;
err0:
	free(index);
	index = NULL;
out:
	return index;
}

static
void __formula_begin(const char *formula, const char **a, const char **b)
{
	*a = formula;
	*b = strchr(formula, ',');
	if (!*b)
		*b = (*a) + strlen(*a);
}

static
int __next_in_formula(const char **a, const char **b)
{
	if (!**b)
		return ENOENT;
	*a = *b+1;
	*b = strchr(*a, ',');
	if (!*b)
		*b = *a + (strlen(*a));
	return 0;
}

int bassoc_rule_index_add(struct bassoc_rule_index *index, struct bassoc_rule *rule)
{
	int rc = 0;
	size_t tgt_len;
	struct bdstr *bdstr = bdstr_new(256);
	int *formula;
	int formula_len;
	int i;

	if (!bdstr)
		return ENOMEM;

	formula = (int*)rule->formula->str;
	formula_len = rule->formula->str_len / sizeof(int);
	rc = bdstr_append_mem(bdstr, formula, sizeof(int));
	if (rc)
		goto out;
	tgt_len = bdstr->str_len;

	for (i = 1; i < formula_len; i++) {
		bdstr->str_len = tgt_len;
		rc = bdstr_append_mem(bdstr, &formula[i], sizeof(int));
		if (rc)
			goto out;
		struct bassoc_rule_index_entry *ient = malloc(sizeof(*ient));
		if (!ient) {
			rc = ENOMEM;
			goto out;
		}
		ient->rule = rule;
		pthread_mutex_lock(&index->mutex);
		struct bhash_entry *hent = bhash_entry_get(index->hash,
						bdstr->str, bdstr->str_len);
		if (!hent) {
			hent = bhash_entry_set(index->hash, bdstr->str,
							bdstr->str_len, 0);
			if (!hent) {
				rc = ENOMEM;
				pthread_mutex_unlock(&index->mutex);
				goto out;
			}
		}

		struct bassoc_rule_index_list *l = (void*)&hent->value;
		LIST_INSERT_HEAD(l, ient, entry);
		pthread_mutex_unlock(&index->mutex);
	}

out:
	bdstr_free(bdstr);
	return rc;
}

struct bassoc_rule_index_entry *
bassoc_rule_index_get(struct bassoc_rule_index *index, const char *key,
								size_t keylen)
{
	struct bhash_entry *hent;
	struct bassoc_rule_index_list *list;
	struct bassoc_rule_index_entry *ient = NULL;

	pthread_mutex_lock(&index->mutex);
	hent = bhash_entry_get(index->hash, key, keylen);
	if (!hent)
		goto out;
	list = (void*)&hent->value;
	ient = LIST_FIRST(list);
out:
	pthread_mutex_unlock(&index->mutex);
	return ient;
}

int handle_target(const char *tname)
{
	int rc = 0;
	int idx;
	struct bassocimg *img = NULL;
	struct bassocimg *timg = NULL;
	struct bhash_entry *hent;
	struct bassoc_rule *rule;
	struct bdstr *bdstr = NULL;

	bdstr = bdstr_new(PATH_MAX);
	if (!bdstr) {
		rc = ENOMEM;
		goto out;
	}

	rule = bassoc_rule_new();
	if (!rule) {
		rc = ENOMEM;
		goto out;
	}

	hent = bhash_entry_get(images_hash, tname, strlen(tname));
	if (!hent) {
		rc = ENOENT;
		goto err;
	}
	img = (void*)hent->value;

	bdstr_reset(bdstr);
	bdstr_append_printf(bdstr, "%s/comp_img/%s%+d", workspace_path,
							tname, offset);
	timg = bassocimg_open(bdstr->str, 1);
	if (!timg) {
		berr("Cannot open/create composite image: %s, err(%d): %m",
						bdstr->str, errno);
		rc = errno;
		goto err;
	}

	if (bassocimg_get_pixel_len(timg) > 0) {
		/* image has already  been populated, just skip it */
		goto enqueue;
	}

	rc = bassocimg_shift_ts(img, offset * conf_handle->conf->spp, timg);
	if (rc) {
		berr("bassocimg_shift_ts('%s', %d, '%s')"
				" failed, rc: %d",
				bassocimg_get_path(img),
				offset * conf_handle->conf->spp,
				bassocimg_get_path(timg),
				rc);
		goto err;
	}

enqueue:
	rule->last_idx = -1;

	barray_append(target_images, &timg);
	idx = barray_get_len(target_images) - 1;

	bdstr_reset(rule->formula);
	rc = bdstr_append_mem(rule->formula, &idx, sizeof(idx));
	if (rc) {
		goto err;
	}
	hent = bhash_entry_set(target_images_hash, rule->formula->str,
						rule->formula->str_len,
							(uint64_t)timg);
	if (!hent) {
		berr("target image load error(%d): %m", errno);
		goto err;
	}

	bassoc_rule_add(&rule_q, rule);

	goto out;

err:
	bassoc_rule_free(rule);
out:
	if (bdstr)
		bdstr_free(bdstr);
	return rc;
}

int handle_mine_target_line(char *line, void *ctxt)
{
	return handle_target(line);
}

static
void handle_mine_target_file()
{
	int rc;
	if (!mine_target_file_path)
		return;

	rc = bprocess_file_by_line_w_comment(mine_target_file_path,
					handle_mine_target_line, NULL);
	if (rc) {
		berr("Error processing target file: %s, rc: %d",
					mine_target_file_path, rc);
		exit(-1);
	}
}

static
void init_images_routine()
{
	int rc, i;
	int idx;
	struct bassocimg *img;
	struct bdstr *path = bdstr_new(PATH_MAX);
	struct bhash_entry *hent;
	const char *name;
	wordexp_t wexp;

	if (!path) {
		berr("Not enough memory");
		exit(-1);
	}
	bdstr_append_printf(path, "%s/img", workspace_path);
	images = barray_alloc(sizeof(void*), 1024);
	if (!images) {
		berr("Not enough memory");
		exit(-1);
	}

	images_hash = bhash_new(65539, 11, NULL);
	if (!images_hash) {
		berr("Not enough memory");
		exit(-1);
	}

	target_images = barray_alloc(sizeof(void*), 1024);
	if (!target_images) {
		berr("Not enough memory");
		exit(-1);
	}

	target_images_hash = bhash_new(65539, 11, NULL);
	if (!target_images_hash) {
		berr("Not enough memory");
		exit(-1);
	}

	bdstr_reset(path);
	bdstr_append_printf(path, "%s/img/*", workspace_path);
	rc = wordexp(path->str, &wexp, 0);
	if (rc) {
		berr("wordexp() error, rc: %d", rc);
		exit(-1);
	}
	for (i = 0; i < wexp.we_wordc; i++) {
		img = bassocimg_open(wexp.we_wordv[i], 0);
		if (!img) {
			berr("Cannot open image: %s, err(%d): %m", path->str, errno);
			exit(-1);
		}
		rc = barray_set(images, i, &img);
		if (rc) {
			berr("barray_set() error, rc: %d", rc);
			exit(-1);
		}
		name = strrchr(wexp.we_wordv[i], '/') + 1;
		hent = bhash_entry_set(images_hash, name,
				strlen(name), (uint64_t)(void*)img);
		if (!hent) {
			berr("bhash_entry_set() error(%d): %m", errno);
			exit(-1);
		}
	}
	bdstr_free(path);
}

void init_target_images_routine()
{
	int i, n, rc;
	struct bassocimg *img;
	struct bassocimg *timg;
	const char *tname;
	struct bdstr *bdstr = bdstr_new(1024);

	if (!bdstr) {
		berr("Out of memory");
		exit(-1);
	}

	bdstr_reset(bdstr);
	bdstr_append_printf(bdstr, "%s/comp_img", workspace_path);

	rc = bmkdir_p(bdstr->str, 0755);

	if (rc && rc != EEXIST) {
		berr("Cannot create directory %s, rc: %d", bdstr->str, rc);
		exit(-1);
	}

	handle_mine_target_file();

	n = barray_get_len(cli_targets);
	for (i = 0; i < n; i++) {
		barray_get(cli_targets, i, &tname);
		rc = handle_target(tname);
		if (rc) {
			berr("Error processing target: %s, rc: %d", tname, rc);
			exit(-1);
		}
	}

	bdstr_free(bdstr);
}

#define MINER_CTXT_STACK_SZ 11

struct miner_ctxt {
	struct bdstr *bdstr;

	/* Image-Recipe stack */
	/* The additional +2 are the space for miner operation */
	struct bassocimg *img[MINER_CTXT_STACK_SZ + 2];
	int recipe[MINER_CTXT_STACK_SZ];
	int stack_sz;
};

struct miner_ctxt *miner_init(int thread_number)
{
	int i, rc;
	struct miner_ctxt *ctxt = calloc(1, sizeof(*ctxt));
	if (!ctxt)
		goto out;

	ctxt->bdstr = bdstr_new(PATH_MAX);
	if (!ctxt->bdstr)
		goto err0;

	bdstr_reset(ctxt->bdstr);
	bdstr_append_printf(ctxt->bdstr, "%s/miner-%d", workspace_path,
								thread_number);
	rc = bmkdir_p(ctxt->bdstr->str, 755);
	if (rc && rc != EEXIST) {
		errno = rc;
		berr("Cannot create directory: %s", ctxt->bdstr->str);
		goto err1;
	}

	for (i = 1; i < MINER_CTXT_STACK_SZ + 2; i++) {
		/* img[0] need no tmp file */
		bdstr_reset(ctxt->bdstr);
		bdstr_append_printf(ctxt->bdstr, "%s/miner-%d/img%d",
					workspace_path, thread_number, i);
		ctxt->img[i] = bassocimg_open(ctxt->bdstr->str, 1);
		if (!ctxt->img[i]) {
			berr("cannot open img: %s", ctxt->bdstr->str);
			goto err1;
		}
	}

	goto out;

err1:
	for (i = 0; i < MINER_CTXT_STACK_SZ; i++) {
		if (ctxt->img[i])
			bassocimg_close_free(ctxt->img[i]);
	}

	bdstr_free(ctxt->bdstr);
err0:
	free(ctxt);
out:
	return ctxt;
}

static inline
struct bassocimg *get_image(struct bhash *hash, const char *key, size_t keylen)
{
	struct bhash_entry *ent = bhash_entry_get(hash, key, keylen);
	if (!ent)
		return NULL;
	return (void*)ent->value;
}

int miner_add_stack_img(struct miner_ctxt *ctxt, struct bassocimg *img,
								int img_idx)
{
	int i = ctxt->stack_sz;
	int rc;
	if (i >= MINER_CTXT_STACK_SZ)
		return ENOMEM;
	ctxt->stack_sz++;
	ctxt->recipe[i] = img_idx;
	if (i == 0) {
		ctxt->img[0] = img;
		return 0;
	}
	return bassocimg_intersect(ctxt->img[i-1], img, ctxt->img[i]);
}

/*
 * Test whether \c r0 is more general than \c r1.
 * In other words, test whether \c r0 is a subset of \c r1.
 */
static
int bassoc_rule_general(struct bassoc_rule *r0, struct bassoc_rule *r1)
{
	int *f0 = (int*)r0->formula->str;
	int f0_len = r0->formula->str_len / sizeof(int);
	int *f1 = (int*)r1->formula->str;
	int f1_len = r1->formula->str_len / sizeof(int);
	int i0, i1;
	if (f0[0] != f1[0])
		/* Different target */
		return 0;
	i0 = i1 = 1;
	while (i0 < f0_len && i1 < f1_len) {
		if (f0[i0] < f1[i1])
			return 0;
		if (f0[i0] == f1[i1])
			i0++;
		i1++;
	}
	return i0 == f0_len;
}

void *miner_proc(void *arg)
{
	int thread_number = (uint64_t)arg;
	int i, n, rc;
	int idx;
	int fidx;
	int sidx;
	int tidx;
	int rlen;
	const char *s;
	const char *t;
	const char *target;
	struct bhash_entry *hent;
	/* suppose rule := Ax->z */
	struct bassoc_rule *rule;
	struct bassocimg *timg; /* target image (z) */
	struct bassocimg *aimg; /* antecedent image (Ax) */
	struct bassocimg *bimg; /* base of antecedent image (A) */
	struct bassocimg *cimg; /* consequence+antecedent image (Axz) */
	struct bassocimg *img;
	int *formula; /* formula[0] = tgt, formula[x] = event */
	size_t formula_len;

	struct miner_ctxt *ctxt;

	ctxt = miner_init(thread_number);

	if (!ctxt) {
		/* Error details should be printed within miner_init() */
		return 0;
	}

	n = barray_get_len(images);

	cimg = ctxt->img[MINER_CTXT_STACK_SZ + 1];

loop:
	rule = bassoc_rule_get(&rule_q);
	if (!rule ) {
		/* DONE */
		goto out;
	}

	formula = (int*)rule->formula->str;
	formula_len = rule->formula->str_len / sizeof(formula[0]);
	tidx = formula[0];
	barray_get(target_images, tidx, &timg);

	sidx = 0;
	fidx = 1;
	while (sidx < ctxt->stack_sz && fidx < formula_len) {
		if (ctxt->recipe[sidx] != formula[fidx])
			break;
		sidx++;
		fidx++;
	}

	/* we can reuse what in the stack, up to sidx - 1 */
	ctxt->stack_sz = sidx;

	while (fidx < formula_len) {
		img = NULL;
		barray_get(images, formula[fidx], &img);
		if (!img) {
			berr("Cannot get image: %.*s", t - s, s);
			assert(0);
		}
		rc = miner_add_stack_img(ctxt, img, formula[fidx]);
		fidx++;
	}
	/* Now, the top-of-stack is the antecedent image */

	bimg = (ctxt->stack_sz)?(ctxt->img[ctxt->stack_sz - 1]):(NULL);
	aimg = ctxt->img[MINER_CTXT_STACK_SZ];

	for (i = rule->last_idx + 1; i < n; i++) {
		/* Expanding rule candidates to discover rules */
		struct bassoc_rule *r = bassoc_rule_new();
		if (!r) {
			berr("Cannot allocate memory for a new rule ...");
			goto out;
		}
		barray_get(images, i, &img);

		/* construct rule candidate */
		bdstr_reset(r->formula);
		rc = bdstr_append_mem(r->formula, rule->formula->str,
						rule->formula->str_len);
		assert(rc == 0);
		bdstr_append_mem(r->formula, &i, sizeof(int));
		assert(rc == 0);
		r->last_idx = i;

		bdstr_reset(ctxt->bdstr);
		bdstr_append_mem(ctxt->bdstr, &tidx, sizeof(int));
		bdstr_append_mem(ctxt->bdstr, &i, sizeof(int));

		struct bassoc_rule_index_entry *rent = bassoc_rule_index_get(
						rule_index, ctxt->bdstr->str,
						ctxt->bdstr->str_len);

		/* General rule bound */
		while (rent) {
			if (bassoc_rule_general(rent->rule, r)) {
				bassoc_rule_debug(r, "general bounded");
				goto bound;
			}
			rent = LIST_NEXT(rent, entry);
		}

		/* intersect ... */
		if (bimg) {
			rc = bassocimg_intersect(bimg, img, aimg);
			assert(rc == 0);
		} else {
			aimg = img;
		}
		rc = bassocimg_intersect(aimg, timg, cimg);
		assert(rc == 0);

		struct bassocimg_hdr *ahdr, *bhdr, *chdr, *thdr;
		ahdr = bassocimg_get_hdr(aimg);
		if (bimg)
			bhdr = bassocimg_get_hdr(bimg);
		chdr = bassocimg_get_hdr(cimg);
		thdr = bassocimg_get_hdr(timg);

		/* calculate confidence, significance */
		r->conf = chdr->count / (double)ahdr->count;
		r->sig = chdr->count / (double)thdr->count;

		if (r->sig < significance) {
			/* Significance bound */
			bassoc_rule_debug(r, "significance bounded");
			goto bound;
		}

		if (r->conf > confidence) {
			/* This is a rule, no need to expand more */
			bassoc_rule_print(r, "rule");
			bassoc_rule_index_add(rule_index, r);
			goto term;
		}

		if (bimg && (bhdr->count - ahdr->count) / (double)(bhdr->count) < difference) {
			/* Difference bound */
			bassoc_rule_debug(r, "difference bounded");
			goto bound;
		}

		/* Good candidate, add to the queue */
		bassoc_rule_add(&rule_q, r);
		bassoc_rule_debug(r, "valid candidate");
		continue;

	bound:
		bassoc_rule_free(r);
	term:
		continue;
	}

end:
	/* done with the rule, put it down */
	bassoc_rule_put(&rule_q, rule);
	goto loop;

out:
	return NULL;
}

static
void mine_routine()
{
	int i, rc;
	/* Initialize rule mining queue */
	TAILQ_INIT(&rule_q.head);
	pthread_mutex_init(&rule_q.mutex, NULL);
	pthread_cond_init(&rule_q.cond, NULL);
	rule_q.refcount = 0;
	rule_q.state = BASSOC_RULE_Q_STATE_ACTIVE;

	init_images_routine();
	init_target_images_routine();

	if (miner_threads - 1) {
		miner = calloc(sizeof(pthread_t), miner_threads - 1);
		if (!miner) {
			berr("Out of memory");
			exit(-1);
		}
	}

	binfo("Mining ...");

	for (i = 0; i < miner_threads - 1; i++) {
		rc = pthread_create(&miner[i], NULL, miner_proc, (void*)(uint64_t)i);
		if (rc) {
			berr("pthread_create() error, rc: %d", rc);
			exit(-1);
		}
	}

	miner_proc((void*)(uint64_t)(miner_threads - 1));

	for (i = 0; i < miner_threads - 1; i++) {
		pthread_join(miner[i], NULL);
	}
}

void init() {
	cli_recipe = barray_alloc(sizeof(void*), 1024);
	if (!cli_recipe) {
		berr("Out of memory");
		exit(-1);
	}
	cli_targets = barray_alloc(sizeof(void*), 1024);
	if (!cli_targets) {
		berr("Out of memory");
		exit(-1);
	}
	rule_index = bassoc_rule_index_new(65521);
	if (!rule_index) {
		berr("Out of memory");
		exit(-1);
	}
}

int main(int argc, char **argv)
{
	init();
	handle_args(argc, argv);
	void (*(routine[]))(void) = {
		[RUN_MODE_CREATE]   =  create_routine,
		[RUN_MODE_EXTRACT]  =  extract_routine,
		[RUN_MODE_INFO]     =  info_routine,
		[RUN_MODE_MINE]     =  mine_routine,
	};
	switch (run_mode_flag) {
	case RUN_MODE_EXTRACT:
	case RUN_MODE_INFO:
	case RUN_MODE_MINE:
		open_bassoc_conf_routine();
	case RUN_MODE_CREATE:
		routine[run_mode_flag]();
		break;
	default:
		berr("Cannot determine run mode.");
	}
	return 0;
}