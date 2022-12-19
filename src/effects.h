/*
 * effects.h
 *
 * Special effects module.
 * State machines used to perform light and sound effects
 * of gingerbread house.
 *
 *  Created on: 17 gru 2022
 *      Author: andrzej
 */

#ifndef EFFECTS_H_
#define EFFECTS_H_

/**
 * Init module.
 */
void effects_init(void);
/**
 * Perform task cycle.
 * Call this frequently in a thread to
 * make effects run.
 */
void effects_cycle(void);
/**
 * De-init module - disable all effects.
 */
void effects_deinit(void);


/**
 * Start/stop effect: go to WC.
 *
 * Turns-on light in small room,
 * plays "bathroom sounds" (e.g. toilet
 * flushing) and turns off light.
 */
void effects_go_to_wc(void);

/**
 * Start/stop effect: disco.
 *
 * Plays "Bania u Cygana", flashes
 * RGB lights wildly.
 */
void effects_disco(void);

/**
 * Start/stop effect: alarm.
 *
 * Plays alarm siren sound.
 * Flashes red lights.
 */
void effects_alarm(void);

#endif /* EFFECTS_H_ */
