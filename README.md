# Arduino library for the CAN bus peripheral on the UNO R4

The CAN bus library currently included with the Arduino core for the UNO R4 has some serious deficiencies.
It only supports extended (29-bit) CAN frames and not standard (11-bit) or remote transfer request (RTR) frames.

This library is a quick and dirty hack to address these issues, until Arduino fix them in the official core and library.

## Hardware

UNO R4 Minima and WiFi

## Documentation

See the included example sketch 

## Limitations

Received messages are not buffered by the code (although the chip peripheral has numerous mailboxes)
IO pins are fixed: CAN TX = D4, CAN RX = D5.

## Testing

Tested with an MCP2562 CAN bus transceiver chip at 125kbps.

