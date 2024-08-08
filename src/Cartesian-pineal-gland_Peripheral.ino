/* 
  Cartesian pineal gland project
  Copyright (c) 2024 Sangbong Lee <sangbong@me.com>
  
  * This installation has two different devices. the first one is an LCD module, and the second is a magnetic memory module.
  * First, the LCD / ASCII data controller converts a character into an 8-bit binary and transmits it to the memory core module. 
  * The code for the display module allows the Arduino to display text on the screen and it converts each character into ASCII one by one and sends it to the memory module. 
  * Second, the magnetic memory module operates L293 to apply current to the wire passing through the ferrite core to store the binary data from the controller.
  * Also, when the data is stored in the core, the induced voltage spike triggers an electromagnet and makes its magnetism generate laser diffraction.

  This work is licensed under the Creative Commons Attribution 4.0 International License.
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/.
*/

// Library
#include <Wire.h>

// Relay signal pin
const int RELAY_PIN = 4;  //D4

// L293D controll pins
const int EN_L293_PIN = 10;
const int IN1_PIN = 9; //D8
const int IN2_PIN = 8; //D9

// LM393 output receive pins
const int LM_OUTPUT_RECEIVE_PIN_1 = 3; //D3 //LM_OUTPUT_1_RECEIVE_PIN || 'BIT 1'
const int LM_OUTPUT_RECEIVE_PIN_2 = 2; //D2 //LM_OUTPUT_2_RECEIVE_PIN || 'BIT 0'

// Relay operation flag
boolean relay_finished = false;

// LM393 Pulse detection flag
boolean bit_write = false;

// Peripheral addresses
int address = 6; // 1,2,3,4,5,6,7

// I2C communication
String statusString;
byte receivedCharbyte;
byte command_byte;

void setup() {
  // I2C Communication setup:
  I2C_Initialize();

  // Set relay pins and disable it
  Relay_PinSetup();

  // Set L293D controll pins
  L293_PinSetup();

  // Set LM393 inout pins
  LM393_PinSetup();

  // Memory core initialize;
  CORE_Initialize();

  //Attach interrupt routines for LM393 Output
  //attachInterrupt(digitalPinToInterrupt(LM_OUTPUT_RECEIVE_PIN_1), LM393_Bit_1_Pulse_Check, RISING);
  //attachInterrupt(digitalPinToInterrupt(LM_OUTPUT_RECEIVE_PIN_2), LM393_Bit_0_Pulse_Check, RISING);
}

void loop() {
  switch (command_byte) {
    case 1:
      // Function 1 || Bit '0'
      // APPLY CURRENT D1_L -> D2_H || Write '0' || bit_write = false;
      L293_ApplyCurrent_D1toD2();
      L293_StopCurrent();

      // clear command
      command_byte = 0;
      break;

    case 2:
      // Function 2 || Bit '1'
      // APPLY CURRENT D1_H -> D2_L || Write '1' || bit_write = true;
      L293_ApplyCurrent_D2toD1();
      L293_StopCurrent();

      // clear command
      command_byte = 0;
      break;

    default:
      // Do nothing
      break;
  }
  relay_ON_OFF(bit_write);
}

////////======= Pin Setup_Functions =======////////

void Relay_PinSetup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}

void L293_PinSetup() {
  //Set L293D controll pins
  pinMode(EN_L293_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
}

void LM393_PinSetup() {
  //Set LM393 inout pins
  pinMode(LM_OUTPUT_RECEIVE_PIN_1, INPUT);
  pinMode(LM_OUTPUT_RECEIVE_PIN_2, INPUT);
}

void CORE_Initialize() {
  //Write '0' into the core
  L293_ApplyCurrent_D1toD2();
  L293_StopCurrent();
}

////////======= Relay_Control_Functions =======////////

void relay_ON_OFF (boolean _bitFlag) {
  //Relay ON or OFF
  if (!_bitFlag) {
    digitalWrite(RELAY_PIN, LOW);
  }
  else {
    digitalWrite(RELAY_PIN, HIGH);
  }
  relay_finished = true;
}

////////======= Core_Memory(L293D & LM393)_Functions =======////////

void L293_ApplyCurrent_D1toD2() {
  // Reset core bit flag
  // bit_write = false;

  noInterrupts();
  digitalWrite(IN1_PIN, LOW); // IN1 LOW
  digitalWrite(IN2_PIN, HIGH);// IN2 HIGH
  analogWrite(EN_L293_PIN, 255);;
  interrupts();
}

void L293_ApplyCurrent_D2toD1() {
  // Reset core bit flag
  // bit_write = false;

  noInterrupts();
  digitalWrite(IN1_PIN, HIGH); // IN1 HIGH
  digitalWrite(IN2_PIN, LOW);  // IN2 LOW
  analogWrite(EN_L293_PIN, 255);
  interrupts();
}

void L293_StopCurrent() {
  // Applying 0 current || PWM value = 0
  noInterrupts();
  analogWrite(EN_L293_PIN, 0);
  interrupts();
}

void LM393_Bit_0_Pulse_Check() {
  // Interrupt function
  // Bit '0' 
  bit_write = false;
}

void LM393_Bit_1_Pulse_Check() {
  // Interrupt function
  // Bit '1'
  bit_write = true;
}

void Core_Mag_BitCheck() {

}
////////======= I2C_Communication_Functions =======////////

void I2C_Initialize() {
  Wire.begin(address);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent); // register event
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    receivedCharbyte = Wire.read();

    //Set command bit based on the received byte
    getCommand(receivedCharbyte);
  }
}

void requestEvent() {
  statusString = String(address); // + 1
  statusString += ','; // + 1
  statusString += char(receivedCharbyte); // + 1
  statusString += ','; // + 1
  statusString += String(bit_write); // + 1
  statusString += ','; // + 1
  if (relay_finished) {
    statusString += 'A'; // + 1
  } else {
    statusString += 'F';
  }
  // total num 7

  for (int i = 0; i < statusString.length(); i++) {
    Wire.write(statusString.charAt(i)); // respond with message of 6 bytes
  }
  relay_finished = false;
}

void getCommand (byte _receivedByte) {
  if (_receivedByte == '0') {
    command_byte = 1;
    LM393_Bit_0_Pulse_Check();
  } 
  else if (_receivedByte == '1') {
    command_byte = 2;
    LM393_Bit_1_Pulse_Check();
  } 
  else {
    command_byte = 0;
    bit_write = false;
  }
}


////////======= H-Bridge(L298N)_Control_Functions =======////////
void H_Bridge_Control() {

}

//Gradually increase magnetism
void H_Bridge_On(int _value) {

}

//Gradually decrease magnetism
void H_Bridge_Off(int _value) {

}
