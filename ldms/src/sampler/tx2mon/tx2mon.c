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
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "ldms.h"
#include "ldmsd.h"
#include "sampler_base.h"

static ldmsd_msg_log_f msglog;
static ldms_set_t set = NULL;

#include "tx2mon.h"
#if 0
/*
 * Forward declarations.
 */
static void term(struct ldmsd_plugin *self);
static int config(struct ldmsd_plugin *self, struct attr_value_list *kwl, struct attr_value_list *avl);
static const char *usage(struct ldmsd_plugin *self);
static ldms_set_t get_set(struct ldmsd_sampler *self);			/* Obsolete */
static int sample(struct ldmsd_sampler *self);
#endif

/*
 * Plug-in data structure and access method.
 */
static struct ldmsd_sampler tx2mon_plugin = {
	.base = {
		.name = "tx2mon",
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
	return -1;
}

static const char *usage(struct ldmsd_plugin *self)
{
	return "tx2mon: Lorem Ipsum";
}

static void term(struct ldmsd_plugin *self)
{
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
