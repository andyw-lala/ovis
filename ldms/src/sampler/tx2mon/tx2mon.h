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
 * Forward declarations.
 */
static void term(struct ldmsd_plugin *self);
static int config(struct ldmsd_plugin *self, struct attr_value_list *kwl, struct attr_value_list *avl);
static const char *usage(struct ldmsd_plugin *self);
static ldms_set_t get_set(struct ldmsd_sampler *self);			/* Obsolete */
static int sample(struct ldmsd_sampler *self);
