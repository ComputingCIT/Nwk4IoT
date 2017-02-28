#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>


PROCESS(base, "");
AUTOSTART_PROCESSES(&base);

#define MY_CHANNEL  29
enum{
  REPLY_SUCCESS,
  REPLY_STOP,
};

const char* success_prefix = "Well done ";
uint8_t success_prefix_len = 10;
const char* stop_prefix = "Your message has already been received ";
uint8_t stop_prefix_len = 39;

struct sender{
  char name[20];
  uint8_t name_len;
};

struct unicast_conn myuc;
struct sender senders[20]; // Array to hold the names of the senders
uint8_t num_senders;

/*
 * Send a reply to the sender to notify of success, or to ask to stop
 */
void reply_to_sender(const linkaddr_t *sender, const struct sender *name, uint8_t status)
{
  char *pkt = packetbuf_dataptr();
  uint8_t offset;

  // Copy the header of the packet
  if (status == REPLY_SUCCESS){
    offset = success_prefix_len;
    memcpy(pkt, success_prefix, offset);
  }else if (status == REPLY_STOP){
    offset = stop_prefix_len;
    memcpy(pkt, stop_prefix, offset);
  }

  // Copy the name of the sender
  memcpy(pkt+offset, name->name, name->name_len);

  packetbuf_set_datalen(offset+name->name_len);
  // Packet ready, send the unicast
  unicast_send(&myuc, sender);
  printf("Replied to %x.%x status %u\n", sender->u8[0], sender->u8[1], status);
}

void my_recv(struct unicast_conn *conn, const linkaddr_t *from)
{
  const char *txt, *name;
  uint8_t name_len;
  uint8_t i;

  txt = (char*)packetbuf_dataptr();
  name_len = packetbuf_datalen();
  name = txt+12;

  // Make sure we haven't already received from this sender
  for (i=0; i<num_senders; i++){
    if (!strncmp(senders[i].name, name, name_len)){
      // You've already sent us a packet...
      reply_to_sender(from, &senders[i], REPLY_STOP);
      printf("Received duplicate from %s\n", senders[i].name);
      break;
    }
  }

  if (i == num_senders){
    // New sender, add to list first
    strncpy(senders[num_senders].name, name, name_len);
    senders[num_senders].name[name_len] = '\0';
    senders[num_senders].name_len = name_len;
    num_senders ++;
    // Send a reply now
    reply_to_sender(from, &senders[num_senders-1], REPLY_SUCCESS);
    printf("Received message from %s\n", senders[num_senders-1].name);
  }
}

/*
 * Implement this so we know if the reply was successful or not
 */
void my_sent(struct unicast_conn *conn, int status, int num_tx)
{
  if (status != MAC_TX_OK){
    // TODO - try again
    printf("Couldn't transmit\n");
  }
}

struct unicast_callbacks my_callbacks = {my_recv, my_sent};


PROCESS_THREAD(base, ev, data)
{
  PROCESS_BEGIN();
  
  // Open rime channel
  unicast_open(&myuc, MY_CHANNEL, &my_callbacks);

  printf("Waiting for clients\n");
  // Leave the connection on but exit the process - we don't need it
  PROCESS_END();
}
