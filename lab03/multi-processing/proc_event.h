#ifndef __PROC_EVENT_H_
#define __PROC_EVENT_H_

#include "contiki.h"

/*
 * This file defines the handle of the first process
 * and the custom event,
 * so they are accessible from both C files.
 */
PROCESS_NAME(process1);
process_event_t custom_event;

#endif
