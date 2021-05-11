/*===================================================

                  AVN Interface Mgr


GENERAL DESCRIPTION




Copyright (c) 2018 by Digen, Incorporated.  All Rights Reserved.
Digen Confidential and Proprietary.
=====================================================*/

/*====================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------

from https://gist.github.com/ryankurte/8143058

*/

/*=====================================================

                     INCLUDE FILES FOR MODULE

=====================================================*/
#ifndef _AVN_INTERFACE_MGR_H
#define _AVN_INTERFACE_MGR_H


#include <stdint.h>
#include <termios.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>


#include "msg_if.h"

#define BUFF_SIZE 100
#define POLL_TIMEOUT 2000
//#define POLL_TIMEOUT 10 //10 msec

#define DEFAULT_BAUD_RATE		115200



#ifdef __cplusplus
extern "C" {
#endif


/**
 * @struct Serial device structure.
 * Encapsulates a serial connection.
 */
struct serial_s {
    int fd;                  //>! Connection file descriptor.
    int state;               //>! Signifies connection state.
    int running;             //>! Signifies thread state.

    char rxbuff[BUFF_SIZE];  //>! Buffer for RX data.
    
    int start, end;          //>! Pointers to start and end of buffer.

    pthread_t rx_thread;     //>! Listening thread.
};



typedef struct serial_s serial_t;



serial_t* serial_fd;


/**
 * Create the serial structure.
 * Convenience method to allocate memory
 * and instantiate objects.
 * @return serial structure.
 */
serial_t* serial_create();

/**
 * Destroy the serial structure
 */
void serial_destroy(serial_t* s);

/**
 * Connect to a serial device.
 * @param s - serial structure.
 * @param device - serial device name.
 * @param baud - baud rate for connection.
 * @return -ve on error, 0 on success.
 */
int serial_connect(serial_t* s, char device[], int baud);

/**
 * Send data.
 * @param s - serial structure.
 * @param data - character array to transmit.
 * @param length - size of the data array.
 */
int serial_send(serial_t* s, uint8_t data[], int length);

//For common send
int send_to_serial(avn_data_t* data, int length);


/**
 * Send a single character.
 * @param s - serial structure.
 * @param data - single character to be sent.
 */
void serial_put(serial_t* s, uint8_t data);

/**
 * Determine how much data is available
 * in the serial buffer.
 * @param s - serial structure.
 * @return number of characters available.
 */
int serial_available(serial_t* s);

/**
 * Fetch one char from the serial buffer.
 * @param s - serial structure.
 * @return character. Null if empty.
 */
char serial_get(serial_t* s);

/**
 * Fetch one char from the serial buffer.
 * Blocks until data becomes available.
 * @param s - serial structure.
 * @return character.
 */
char serial_blocking_get(serial_t* s);

/**
 * Clear the serial buffer.
 * @param s - serial structure.
 */
void serial_clear(serial_t* s);

/**
 * Close the serial port.
 * @param s - serial structure.
 * @return value of close().
 */
int serial_close(serial_t* s);

#ifdef __cplusplus
}
#endif

#endif //_AVN_INTERFACE_MGR_H



