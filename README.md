# Arduino library for the CAN bus peripheral on the UNO and Nano R4

The CAN bus library currently (as of Sep 2025) included with the Arduino core for the Renesas R4 boards has a serious deficiency: it does not support remote transfer request (RTR) frames.

This library is a quick and dirty hack to address this issue, until such time as the Arduino project fixes it in the official core and library.

It also correctly uses multiple transmit mailboxes when sending a message, depending on the frame type. Thus, you do not need to wait for the preceding frame to be forwarded to the bus before sending the next. Similarly, there are multiple receive mailboxes.

Transmit and receive mailboxes are configured as:

  - 0-7   - standard frames x8
  - 8-11  - remote frames x4
  - 12-15 - extended frames x4

If you do not require CAN RTR frames, I recommend you use the official Arduino_CAN library and its example sketches. This library will likely be deprecated once the Arduino project gets its act together or someone provided the appropriate PR to fix this deficiency.

## Hardware

UNO R4 Minima and WiFi, Nano R4

## Documentation

See the included example sketch 

## Limitations

Sent and received messages are not buffered by the code (although the chip has numerous mailboxes).

The example sketch could be modified to implement software message buffers.

CAN message filtering is not supported, but would be straightforward to add. (I have no need for this).

IO pins are fixed: CAN TX = D4, CAN RX = D5.

## Testing

Tested on Arduino UNO R4 Minima and Nano R4 with an MCP2562 CAN bus transceiver chip at 125kbps. (NB: it must be a 5V device)

