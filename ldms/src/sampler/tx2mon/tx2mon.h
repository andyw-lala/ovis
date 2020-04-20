/*
 * tx2mon.h -	LDMS sampler for basic Marvell TX2 chip telemetry.
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

/*
 * Header file from github.com/jchandra-cavm/tx2mon.git, which
 * is required to be checked out elsewhere on the build system,
 * and pointed to during the configure phase.
 *
 * TODO: insert exact configure command line required here.
 */
#include "mc_oper_region.h"

/*
 * Forward declarations.
 */
static void term(struct ldmsd_plugin *self);
static int config(struct ldmsd_plugin *self, struct attr_value_list *kwl, struct attr_value_list *avl);
static const char *usage(struct ldmsd_plugin *self);
static ldms_set_t get_set(struct ldmsd_sampler *self);			/* Obsolete */
static int sample(struct ldmsd_sampler *self);

static int create_metric_set(base_data_t base);

/*
 * Location of the sysfs entries created by the kernel module.
 *
 * NOTE: TX2MON_NODE_PATH is a snprintf() string to create actual path.
 */
#define	TX2MON_SYSFS_PATH	"/sys/bus/platform/devices/tx2mon/"
#define	TX2MON_SOCINFO_PATH	TX2MON_SYSFS_PATH "socinfo"
#define	TX2MON_NODE_PATH	TX2MON_SYSFS_PATH "node%d_raw"

/*
 * Max number of CPUs (TX2 chips) supported.
 */
#define	TX2MON_MAX_CPU	(2)

/*
 * Size of temporary buffer used when registering schema etc
 */
#define	TMP_BUF_SIZE	(512)

/*
 * Set of possible capabilities supported by the kernel module.
 */
typedef enum {CAP_BASIC = 0x00, CAP_THROTTLE = 0x01} tx2mon_cap;
	
/*
 * Housekeeping structures used internally.
 */

/*
 * Per-CPU record keeping
 */
struct cpu_info {
	int	fd;		/* fd of raw file opened */
	int	metric_offset;	/* starting offset into schema for this CPU */
	struct	mc_oper_region *mcp;	/* mmapped data structure (from fd) */
};

/*
 * Per-sampler record keeping
 *
 * TODO: Investigate pulling base & set into here.
 */
struct tx2mon_sampler {
	int	n_cpu;		/* number of CPUs (e.g. TX2 chips) present */
	int	n_core;		/* cores *per CPU* */
	int	n_thread;	/* threads *per core* (unused currently) */
	tx2mon_cap	cap;	/* capabilites of kernel module */

	struct cpu_info cpu[TX2MON_MAX_CPU];
} ;
