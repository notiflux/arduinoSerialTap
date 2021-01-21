#include <avr/wdt.h>
#include "serialTap.h"

//
// newer arduinos keep the watchdog enabled after reset, so we need to
// disable it manually in the pre boot stage
//
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

void wdt_init(void)
{
   MCUSR = 0;
   wdt_disable();

   return;
}

void configurePorts (const char arg[], const int i) {
  char baudC[8]   = {'0'};
  char config[4]  = {'0'};
  long baud = 0;

  //
  // BAUDRATE is the only parameter that's variable in length.
  // all other parameters and syntax characters amount to 9 chars.
  // if the user supplies less than 10 characters, the input cannot
  // be valid so we assign default values instead.
  //
  if (i > 9) {
    memcpy (baudC, arg + 3, i - 9);
    baud = atol (baudC);
    //
    // the configuration always has a length of 3 and is located 4
    // chars before the newline character.
    //
    memcpy (config, arg + i - 4, 3);
  }

  uint8_t c;
  if      (strcmp (config, "5N1") == 0)
    c = SERIAL_5N1;
  else if (strcmp (config, "5N2") == 0)
    c = SERIAL_5N2;
  else if (strcmp (config, "5E1") == 0)
    c = SERIAL_5E1;
  else if (strcmp (config, "5E2") == 0)
    c = SERIAL_5E2;
  else if (strcmp (config, "5O1") == 0)
    c = SERIAL_5O1;
  else if (strcmp (config, "5O2") == 0)
    c = SERIAL_5O2;
  else if (strcmp (config, "6N1") == 0)
    c = SERIAL_6N1;
  else if (strcmp (config, "6N2") == 0)
    c = SERIAL_6N2;
  else if (strcmp (config, "6E1") == 0)
    c = SERIAL_6E1;
  else if (strcmp (config, "6E2") == 0)
    c = SERIAL_6E2;
  else if (strcmp (config, "6O1") == 0)
    c = SERIAL_6O1;
  else if (strcmp (config, "6O2") == 0)
    c = SERIAL_6O2;
  else if (strcmp (config, "7N1") == 0)
    c = SERIAL_7N1;
  else if (strcmp (config, "7N2") == 0)
    c = SERIAL_7N2;
  else if (strcmp (config, "7E1") == 0)
    c = SERIAL_7E1;
  else if (strcmp (config, "7E2") == 0)
    c = SERIAL_7E2;
  else if (strcmp (config, "7O1") == 0)
    c = SERIAL_7O1;
  else if (strcmp (config, "7O2") == 0)
    c = SERIAL_7O2;
  else if (strcmp (config, "8N1") == 0)
    c = SERIAL_8N1;
  else if (strcmp (config, "8N2") == 0)
    c = SERIAL_8N2;
  else if (strcmp (config, "8E1") == 0)
    c = SERIAL_8E1;
  else if (strcmp (config, "8E2") == 0)
    c = SERIAL_8E2;
  else if (strcmp (config, "8O1") == 0)
    c = SERIAL_8O1;
  else if (strcmp (config, "8O2") == 0)
    c = SERIAL_8O2;
  else {
    //
    // default case. we get here when the user supplies an
    // unsupported configuration.
    //
    c = SERIAL_8N1;
    config[0] = '8';
    config[1] = 'N';
    config[2] = '1';
  }

  if (baud < 1 || baud > 2000000)
    baud = 9600;

  s[0]->print ("initialising serial ports with these settings: ");
  s[0]->print (baud);
  s[0]->print (" ");
  s[0]->print (config);
  s[0]->println ();

  s[1]->begin (baud, c);
  s[2]->begin (baud, c);
}

