
#include "contiki.h"          // Always need to have contiki.h
#include <stdio.h>            // In case you use printf
#include "lib/sensors.h"      // For sensing macros
#include "dev/light-sensor.h" // To access the light sensor (hamamatsu)
#include "sys/etimer.h"       // The etimer

/* Declare and autostart the process */
PROCESS(light_sensing, "");
AUTOSTART_PROCESSES(&light_sensing);

PROCESS_THREAD(light_sensing, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  etimer_set(&et, CLOCK_SECOND);
  while (1){
    /* Yield control, wait for an event */
    PROCESS_YIELD();

    /* An event happened - only interested in the timer event */
    if (ev == PROCESS_EVENT_TIMER){ // should also check if it's OUR timer!

      /* Switch light sensor ON */
      /* It's important to keep sensors active
       * for as little time as possible.
       */
      SENSORS_ACTIVATE(light_sensor);

      /* Read light sensor */
      int light = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
      printf("Light: %d\n", light);
      etimer_reset(&et);

      /* Switch light sensor OFF again to conserve energy */
      SENSORS_DEACTIVATE(light_sensor);
    }
  }

  PROCESS_END();
}
