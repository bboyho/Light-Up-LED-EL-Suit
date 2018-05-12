/*******************************************************************
  XBee_ELSequencer_ControllerV3.ino
  Created by Ho Yun Bobby Chan @  SparkFun Electronics May 12th, 2017

  Taken from SparkFun XBee EL Sequencer Controller
  Ho Yun Bobby Chan @ SparkFun Electronics June 20, 2014
  Updated by Toni Klopfenstein @ SparkFun Electronics April, 2015
  https://github.com/sparkfun/EL_Sequencer

  Description:
  This is a sketch for the wireless controller used to control 7x EL dance suits.
  The wireless controller consists of a RedBoard Programmed with Arduino,
  XBee Shield, XBee Series 1 transceiver, diffused RGB Common Cathode LED,
  Blue Clear LED, 330Ohm current limiting resistors, a button, 9V battery,
  and a 9V adapter.

  Each of the 7x EL dance suits contain an EL Sequencer, 2x EL Wires, a
  12V EL inverter, XBee female sockets soldered, a 9V battery, 9V adapter,
  and a XBee Series 1 transceiver.  An XBee Series 2 can be used but the throughput
  of the Series 1 is much higher. To reduce latency, I recommend using the XBee
  Series 1. The basic configuration of the XBee module with point-to-point configuratin is
  based on Digi's Example tutorial => https://www.digi.com/blog/xbee/basic-xbee-802-15-4-chat/.
  page 5 of the tutorial shows you how to broadcast with
  point-to-multipoint configuration so that multiple EL Sequencers
  can be controlled.

  Connect the XBee Shield to the Arduino with the switch flipped to the
  software serial side labeled "DLINE".

  By pushing the button, a character is sent from a remote microcontroller.
  The corresponding EL Sequencer will receive the character and control
  the EL component on a channel that is associated with that character.

  Using a RedBoard programmed with Arduino, the XBee transceiver is connected to the
  Software Serial pins. By pushing the button, the Arduino will
  send one character through the XBee. Logic is used to control
  how many characters are sent with the push button. The Arduino
  will not send another character until the button is pressed again.

  Note: This section of the code can be optimized. As the user is not
  pressing down on the button, logic can be added so that the XBee is
  not continuously sending serial characters to the receiving
  EL Sequencer when idle.

  Development environment specifics:
  Arduino 1.6.5

  This code is beerware; if you see me (or any other SparkFun employee) at the local,
  and you've found our code helpful, please buy us a round!
  Distributed as-is; no warranty is given.
**************************************************************************/

#include <SoftwareSerial.h>

SoftwareSerial xbee(2, 3); //Rx = 2, Tx = 3

//Declare variables and pin definitions

char send_CHAR = 'A'; //default send character

int pattern = 0; //pattern that we are going to send

//SEND Button
const int button1Pin = 4; //push button
const int ledPin1 = 13;  //LED to indicate when a character has been sent
//variables to check for button1 state
boolean prev_button1State = false;
boolean current_button1State = false;

//LED Status Indicator
int ledR = 5; //hardware PWM
int ledG = 6; //hardware PWM
int ledB = 9; //hardware PWM

//UP Button
const int button2Pin = 11; //push button to move ahead to next sequence
//variables to check for button2 state
boolean prev_button2State = false;
boolean current_button2State = false;

//DOWN Button
const int button3Pin = 12;//push button to move back a sequence
//variables to check for button3 state
boolean prev_button3State = false;
boolean current_button3State = false;

/*******************Setup Loop***************************/
void setup() {
  //Declare buttons and status LEDs

  pinMode (ledPin1, OUTPUT); //send LED
  pinMode(button1Pin, INPUT_PULLUP); //SEND button, use internal pullup resistor

  // initialize the digital pins as an output for status LED
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(button2Pin, INPUT_PULLUP); //UP button, use internal pullup resistor
  pinMode(button3Pin, INPUT_PULLUP); //DOWN button, use internal pullup resistor

  //Declare serial connections for debugging
  Serial.begin(9600);
  Serial.println("Arduino started sending bytes via XBee");

  //Declare software serial conenction with XBee
  xbee.begin(9600);
  Serial.println("EL Sequencer Controller's XBee Ready to Communicate");

  sequenceTest();//visually initialization to see that we have finished initializing
}

