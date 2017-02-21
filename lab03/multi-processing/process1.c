#include "contiki.h"
#include "sys/etimer.h"
#include "proc_event.h"   // Also include this, for the custom event

PROCESS(process1, "");

PROCESS_THREAD(process1, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  etimer_set(&et, CLOCK_SECOND*5);
  custom_event = process_alloc_event();   // Allocate the custom event

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    // Broadcast the custom event to all processes
    process_post(PROCESS_BROADCAST, custom_event, NULL);

    etimer_reset(&et);
  }

  PROCESS_END();
}
