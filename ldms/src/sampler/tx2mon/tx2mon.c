/*
 * tx2mon.c -	LDMS sampler for basic Marvell TX2 chip telemetry.
 *
 * 		Sampler to provide LDMS data available via the tx2mon
 * 		CLI utility (https://github.com/jchandra-cavm/tx2mon).
 * 		This data exists in structure that is mapped all
 * 		the way into sysfs from the memory of the M3 management
 * 		processor present on each TX2 die.
 * 		This sampler requires the tx2mon kernel module to be loaded.
 * 		This module creates sysfs entries that can be opened and
 * 		mmapped, then overlaid with a matching structure definition.
 * 		Management processor updates the underlying structure at >= 10Hz.
 * 		The structure contains a great deal of useful telemetry, including:
 * 		 - clock speeds
 * 		 - per-core temperatures
 * 		 - power data
 * 		 - throttling statistics
 */

/*
 *  Copyright [2020] Hewlett Packard Enterprise Development LP
 * 
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of version 2 of the GNU General Public License as published 
 * by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to:
 * 
 *   Free Software Foundation, Inc.
 *   51 Franklin Street, Fifth Floor
 *   Boston, MA 02110-1301, USA.
 */

#define _GNU_SOURCE
#include <inttypes.h>
#include <unistd.h>
#include <sys/errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include "ldms.h"
#include "ldmsd.h"
#include "sampler_base.h"
#include "tx2mon.h"

static ldmsd_msg_log_f msglog;
static ldms_set_t set = NULL;
#define SAMP "tx2mon"
static base_data_t base;

static struct tx2mon_sampler tx2mon;

/*
 * Plug-in data structure and access method.
 */
static struct ldmsd_sampler tx2mon_plugin = {
	.base = {
		.name = SAMP,
		.type = LDMSD_PLUGIN_SAMPLER,
		.term = term,
		.config = config,
		.usage = usage,
	},
	.get_set = get_set,
	.sample = sample,
};

struct ldmsd_plugin *get_plugin(ldmsd_msg_log_f pf)
{
	msglog = pf;
	set = NULL;
	return &tx2mon_plugin.base;
}

static int config(struct ldmsd_plugin *self, struct attr_value_list *kwl, struct attr_value_list *avl)
{
	int rc = -1;
	ldms_schema_t schema;

	if (set) {
		msglog(LDMSD_LERROR, SAMP ": Set already created.\n");
		return EINVAL;
	}

	/*
	 * TODO: parse kwl/avl here
	 */

	base = base_config(avl, SAMP, SAMP, msglog);
	if (!base) {
		/*
		 * base_del() ?
		 */
		return errno;
	}

	rc = create_metric_set(base);
	if (rc) {
		msglog(LDMSD_LERROR, SAMP ": failed to create metric set.\n");
	}

	return rc;
}

static const char *usage(struct ldmsd_plugin *self)
{
	return SAMP ": Lorem Ipsum";
}

static void term(struct ldmsd_plugin *self)
{
	int i;
	struct cpu_info *cp;

	for (i = 0; i < tx2mon.n_cpu; i++) {
		cp = &(tx2mon.cpu[i]);

		/*
		 * None of these test should be required, but ...
		 */
		if ((cp->mcp != NULL) && (cp->mcp != MAP_FAILED)) {
			munmap(cp->mcp, sizeof(struct mc_oper_region));
			cp->mcp = NULL;
		}

		if (cp->fd >= 0) {
			close(cp->fd);
			cp->fd = -1;
		}
	}

	if (base)
		base_del(base);
	if (set)
		ldms_set_delete(set);
	set = NULL;
}

static int sample(struct ldmsd_sampler *self)
{
	return EINVAL;
}

/*
 * get_set() -	Obsolete call, no longer used.
 * 		Return safe value, just in case.
 */
static ldms_set_t get_set(struct ldmsd_sampler *self)
{
	return set;
}

/*
 * Create the schema and metric set.
 * 
 *  - Read & parse TX2MON_SOCINFO_PATH to learn about the system config, and
 *    test that the kernel module is loaded etc. This file is plain ascii.
 *  - Open & mmap TX2MON_NODE?_PATH files. These are binary data structures
 *    defined in mc_oper_region.h (that we were magically pointed to during
 *    configure.)
 *  - Establish the capabilities reported, based on the version number of
 *    the data structure.
 *  - Set up schema, and update housekeeping data.
 *
 *    Return 0 iff all good, else report useful error message, clean up,
 *    and return appropriate errno.
 */
