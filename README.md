# Arduino Serial Tap
A serial tap that allows reading and data injection into a serial connection between two devices.

This software was written for the Arduino Mega 2560, but it could probably work on an Arduino Uno or other devices programmable through the Arduino IDE 
too by utilising software serial. This is not currently implemented though since software serial might not be fast enough to ensure 
stable passthrough at high baud rates.

## Setup
The USB serial interface is used to communicate with the sniffer. Two RS-232 connectors need to be wired up to TX1/RX1 and TX2/RX2 respectively
(it is recommended to use a TTL voltage downstepper if you don't want to power the serial ports externally. Serial connectors that already have such
a chip for use with microcontrollers can be found online).

## Usage
When the USB serial port of the Arduino is opened with a software like [HTerm](https://www.der-hammer.info/pages/terminal.html), it will wait for the
serial ports to be set up. This is done with the `c ()` command. It will not open the serial ports before a configuration is supplied! However, if a simple newline
(or any invalid combination of characters for that matter) is sent, it will initialise the ports with the default settings; 9600 8N1.

**IMPORTANT: This software expects a newline character at the end of each command. If none is sent, it will take 3 seconds to time out the
command reader before execution continues.**

### Configuring the serial ports
To (re-)configure the serial ports, this command is used:
```
c (BAUDRATE, CONFIG)
```
* `BAUDRATE` can be any integer between 1 and 2,000,000  
* `CONFIG` is a 3 character string in the format `DataParityStop`  
  * `Data` can be 5, 6, 7, or 8  
  * `Parity` can be N (none), E (even) or O (odd)  
  * `Stop` can be either 1 or 2

For example, to set up the serial ports with a baud rate of 115200, 7 data bits, even parity, and 2 stop bits, the command would be
```
c (115200, 7E2)
```
The ports can be reconfigured at any time.

### Injecting data into the serial ports
To inject data into the serial communication, one can use the commands `1 ()` and `2 ()` respectively. Anything between the parentheses will be sent to either
the first or the second serial device. C escape sequences are supported (with the exception of byte and unicode sequences).

For example, to send the string 
```
super cool list
  - with tabs
  - and multiple lines
```
to serial device 1, this command would do just that:

```
1 (super cool list\n\t- with tabs\n\t- and multiple lines\n)
```

### Resetting the Arduino
If one wants to reset the Arduino through software (for example if it is in an enclosure without physical access to the reset button), the reset command can be utilised:
```
reset ()
```

### Usage overview
To get an overview over available commands, this command can be used:
```
h ()
```

### Debugging
At the moment, there is only one debug measure; having the Arduino reply back to you with the command it received. That can be toggled with this command:
```
debug ()
```
