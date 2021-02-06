# Arduino Serial Tap
A serial tap that allows reading of and data injection into a serial connection between two devices.

This software was written for the Arduino Mega 2560, but it could potentially work on an Arduino Uno or other devices programmable through the Arduino IDE 
too by utilising software serial. This is not currently implemented though since software serial might not be fast enough to ensure 
stable passthrough at high baud rates.

## Setup
The USB serial interface is used to communicate with the sniffer. Two RS-232 connectors need to be wired up to TX1/RX1 and TX2/RX2 respectively
(it is recommended to use a TTL voltage downstepper, otherwise the serial ports will have to be powered externally. Serial connectors that already have such
a chip for use with microcontrollers can be found online).

## Usage
When the USB serial port of the Arduino is opened with a software like [HTerm](https://www.der-hammer.info/pages/terminal.html) or [CoolTerm](https://freeware.the-meiers.org), it will wait for the
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

### Configuring operating modes
This serial tap supports two modes; inject and realtime. Getting both modes operational
requires additional switching hardware (example schematic coming soon). By default,
realtime mode is enabled.

The difference between the two modes is as follows:
* Realtime mode:
  * Both the RX and TX lines of the two serial devices are directly connected to each  
  other. The two lines are connected to the RX lines of the two UARTs of the arduino  
  to allow for data reading. This setup is useful when communication is time critical,  
  since the arduino does not need to relay data between the devices.
* Inject mode:
  * The RX and TX lines of one serial device are connected to one of the arduino's  
  UARTs respectively. The arduino needs to relay data between the devices to allow  
  commmunication between them. This allows for data injection, since the two devices  
  can be treated as two seperate serial commmunications.

To switch between modes, the switching command can be used:
```
m (MODE)
```
`MODE` can be either `inject` or `realtime`.  
If no mode or an invalid mode is supplied, the current mode will be printed.

### Injecting data into the serial communication
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
