/*
 * templater.h
 *
 *  Created on: 19 gru 2022
 *      Author: andrzej
 */

#ifndef TEMPLATER_H_
#define TEMPLATER_H_

typedef struct
{
    unsigned offset;
    const char * key;
} insertion_t;

typedef struct
{
    const char * const_text;
    const insertion_t * inserts;
    unsigned count;
} template_t;

typedef struct {
    const char * key;
    const char * value_fmt;
    const void * arg;
} tvalue_t;

typedef void (*cbAppendDoc)(const char * txt, unsigned size);

void create_from_template(const template_t * tpl, const tvalue_t * values,
        unsigned valcount, cbAppendDoc cb);

void template_get_harmonogram(template_t * pTemplate);

#endif /* TEMPLATER_H_ */
