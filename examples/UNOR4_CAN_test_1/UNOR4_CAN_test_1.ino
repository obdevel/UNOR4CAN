
#include <UNOR4CAN.h>

// void user_callback_handler(can_callback_args_t *p_args);

volatile bool  new_msg_available = false;
volatile can_frame_t imsg;

uint8_t payload[8] = { 0x90, 1, 2, 3, 4, 5, 6, 7 };       // test data payload for sending CAN messages

UNOR4CAN can;   // CAN bus object

///

void setup(void) {

  uint32_t stime = millis();

  Serial.begin(115200);
  while (!Serial && millis() - stime < 5000) {};

  Serial.println();
  Serial.println("> Arduino UNO R4 CAN test sketch 0 1");

  can.set_can_bitrate(CanBitRate::BR_125k);           // limited to BR_125k, BR_250k, BR_500k, BR_1000k
  can.set_callback(user_callback_handler);            // register our handler for CAN bus events
  bool ok = can.begin();                              // start the CAN bus peripheral

  Serial.print("> CAN begin returns ");
  Serial.println(ok);

  Serial.println("> end of setup");
  Serial.println();
}

///

void loop(void) {

  char c;
  int r;
  can_frame_t omsg;

  // a ne CAN message has been received
  if (new_msg_available) {
    new_msg_available = false;
    Serial.println(format_can_message((const can_frame_t *)&imsg));
  }

  // handle user input
  if (Serial.available()) {
    c = Serial.read();

    switch (c) {
      case 's':                     // send a standard 11-bit CAN message
        omsg.id = 12345;
        omsg.id_mode = CAN_ID_MODE_STANDARD;
        omsg.type = CAN_FRAME_TYPE_DATA;
        omsg.data_length_code = 8;
        memcpy(omsg.data, payload, 8);

        r = can.send(&omsg);

        Serial.print("> CAN write of standard frame returns: ");
        Serial.println((r ? "ok" : "fail"));
        break;

      case 'x':                     // send an extended 29-bit CAN message
        omsg.id = 123456789;
        omsg.id_mode = CAN_ID_MODE_EXTENDED;
        omsg.type = CAN_FRAME_TYPE_DATA;
        omsg.data_length_code = 8;
        memcpy(omsg.data, payload, 8);

        r = can.send(&omsg);

        Serial.print("> CAN write of extended frame returns: ");
        Serial.println((r ? "ok" : "fail"));
        break;

      case 'r':                     // send a remote transfer request (RTR) frame
        omsg.id = 12345;
        omsg.id_mode = CAN_ID_MODE_STANDARD;
        omsg.type = CAN_FRAME_TYPE_REMOTE;
        omsg.data_length_code = 0;

        r = can.send(&omsg);

        Serial.print("> CAN write of remote frame returns: ");
        Serial.println((r ? "ok" : "fail"));
        break;

      case '\n':
      case '\r':
        Serial.println();
        break;
    }
  }
}

//
/// user callback function called by the CAN driver to surface various events
/// runs in interrupt context so don't do any heavy processing
//

void user_callback_handler(can_callback_args_t *p_args) {

  switch (p_args->event) {
    case CAN_EVENT_TX_COMPLETE:
      Serial.println("> handler: message sent ok");
      break;

    case CAN_EVENT_RX_COMPLETE:
      Serial.println("> handler: message received");

      imsg.id = p_args->frame.id;
      imsg.id_mode = p_args->frame.id_mode;
      imsg.type = p_args->frame.type;
      imsg.data_length_code = p_args->frame.data_length_code;
      imsg.options = p_args->frame.options;
      memcpy((void *)imsg.data, p_args->frame.data, p_args->frame.data_length_code);

      new_msg_available = true;
      break;

    case CAN_EVENT_ERR_WARNING:          /* error warning event */
    case CAN_EVENT_ERR_PASSIVE:          /* error passive event */
    case CAN_EVENT_ERR_BUS_OFF:          /* error bus off event */
    case CAN_EVENT_BUS_RECOVERY:         /* Bus recovery error event */
    case CAN_EVENT_MAILBOX_MESSAGE_LOST: /* overwrite/overrun error event */
    case CAN_EVENT_ERR_BUS_LOCK:         /* Bus lock detected (32 consecutive dominant bits). */
    case CAN_EVENT_ERR_CHANNEL:          /* Channel error has occurred. */
    case CAN_EVENT_TX_ABORTED:           /* Transmit abort event. */
    case CAN_EVENT_ERR_GLOBAL:           /* Global error has occurred. */
    case CAN_EVENT_TX_FIFO_EMPTY:        /* Transmit FIFO is empty. */
      Serial.print("> handler: error = ");
      Serial.println(p_args->event);
      break;
  }

  return;
}

//
/// format a CAN message as a char string, return a pointer to it
//

char *format_can_message(const can_frame_t *msg) {

  static char mbuff[64], dbuff[8];

  sprintf(mbuff, "[%lX] [%d] [ ", msg->id, msg->data_length_code);

  for (byte i = 0; i < msg->data_length_code; i++) {
    sprintf(dbuff, "%02X ", msg->data[i]);
    strcat(mbuff, dbuff);
  }

  strcat(mbuff, " ] ");

  if (msg->id_mode == CAN_ID_MODE_STANDARD) {
    strcat(mbuff, "S ");
  } else if (msg->id_mode == CAN_ID_MODE_EXTENDED) {
    strcat(mbuff, "X ");
  }

  if (msg->type == CAN_FRAME_TYPE_DATA) {
    strcat(mbuff, "D");
  } else if (msg->type == CAN_FRAME_TYPE_REMOTE) {
    strcat(mbuff, "R");
  }

  return mbuff;

}