void help () {
  s[0]->print ("\n\n--- ARDUINO MEGA SERIAL TAP HELP ---\n\n");
  s[0]->print ("c (BAUDRATE, CONFIGURATION)   -   configure the RS-232 tap ports\n");
  s[0]->print ("    BAUDRATE can be a number between 1 and 2,000,000\n");
  s[0]->print ("    CONFIGURATION is a 3-character-combination of data bits, parity and stop bits\n");
  s[0]->print ("        supported data bits: 5, 6, 7, 8\n");
  s[0]->print ("        supported parities:  N (none), E (even parity), O (odd parity)\n");
  s[0]->print ("        supported stop bits: 1, 2\n");
  s[0]->print ("\n");
  s[0]->print ("    examples:\n");
  s[0]->print ("        c (115200, 8N1)\n");
  s[0]->print ("        c (5579, 6E2)\n");
  s[0]->print ("\n\n");
  s[0]->print ("1 (STRING) / 2 (STRING)   -   inject data into serial communication between tapped devices\n");
  s[0]->print ("    1 (): send data to serial device 1\n");
  s[0]->print ("    2 (): send data to serial device 2\n");
  s[0]->print ("\n");
  s[0]->print ("    C escape sequences are supported (except for bytes and unicode)\n");
  s[0]->print ("\n");
  s[0]->print ("    examples:\n");
  s[0]->print ("        1 (Hello, World!)\n");
  s[0]->print ("        2 (printf (\"Hello, robot!\\n\"))\n");
  s[0]->print ("\n\n");
  s[0]->print ("h ()   -   display this help message\n");
  s[0]->print ("\n\n");
  s[0]->print ("debug ()   -   toggle debug mode\n");
  s[0]->print ("    toggles echo of commands received\n");
  s[0]->print ("\n\n");
  s[0]->print ("reset ()   -   reset arduino\n");
  s[0]->print ("    this can be useful when you don't have access to the physical reset button\n");
}

int setupTrap () {
  int i = 0;
  char arg[20] = {'\0'};
  long time = 0;

  while (true) {
    if (s[0]->available () > 0) {
      time = millis ();
      //
      // we read the input stream byte by byte and cast it to a character.
      // we expect the command to end with a newline character, if we don't
      // get one, we want to stop reading after 20 characters so we don't
      // overflow the array.
      //
      if (i == 20) {
        break;
      }
      arg[i] = (char) (s[0]->read ());
      if (arg[i] == '\n') {
        break;
      }
      ++i;
    }
    //
    // if there's no new data after 3 seconds, we assume the data
    // transmission is over.
    //
    else if (time > 0 && millis () - time > 3000) {
      break;
    }
  }
  if (strstr (arg, "h ()") == &arg[0]) {
    help ();
    return 1;
  }
  else {
    configurePorts (arg, i);
    return 0;
  }
  return 1;
}

void softReset () {
  wdt_enable(WDTO_15MS);
  while (1);
}

void setup () {
  int trapState = 1;

  s[0]->begin (115200);
  s[0]->print ("--- ARDUINO MEGA SERIAL TAP ---\n");
  s[0]->print ("to configure, type \"c (BAUDRATE, CONFIGURATION)\"\n");
  s[0]->print ("for a list of available commands and further explanation, type \"h ()\"\n");

  //
  // make sure we stay in setup until a configuration is set.
  // calling h () should not trigger a jump to loop ().
  //
  
  while (trapState == 1) {
    trapState = setupTrap ();
  }
}

