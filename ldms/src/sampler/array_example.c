/* -*- c-basic-offset: 8 -*-
 * Copyright (c) 2011-2015 Open Grid Computing, Inc. All rights reserved.
 * Copyright (c) 2011-2015 Sandia Corporation. All rights reserved.
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
 * \file array_example.c
 * \brief Example of an ldmsd sampler plugin that uses array metric.
 */
#include <errno.h>
#include "ldms.h"
#include "ldmsd.h"

static ldms_schema_t schema;
static ldms_set_t set;
static ldmsd_msg_log_f msglog;

struct array_construct {
	const char *name;
	enum ldms_value_type type;
	int n;
};

struct array_construct array_contruct_entries[] = {
	{"u8_array", LDMS_V_U8_ARRAY, 10},
	{"s8_array", LDMS_V_S8_ARRAY, 10},
	{"u16_array", LDMS_V_U16_ARRAY, 10},
	{"s16_array", LDMS_V_S16_ARRAY, 10},
	{"u32_array", LDMS_V_U32_ARRAY, 10},
	{"s32_array", LDMS_V_S32_ARRAY, 10},
	{"u64_array", LDMS_V_U64_ARRAY, 10},
	{"s64_array", LDMS_V_S64_ARRAY, 10},
	{"float_array", LDMS_V_F32_ARRAY, 10},
	{"double_array", LDMS_V_D64_ARRAY, 10},
	{NULL, LDMS_V_NONE, 10},
};

static int create_metric_set(const char *instance_name)
{
	int rc;

	schema = ldms_schema_new("array_example");
	if (!schema)
		return ENOMEM;

	struct array_construct *ent = &array_contruct_entries[0];

	while (ent->name) {
		rc = ldms_schema_array_metric_add(schema, ent->name, ent->type, ent->n);
		if (rc < 0) {
			rc = ENOMEM;
			goto err;
		}
		ent++;
	}

	set = ldms_set_new(instance_name, schema);
	if (!set) {
		rc = errno;
		goto err;
	}

	return 0;
 err:
	ldms_schema_delete(schema);
	return rc;
}
static int config(struct attr_value_list *kwl, struct attr_value_list *avl)
{
	char *value;
	const char *producer_name = av_value(avl, "producer");
	if (!producer_name) {
		msglog(LDMSD_LERROR, "array_example: missing producer\n");
		return ENOENT;
	}

	value = av_value(avl, "instance");
	if (!value) {
		msglog(LDMSD_LERROR, "array_example: missing instance.\n");
		return ENOENT;
	}

	if (set) {
		msglog(LDMSD_LERROR, "array_example: Set already created.\n");
		return EINVAL;
	}

	int rc = create_metric_set(value);
	if (rc) {
		msglog(LDMSD_LERROR, "array_example: failed to create a metric set.\n");
		return rc;
	}
	ldms_set_producer_name_set(set, producer_name);
	return 0;
}

static int sample(void)
{
	ldms_transaction_begin(set);
	int i, mid;
	static uint8_t off = 0;
	struct array_construct *ent = array_contruct_entries;
	union ldms_value v;
	mid = 0;
	while (ent->name) {
		for (i = 0; i < ent->n; i++) {
			switch (ent->type) {
			case LDMS_V_S8_ARRAY:
				v.v_s8 = -i + off;
				break;
			case LDMS_V_U8_ARRAY:
				v.v_u8 = i + off;
				break;
			case LDMS_V_S16_ARRAY:
				v.v_s16 = -i + off;
				break;
			case LDMS_V_U16_ARRAY:
				v.v_u16 = i + off;
				break;
			case LDMS_V_S32_ARRAY:
				v.v_s32 = -i + off;
				break;
			case LDMS_V_U32_ARRAY:
				v.v_u32 = i + off;
				break;
			case LDMS_V_S64_ARRAY:
				v.v_s64 = -i + off;
				break;
			case LDMS_V_U64_ARRAY:
				v.v_u64 = i + off;
				break;
			case LDMS_V_F32_ARRAY:
				v.v_f = i/10.0 + off;
				break;
			case LDMS_V_D64_ARRAY:
				v.v_d = i/10.0 + off;
				break;
			default:
				v.v_u64 = 0;
			}
			ldms_array_metric_set(set, mid, i, &v);
		}
		mid++;
		ent++;
	}
	off++;
	ldms_transaction_end(set);
	return 0;
}

static ldms_set_t get_set()
{
	return set;
}

static void term(void)
{
	if (schema)
		ldms_schema_delete(schema);
	schema = NULL;
	if (set)
		ldms_set_delete(set);
	set = NULL;
}

static const char *usage(void)
{
	return  "config name=array_example producer=<prod_name> instance=<inst_name>\n"
		"    <prod_name>  The producer name\n"
		"    <inst_name>  The instance name\n";
}

static struct ldmsd_sampler array_example_plugin = {
	.base = {
		.name = "array_example",
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
	return &array_example_plugin.base;
}