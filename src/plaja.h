/*
 * plaja.h
 *
 *  Created on: 10 gru 2022
 *      Author: andrzej
 */

#ifndef PLAJA_H_
#define PLAJA_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
        PLAJA_CMD_PLAY_NEXT = 0x01,
        PLAJA_CMD_PLAY_PREV = 0x02,
        PLAJA_CMD_PLAY_INDX = 0x03,
        PLAJA_CMD_VOL_UP    = 0x04,
        PLAJA_CMD_VOL_DN    = 0x05,
        PLAJA_CMD_VOL_SET   = 0x06,
        PLAJA_CMD_EQ_SET    = 0x07,
        PLAJA_CMD_LOOP_OFF  = 0x08,
        PLAJA_CMD_SET_DEV   = 0x09,
        PLAJA_CMD_SET_SLEEP = 0x0A,
        PLAJA_CMD_RESET     = 0x0C,
        PLAJA_CMD_PLAY      = 0x0D,
        PLAJA_CMD_PAUSE     = 0x0E,
        PLAJA_CMD_SET_TRK   = 0x0F,
        PLAJA_CMD_SET_AMP   = 0x10,
        PLAJA_CMD_LOOP_ALL  = 0x11,
        PLAJA_CMD_PLAY_MP3  = 0x12,
        PLAJA_CMD_PLAY_ADV  = 0x13,
        PLAJA_CMD_PLAY_FOL  = 0x14,
        PLAJA_CMD_INT_ADV   = 0x15,
        PLAJA_CMD_STOP      = 0x16,
        PLAJA_CMD_LOOP_FLD  = 0x17,
        PLAJA_CMD_RANDOM    = 0x18,
        PLAJA_CMD_LOOP_TRK  = 0x19,
        PLAJA_CMD_SET_DAC   = 0x1A,

        PLAJA_QUERY_DEVICE   = 0x3F,
        PLAJA_QUERY_ERROR    = 0x40,
        PLAJA_QUERY_FEEDBACK = 0x41,
        PLAJA_QUERY_STATUS   = 0x42,
        PLAJA_QUERY_VOLUME   = 0x43,
        PLAJA_QUERY_EQ       = 0x44,
        PLAJA_QUERY_ROOT_USB = 0x47,
        PLAJA_QUERY_ROOT_SD  = 0x48,
        PLAJA_QUERY_CUR_USB  = 0x4B,
        PLAJA_QUERY_CUR_SD   = 0x4C,
        PLAJA_QUERY_FOLDER   = 0x4E,
        PLAJA_QUERY_DEV_CNT  = 0x4F,
} EPlayaRequest_t;

enum {
    TRACK_TOILET_FLUSH      = 1,
    TRACK_DOOR_CREAK        = 2,
    TRACK_PEE               = 3,
    TRACK_CAR_ALARM         = 4,
    TRACK_BANIA_U_CYGANA    = 5,
};

void plaja_init(void);
void plaja_set_on(bool enable);
void plaja_action(EPlayaRequest_t request, uint16_t parameter);

#endif /* PLAJA_H_ */