/*******************Main Loop***************************/
void loop() {
  //initialize variables to read buttons
  int button1State;
  int button2State;
  int button3State;

  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  /*buttonXstate
    - LOW or 0 means pressed
    - HIGH or 1 means not pressed
  */

  //-----------Check If SENT Button Has Been Pressed----------
  //if SENT button is pressed, it will be pulled low
  if (button1State == LOW) {
    digitalWrite(ledPin1, HIGH); //turn LED indicating if a character has been sent ON
    current_button1State = true; // button has been pressed once

    if (prev_button1State != current_button1State) //check to see if button is still being pressed
    {
      Serial.println("Button is pressed.");
      xbee.write(send_CHAR);//Tell Sequencer to change mode
    }
    else {
      //do nothing because finger is still on button
    }
    prev_button1State = current_button1State;
  }

  //sent button has not been pressed, it will be high again
  else {
    current_button1State = false;
    digitalWrite(ledPin1, LOW); // turn push button LED OFF

    prev_button1State = current_button1State;
  }//-----------End Check for SENT Button----------

  //-----------Check If UP Button Has Been Pressed----------
  if (button2State == LOW) {
    current_button2State = true; //UP button has been pressed once

    if (prev_button2State != current_button2State) { //check to see if button is still being pressed
      pattern = pattern + 1; //change LED pattern after button has been pressed
      if (pattern < 0 || pattern > 7) {
        //reset pattern
        pattern = 0;
      }

    }
    else { //do nothing because finger is still on button
    }
    prev_button2State = current_button2State;
  }
  //UP button has not been pressed, it will be high
  else {
    current_button2State = false;
    prev_button2State = current_button2State;
  }//-----------End Check for Up Button----------

  //-----------Check If DOWN Button Has Been Pressed----------
  if (button3State == LOW) {
    current_button3State = true; //button has been pressed once

    if (prev_button3State != current_button3State) { //check to see if button is still being pressed
      pattern = pattern - 1; //change LED pattern after button has been pressed
      if (pattern < 0 || pattern > 7) {
        //reset pattern
        pattern = 6;
      }
    }
    else { //do nothing because finger is still on button
    }
    prev_button3State = current_button3State;
  }
  //button has not been pressed, it will be high
  else {
    current_button3State = false;
    prev_button3State = current_button3State;
  }//-----------End Check for DOWN Button----------

  delay(50);

  //save send character into variable depending on button press and change status LED
  switch (pattern) {
    case 1:
      allOFF();//blink status LED
      delay(50);
      blueON();
      delay(50);
      send_CHAR = 'B';
      break;
    case 2:
      magentaON();
      send_CHAR = 'C';
      break;
    case 3:
      yellowON();
      send_CHAR = 'D';
      break;
    case 4:
      clearblueON();
      send_CHAR = 'E';
      break;
    case 5:
      blueON();
      send_CHAR = 'F';
      break;
    case 6:
      redON();
      send_CHAR = 'G';
      break;
    default:
      whiteON();
      send_CHAR = 'A';
      break;
  }

}//end loop


/*Below are the modular functions for changing the color of a RGB LED.
  This will be used to help identify what mode we are currently in:
  ROYGBIV
  Note: A 9V battery is not able to fully power all three LEDs simultaneously...
  MODE
  1.) red              = red[HIGH]
  .) tangerine orange = red[HIGH]+ green[50]
  2.) yellow           = red[HIGH]+ green[HIGH]
  3.) green            =          + green[HIGH]
  4.) clear blue       =          + green[HIGH] + blue[HIGH]
  5.) blue             =                        + blue[HIGH]
  6.) magenta          = red[HIGH]+             + blue[HIGH]
  .) white            = red[HIGH]+ green[HIGH] + blue[HIGH]
*/
void whiteON() {
  analogWrite(ledR, 150);
  analogWrite(ledG, 255);
  analogWrite(ledB, 255);
}

void allOFF() {
  analogWrite(ledR, 0);
  analogWrite(ledG, 0);
  analogWrite(ledB, 0);
}

void redON() { //Seq 1
  analogWrite(ledR, 255);
  analogWrite(ledG, 0);
  analogWrite(ledB, 0);
}

void greenON() { //Seq 2
  analogWrite(ledR, 0);
  analogWrite(ledG, 255);
  analogWrite(ledB, 0);
}

void blueON() { //Seq 3
  analogWrite(ledR, 0);
  analogWrite(ledG, 0);
  analogWrite(ledB, 255);
}

void yellowON() {
  analogWrite(ledR, 150);
  analogWrite(ledG, 200);
  analogWrite(ledB, 0);
}

void clearblueON() {
  analogWrite(ledR, 0);
  analogWrite(ledG, 150);
  analogWrite(ledB, 255);
}

void magentaON() {
  analogWrite(ledR, 150);
  analogWrite(ledG, 0);
  analogWrite(ledB, 255);
}

void sequenceTest() {
  redON();
  delay(50);
  allOFF();
  delay(50);

  yellowON();
  delay(50);
  allOFF();
  delay(50);

  greenON();
  delay(50);
  allOFF();
  delay(50);

  clearblueON();
  delay(50);
  allOFF();
  delay(50);

  blueON();
  delay(50);
  allOFF();
  delay(50);

  magentaON();
  delay(50);
  allOFF();
  delay(50);

  //whiteON();//white drains too much power from a 9V, commenting out.
  //delay(50);
  //allOFF();
  //delay(50);

}