void loop () {
  if (s[0]->available () > 0) {
    int i = 0;
    long time = 0;
    char arg[2048] = {'\0'};
    char send[2044] = {'\0'};
    char currentByte = '\0';
    bool escape = false;
    bool done = false;
    bool flagged = false;
    
    while (true) {
      if (s[0]->available () > 0) {
        //
        // break before array overflow.
        //
        if (i == 2048) {
          flagged = true;
          break;
        }

        time = millis ();
        currentByte = (char) (s[0]->read ());
        if (currentByte == '\\' && !escape) {
          escape = true;
          continue;
        }
        else {
          //
          // handle escape sequences. escape was set in the last iteration,
          // the counter was not increased. leading backslash does not appear
          // in the array.
          //
          if (escape) {
            escape = false;
            switch (currentByte) {
              case 'a':
                //
                // alert
                //
                arg[i] = (char) 0x07;
                break;
              case 'b':
                //
                // backspace
                //
                arg[i] = (char) 0x08;
                break;
              case 'e':
                //
                // escape
                //
                arg[i] = (char) 0x1B;
                break;
              case 'f':
                //
                // form feed
                //
                arg[i] = (char) 0x0C;
                break;
              case 'n':
                //
                // line feed
                //
                arg[i] = (char) 0x0A;
                break;
              case 'r':
                //
                // carriage return
                //
                arg[i] = (char) 0x0D;
                break;
              case 't':
                //
                // horizontal tab
                //
                arg[i] = (char) 0x09;
                break;
              case 'v':
                //
                // vertical tab
                //
                arg[i] = (char) 0x0B;
                break;
              default:
                //
                // all other escape sequences simply require the backslash in front
                // of them to be removed. unsupported escape sequences will be
                // treated as normal characters, without the leading backslash.
                //
                arg[i] = currentByte;
            }
          }
          else {
            arg[i] = currentByte;
          }
        }
        //
        // we expect only one newline per command
        //
        if (currentByte == '\n') {
          break;
        }
        ++i;
      }
      else if (time > 0 && millis () - time > 3000) {
        //
        // if we get here that usually means we received an invalid command.
        // however, if the user does not send newlines at the end of a
        // command, we still want to continue execution after 3 seconds.
        //
        s[0]->print ("continuing after timeout. possibly missing newline at EOL?\n");
        break;
      }
    }
    //
    // when there's a buffer overflow, the input buffer might be empty although
    // there's still more data on the way
    //
    if (flagged)
      delay (200);

    if (s[0]->available ()) {
      //
      // we get here when there's still data in the pipeline but we already
      // exited the read loop. this happens when there's a newline character
      // in the command string before EOL or when we receive more than 2048
      // bytes. We flush the pipeline and flag the command to prevent execution.
      //
      while (s[0]->available () > 0) {
        s[0]->read ();
      }

      if (flagged) {
        s[0]->print ("buffer overflow. the command size limit is 2048 bytes\n");
      }
      else {
        s[0]->print ("malformed command\n");
      }

      flagged = true;
    }
    if (debug) {
      s[0]->print ("received command: ");
      s[0]->print (arg);
    }
    if (!flagged) {
      if (strstr (arg, "h ()") == &arg[0]) {
        help ();
      }
      else if (strstr (arg, "c (") == &arg[0]) {
        s[1]->end ();
        s[2]->end ();
        s[0]->print ("re-");
        configurePorts (arg, i);
      }
      else if (strstr (arg, "1 (") == &arg[0]) {
        memcpy (send, arg + 3, i - 4);
        s[0]->print ("sending to device 1: ");
        s[0]->print (send);
        s[1]->write (send);
        s[0]->println ();
      }
      else if (strstr (arg, "2 (") == &arg[0]) {
        memcpy (send, arg + 3, i - 4);
        s[0]->print ("sending to device 2: ");
        s[0]->print (send);
        s[2]->write (send);
        s[0]->println ();
      }
      else if (strstr (arg, "debug ()") == &arg[0]) {
        if (!debug) {
          debug = true;
          s[0]->print ("debug mode enabled\n");
        }
        else {
          debug = false;
          s[0]->print ("debug mode disabled\n");
        }
      }
      else if (strstr (arg, "reset ()") == &arg[0]) {
        softReset ();
      }
      else {
        s[0]->print ("invalid command\n");
      }
    }
  }
  if (s[1]->available () > 0) {
    char currentByte = '\0';

    if (secondDevice) {
      secondDevice = false;
      s[0]->print ("\n1: ");
    }
    if (firstMessage) {
      firstMessage = false;
      s[0]->print ("\n1: ");
    }

    for (int i = 1; i <= s[1]->available (); ++i) {
      currentByte = s[1]->read ();
      s[2]->write (currentByte);
      s[0]->write (currentByte);
    }
  }
  if (s[2]->available () > 0) {
    char currentByte = '\0';

    if (!secondDevice) {
      secondDevice = true;
      s[0]->print ("\n2: ");
    }

    for (int i = 1; i <= s[2]->available (); ++i) {
      currentByte = s[2]->read ();
      s[1]->write (currentByte);
      s[0]->write (currentByte);
    }
  }
}
