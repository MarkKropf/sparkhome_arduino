#include <SPI.h>
#include <Ethernet.h>
#include <ArdOSC.h>

byte myMac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 
  10, 0, 0, 245 };
int  serverPort  = 10000;

// Defines for use with Arduino functions
#define clockpin   4 // CI
#define enablepin  8 // EI
#define latchpin    7 // LI
#define datapin    2 // DI

// Defines for direct port access.
#define CLKPORT PORTD
#define ENAPORT PORTB
#define LATPORT PORTD
#define DATPORT PORTD
#define CLKPIN  4
#define ENAPIN  0
#define LATPIN  7
#define DATPIN  2

// Variables for communication
unsigned long SB_CommandPacket;
int SB_CommandMode;
int SB_BlueCommand;
int SB_RedCommand;
int SB_GreenCommand;

// Define number of ShiftBrite modules
#define NumLEDs 40
#define NumColumns 5
#define NumRows 8

// Create LED value storage array
int LEDChannels[NumLEDs][3] = {
  0};
// Create Colorize value storage array
int LEDColorize[NumRows][3] = {
  0};

//Octobar Settings
int redvalue = 0;
int greenvalue = 0;
int bluevalue = 0;
int effect = 1;
int reverse = 0;
int old_r = 0;
int old_g = 0;
int old_b = 0;
long previousMillis = 0;
long refreshMillis = 0;
int refresh_val = 250;
int delay_val = 250;
int fadestep = 0;
int pulsestep = 0;
int directional = 1;
int filled = 0;
float dimmerval = 0.4;
float olddimmerval = 0;
float boostval = 1;
int blackoutval = 0;
int beatval = 0;
int strobeval = 0;
int colorizeval = 0;
int colorset = 0;

// OSC Server Setup
OSCServer server;

