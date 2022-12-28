/*
 * templater.c
 *
 *  Created on: 19 gru 2022
 *      Author: andrzej
 */

#include "templater.h"

#include <string.h>
#include <stdio.h>

#if 0
char big_buf [2048];
unsigned bb_wi;

static void bb_append(const char * from, unsigned size)
{
    if (bb_wi + size < sizeof(big_buf) - 1)
    {
        memcpy(&big_buf[bb_wi], from, size);
        bb_wi += size;
        big_buf[bb_wi] = 0;
    }
}
#endif

char build_value_buf [128];

void create_from_template(const template_t * tpl, const tvalue_t * values,
        unsigned valcount, cbAppendDoc cb)
{
    unsigned ri = 0, ti;

    for (ti = 0; ti < tpl->count; ++ti)
    {
        unsigned vi;

        cb(&tpl->const_text[ri], tpl->inserts[ti].offset - ri);

        for (vi = 0; vi < valcount; ++vi)
        {
            if (0 == strcmp(tpl->inserts[ti].key, values[vi].key))
            {
                if (values[vi].arg)
                {
                    // jeez i am an idiot
                    unsigned slen = snprintf(build_value_buf, sizeof(build_value_buf),
                            values[vi].value_fmt, *((int *) values[vi].arg));
                    cb(build_value_buf, slen);
                }
                else
                {
                    cb(values[vi].value_fmt, strlen(values[vi].value_fmt));
                }

                break;
            }
        }
        ri = tpl->inserts[ti].offset;
    }

    cb(&tpl->const_text[ri], strlen(&tpl->const_text[ri]));

}