static int create_metric_set(base_data_t base)
{
	ldms_schema_t schema;
	int rc = -1;
	int offset;
	int cpu;
	FILE *socinfo;
	char *path;
	int i;
	char buf[TMP_BUF_SIZE];
	struct cpu_info *cp;

	path = realpath(TX2MON_SOCINFO_PATH, NULL);
	if (path == NULL) {
		msglog(LDMSD_LERROR, SAMP ": cannot resolve path for '%s'.\n",
				TX2MON_SOCINFO_PATH);
		return EBADF;
	}

	socinfo = fopen(path, "r");
	if (!socinfo) {
		msglog(LDMSD_LERROR, SAMP ": cannot open '%s', %s.\n",
				path, strerror(errno));
		free(path);
		return errno;
	}

	/*
	 * Parse socinfo file, it contains three integers with a single
	 * space between, any problem => fail.
	 */
	if (fscanf(socinfo, "%d %d %d", &(tx2mon.n_cpu),
				&(tx2mon.n_core), &(tx2mon.n_thread)) != 3) {
		msglog(LDMSD_LERROR, SAMP ": cannot parse '%s'.\n", path);
		fclose(socinfo);
		free(path);
		return EBADF;
	}

	fclose(socinfo);
	free(path);

	msglog(LDMSD_LINFO, SAMP ": n_cpu: %d, n_core: %d, n_thread: %d.\n",
			tx2mon.n_cpu, tx2mon.n_core, tx2mon.n_thread);

	if (TX2MON_MAX_CPU < tx2mon.n_cpu) {
		msglog(LDMSD_LWARNING, SAMP ": sampler built for max %d CPUs, system reporting %d CPUs, limiting reporting to %d.\n",
				TX2MON_MAX_CPU, tx2mon.n_cpu, TX2MON_MAX_CPU);
		tx2mon.n_cpu = TX2MON_MAX_CPU;
	}

	if (MAX_CPUS_PER_SOC < tx2mon.n_core) {
		msglog(LDMSD_LWARNING, SAMP ": sampler built for max %d cores, system reporting %d cores, limiting reporting to %d.\n",
				MAX_CPUS_PER_SOC, tx2mon.n_core, MAX_CPUS_PER_SOC);
		tx2mon.n_core = MAX_CPUS_PER_SOC;
	}

	schema = base_schema_new(base);
	if (!schema) {
		msglog(LDMSD_LERROR, SAMP ": The schema '%s' could not be created, %s.\n",
		       base->schema_name, strerror(errno));
		return errno;
	}

	tx2mon.cap = CAP_BASIC;

	/*
	 * Iterate over all CPUs.
	 */
	for (i = 0; i < tx2mon.n_cpu; i++) {
		cp = &(tx2mon.cpu[i]);

		cp->mcp = NULL;
		cp->metric_offset = ldms_schema_metric_count_get(schema);

		/*
		 * Generate the pathname to the raw file we will open and mmap.
		 */
		snprintf(buf, TMP_BUF_SIZE, TX2MON_NODE_PATH, i);
		path = realpath(buf, NULL);

		if (path == NULL) {
			msglog(LDMSD_LERROR, SAMP ": cannot resolve path for '%s'.\n",
					buf);
			return EBADF;
		}

		cp->fd = open(path, O_RDONLY);
		if (cp->fd < 0) {
			rc = errno;
			msglog(LDMSD_LERROR, SAMP ": cannot open '%s', %s.\n",
					path, strerror(rc));
			free(path);
			/*
			 * Free up previously allcoated resources, and bail.
			 */
			for (i--; i >= 0; i--) {
				cp = &(tx2mon.cpu[i]);
				munmap(cp->mcp, sizeof(struct mc_oper_region));
				close(cp->fd);
			}
				
			return rc;
		}

		free(path);

		cp->mcp = mmap(NULL, sizeof(struct mc_oper_region), PROT_READ,
						MAP_SHARED, cp->fd, 0);
		if (cp->mcp == MAP_FAILED) {
			rc = errno;
			close(cp->fd);

			/*
			 * Free up previously allcoated resources, and bail.
			 */
			for (i--; i >= 0; i--) {
				cp = &(tx2mon.cpu[i]);
				munmap(cp->mcp, sizeof(struct mc_oper_region));
				close(cp->fd);
			}
				
			return rc;
		}

		/*
		 * Check the version of the first structure (they both should
	:	 * be the same), and map that onto capabilities.
		 *
		 * TODO: Check whether we need to wait for CMD_STATUS_READY()
		 */
		if (i == 0) {
			int version = CMD_VERSION(cp->mcp->cmd_status);

			if (i >= 1) {
				tx2mon.cap |= CAP_THROTTLE;
			}
			if (i > 1) {
				msglog(LDMSD_LWARNING, SAMP ": unknown capability: %d. Ignoring.\n", i);
			}
		}

		/*
		 * TODO: build schema here.
		 */
	}

	set = base_set_new(base);
	if (!set) {
		rc = errno;

		for (i = 0; i < tx2mon.n_cpu; i++) {
			cp = &(tx2mon.cpu[i]);

			munmap(cp->mcp, sizeof(struct mc_oper_region));
			close(cp->fd);
		}
		return rc;
	}

	return 0;
}