// Set pins to outputs and initial states
void setup() {
  //Serial.begin(19200);
  pinMode(datapin, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(enablepin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  digitalWrite(latchpin, LOW);
  digitalWrite(enablepin, LOW);
  Ethernet.begin(myMac ,myIp); 
  server.begin(serverPort);
  server.addCallback("/octobar/fader_refresh",&fader_refresh);
  server.addCallback("/octobar/fader_delay",&fader_delay);
  server.addCallback("/octobar/fader_red",&fader_red);
  server.addCallback("/octobar/fader_green",&fader_green);
  server.addCallback("/octobar/fader_blue",&fader_blue);
  server.addCallback("/octobar/push_red",&push_red);
  server.addCallback("/octobar/push_green",&push_green);
  server.addCallback("/octobar/push_blue",&push_blue);
  server.addCallback("/octobar/push_yellow",&push_yellow);
  server.addCallback("/octobar/push_purple",&push_purple);
  server.addCallback("/octobar/push_white",&push_white);
  server.addCallback("/octobar/push_natural",&push_natural);
  server.addCallback("/octobar/color_select",&color_select);
  server.addCallback("/octobar/pulse",&pulse);
  server.addCallback("/octobar/fade",&fade);
  server.addCallback("/octobar/randoms",&randoms);
  server.addCallback("/octobar/strobe",&strobe);
  server.addCallback("/octobar/forward",&forward);
  server.addCallback("/octobar/backward",&backward);
  server.addCallback("/octobar/left",&left);
  server.addCallback("/octobar/right",&right);
  server.addCallback("/octobar/fill",&fill);
  server.addCallback("/octobar/dimmer",&dimmer);
  server.addCallback("/octobar/blackout",&blackout);
  server.addCallback("/octobar/push_beat",&push_beat);
  server.addCallback("/octobar/boost",&boost);
  server.addCallback("/octobar/colorize",&colorize);
  randomSeed(analogRead(0));
  //Serial.println("Setup Complete");
}

void loop() {
  server.aviableCheck();
  unsigned long currentMillis = millis();
  if (beatval == 1) {
    olddimmerval = dimmerval;
    dimmerval = boostval;
    beatval = 2;
  }
  if (blackoutval == 1) {
    for (int i = 0; i < NumLEDs; i++) {
      LEDChannels[i][0] = 0;
      LEDChannels[i][1] = 0;
      LEDChannels[i][2] = 0;
    }
    WriteLEDArray();
  } 
  else if (effect == 1) {
    if(currentMillis - previousMillis > delay_val) {
      previousMillis = currentMillis;
      for (int i = 0; i < NumLEDs; i++) {
        LEDChannels[i][0] = redvalue*dimmerval;
        LEDChannels[i][1] = greenvalue*dimmerval;
        LEDChannels[i][2] = bluevalue*dimmerval;
      }
      WriteLEDArray();
      if (beatval == 2 ) { 
        beatval=3; 
      };
    }
  } 
  else if (effect == 2) {
    // Pulse Effect
    if(currentMillis - refreshMillis > refresh_val) {
      if(currentMillis - previousMillis > delay_val) {
        previousMillis = currentMillis;
        if ((directional < 3 && pulsestep > 8) | (directional > 2 && pulsestep > 5)) {
          pulsestep = 0;
          colorset = 0;
        }
        if (colorizeval == 1 && colorset == 0) {
          for (int j2 = 0; j2 < 8; j2++) {
            LEDColorize[j2][1] = random(1, 1023);
            LEDColorize[j2][2] = random(1, 1023);
            LEDColorize[j2][3] = random(1, 1023);
          }
          colorset = 1;
        } 
        // Normal Colors
        if (colorizeval == 0) {
        for (int j1 = 0; j1 < NumLEDs; j1++) {
          int column = (j1+8) / 8;
          int row = j1 - ((column - 1) * 8);
          if(directional == 1) {
            // Forwards
            if(row <= pulsestep && pulsestep < 8 && filled==1) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else if (row == pulsestep && pulsestep < 8 && filled==0) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          } 
          else if (directional == 2) {
            // Backwards
            if(row >= 7 - pulsestep && pulsestep < 8 && filled==1) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else if(row == 7 - pulsestep && pulsestep < 8 && filled==0) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          } 
          else if (directional == 3) {
            // Left
            if(column-1 >= 4 - pulsestep && pulsestep < 5 && filled==1) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else if(column-1 == 4 - pulsestep && pulsestep < 5 && filled==0) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          } 
          else if (directional == 4) {
            // Right
            if(column-1 <= pulsestep && pulsestep < 5 && filled==1) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else if(column-1 == pulsestep && pulsestep < 5 && filled==0) {
              LEDChannels[j1][0] = redvalue*dimmerval;
              LEDChannels[j1][1] = greenvalue*dimmerval;
              LEDChannels[j1][2] = bluevalue*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          }
        }
        } else {
        // Color Wheel Shit
        for (int j1 = 0; j1 < NumLEDs; j1++) {
          int column = (j1+8) / 8;
          int row = j1 - ((column - 1) * 8);
          if(directional == 1) {
            // Forwards
            if(row <= pulsestep && pulsestep < 8 && filled==1) {
              LEDChannels[j1][0] = LEDColorize[row][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[row][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[row][3]*dimmerval;
            } 
            else if (row == pulsestep && pulsestep < 8 && filled==0) {
              LEDChannels[j1][0] = LEDColorize[row][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[row][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[row][3]*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          } 
          else if (directional == 2) {
            // Backwards
            if(row >= 7 - pulsestep && pulsestep < 8 && filled==1) {
              LEDChannels[j1][0] = LEDColorize[row][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[row][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[row][3]*dimmerval;
            } 
            else if(row == 7 - pulsestep && pulsestep < 8 && filled==0) {
              LEDChannels[j1][0] = LEDColorize[row][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[row][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[row][3]*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          } 
          else if (directional == 3) {
            // Left
            if(column-1 >= 4 - pulsestep && pulsestep < 5 && filled==1) {
              LEDChannels[j1][0] = LEDColorize[column][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[column][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[column][3]*dimmerval;
            } 
            else if(column-1 == 4 - pulsestep && pulsestep < 5 && filled==0) {
              LEDChannels[j1][0] = LEDColorize[column][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[column][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[column][3]*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          } 
          else if (directional == 4) {
            // Right
            if(column-1 <= pulsestep && pulsestep < 5 && filled==1) {
              LEDChannels[j1][0] = LEDColorize[column][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[column][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[column][3]*dimmerval;
            } 
            else if(column-1 == pulsestep && pulsestep < 5 && filled==0) {
              LEDChannels[j1][0] = LEDColorize[column][1]*dimmerval;
              LEDChannels[j1][1] = LEDColorize[column][2]*dimmerval;
              LEDChannels[j1][2] = LEDColorize[column][3]*dimmerval;
            } 
            else {
              LEDChannels[j1][0] = 0;
              LEDChannels[j1][1] = 0;
              LEDChannels[j1][2] = 0;
            }
          }
        }
        
        }
        WriteLEDArray();
        if (beatval == 2 ) { 
          beatval=3; 
        };
        if ((directional < 3 && pulsestep == 8) | (directional > 2 && pulsestep == 5)) {
          refreshMillis = currentMillis;
        }
        pulsestep++;  
      }
    }
  } 
  else if (effect == 3) {
    // Fade Effect
    if(currentMillis - previousMillis > delay_val) {
      previousMillis = currentMillis; 
      if (redvalue != LEDChannels[0][0] | greenvalue != LEDChannels[0][1] | bluevalue != LEDChannels[0][2]) {
        if (fadestep < 33) {
          for (int j1 = 0; j1 < NumLEDs; j1++) {
            LEDChannels[j1][0] = (old_r * (32 - fadestep) + redvalue * fadestep)/32;
            LEDChannels[j1][1] = (old_g * (32 - fadestep) + greenvalue * fadestep)/32;
            LEDChannels[j1][2] = (old_b * (32 - fadestep) + bluevalue * fadestep)/32;
          }
          fadestep++;
        }
        WriteLEDArray();
      }
    }   
  } 
  else if (effect == 4) {
    // Random Effect
    if(currentMillis - previousMillis > delay_val) {
      previousMillis = currentMillis; 
      for (int i = 0; i < NumLEDs; i++) {
        LEDChannels[i][0] = random(1, 1023)*dimmerval;
        LEDChannels[i][1] = random(1, 1023)*dimmerval;
        LEDChannels[i][2] = random(1, 1023)*dimmerval;
      }
      WriteLEDArray();
      if (beatval == 2 ) { 
        beatval=3; 
      };
    }
  } 
  else if (effect == 5) {
    // Strobe Effect
    if(currentMillis - previousMillis > delay_val / 4) {
      previousMillis = currentMillis; 
      if (strobeval == 0) {
        for (int i = 0; i < NumLEDs; i++) {
          LEDChannels[i][0] = redvalue*dimmerval;
          LEDChannels[i][1] = greenvalue*dimmerval;
          LEDChannels[i][2] = bluevalue*dimmerval;
        }
        strobeval = 1;
      } 
      else {
        for (int i = 0; i < NumLEDs; i++) {
          LEDChannels[i][0] = 0;
          LEDChannels[i][1] = 0;
          LEDChannels[i][2] = 0;
        }
        strobeval = 0;
      }
      WriteLEDArray();
      if (beatval == 2 ) { 
        beatval=3; 
      };
    }
  }
  if (beatval == 3) {
    dimmerval = olddimmerval;
    beatval = 0;
  }
}

void old_color() {
  old_r = LEDChannels[0][0];
  old_g = LEDChannels[0][1];
  old_b = LEDChannels[0][2];
  fadestep = 0;
}

void push_red(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) {
    old_color();
    redvalue = 1023;
    greenvalue = 0;
    bluevalue = 0;
  }
}

void push_green(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) {
    old_color();
    redvalue = 0;
    greenvalue = 1023;
    bluevalue = 0;
  }
}

void push_blue(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) {
    old_color();
    redvalue = 0;
    greenvalue = 0;
    bluevalue = 1023;
  }
}

void push_yellow(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) {
    old_color();
    redvalue = 1023;
    greenvalue = 1023;
    bluevalue = 0;
  }
}

void push_purple(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) {
    old_color();
    redvalue = 1023;
    greenvalue = 0;
    bluevalue = 1023;
  }
}

void push_white(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) {
    old_color();
    redvalue = 1023;
    greenvalue = 1023;
    bluevalue = 1023;
  }
}

void push_natural(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) {
    old_color();
    redvalue = 1023;
    greenvalue = 700;
    bluevalue = 110;
  }
}

void fader_refresh(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  refresh_val = tmpF*1000;
}

void fader_delay(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  delay_val = tmpF*1000;
}

void fader_red(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  redvalue = tmpF*1023;
}

void fader_green(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  greenvalue = tmpF*1023;
}

void fader_blue(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  bluevalue = tmpF*1023;
}

void color_select(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    effect = 1; 
  }
}

void pulse(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    effect = 2; 
  }
}

void fade(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    effect = 3; 
  }
}

void randoms(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    effect = 4; 
  }
}
void strobe(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    effect = 5; 
  }
}
void forward(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    directional = 1; 
  }
}
void backward(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    directional = 2; 
  }
}
void left(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    directional = 3; 
  }
}
void right(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    directional = 4; 
  }
}
void fill(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    filled = 1;
  } 
  else { 
    filled = 0;
  }
}
void dimmer(OSCMessage *_mes) {
  // Master Dimmer Control
  float tmpF=_mes->getArgFloat(0);
  dimmerval = tmpF;
}
void boost(OSCMessage *_mes) {
  // Master Dimmer Control
  float tmpF=_mes->getArgFloat(0);
  boostval = tmpF;
}
void blackout(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    blackoutval = 1; 
  } 
  else { 
    blackoutval = 0; 
  }
}
void push_beat(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    beatval = 1; 
  }
}
void colorize(OSCMessage *_mes) {
  float tmpF=_mes->getArgFloat(0);
  if (tmpF) { 
    colorizeval = 1; 
  } 
  else { 
    colorizeval = 0; 
  }
}
void SB_SendPacket() {

  if (SB_CommandMode == B01) {
    SB_RedCommand = 127;
    SB_GreenCommand = 110;
    SB_BlueCommand = 110;
  }

  SB_CommandPacket = SB_CommandMode & B11;
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_BlueCommand & 1023);
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_RedCommand & 1023);
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_GreenCommand & 1023);

  for (int j = 0; j < 32; j++) {
    if ((SB_CommandPacket >> (31 - j)) & 1) {
      DATPORT |= (1 << DATPIN);
    } 
    else {
      DATPORT &= ~(1 << DATPIN);
    }
    CLKPORT |= (1 << CLKPIN);
    CLKPORT &= ~(1 << CLKPIN); 
  } 
}

void SB_Latch() {

  delayMicroseconds(1);
  LATPORT |= (1 << LATPIN);
  ENAPORT |= (1 << ENAPIN);
  delayMicroseconds(1);
  ENAPORT &= ~(1 << ENAPIN);
  LATPORT &= ~(1 << LATPIN);

}

void WriteLEDArray() {

  SB_CommandMode = B00; // Write to PWM control registers

  for (int i = 0; i < NumLEDs; i++) {
    SB_RedCommand = LEDChannels[i][0];
    SB_GreenCommand = LEDChannels[i][1];
    SB_BlueCommand = LEDChannels[i][2];
    SB_SendPacket();
  }

  SB_Latch();

  SB_CommandMode = B01; // Write to current control registers

  for (int z = 0; z < NumLEDs; z++) SB_SendPacket();   

  SB_Latch();

}


