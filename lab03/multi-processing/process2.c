#include "proc_event.h"   // This already includes "contiki.h"
#include <stdio.h>        // For printf

PROCESS(output_process, "");
// List of processes to start - both of them!
AUTOSTART_PROCESSES(&output_process, &process1);

PROCESS_THREAD(output_process, ev, data)
{
  PROCESS_BEGIN();

  while (1){

    PROCESS_WAIT_EVENT_UNTIL(ev == custom_event); // Wake up only for the custom event

    printf("Received and event!\n");

  }

  PROCESS_END();
}
