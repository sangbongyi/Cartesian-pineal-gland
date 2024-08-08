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

// Libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//================ LCD variable ================//
// LCD numbers of column and row
const int lcdNum = 3;
const int lcdRowNum = 20;
const int lcdColNum = 4;
const int lcdColNum_total = 12;

// LCD Display Address setup
const int i2c_display1_addr = 0x27;
const int i2c_display2_addr = 0x26;
const int i2c_display3_addr = 0x25;

// LCD Constructors setup
LiquidCrystal_I2C lcd1(i2c_display1_addr, lcdRowNum, lcdColNum);
LiquidCrystal_I2C lcd2(i2c_display2_addr, lcdRowNum, lcdColNum);
LiquidCrystal_I2C lcd3(i2c_display3_addr, lcdRowNum, lcdColNum);
LiquidCrystal_I2C *lcds[3] = {&lcd1, &lcd2, &lcd3};

//================ LCD TEXT variables ================//
const int totalTextArrayLength = 32;

// Create a set of new characters //Limited 0 - 7
const uint8_t charBitmap[][8] = {
  { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}
};

// Hexadecimal number to binary number
char *hexToBin[] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
                    "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
                   };

// Text array
char *myText[] = {"My view is that this", "gland is the princi-", "pal seat of the soul", ", and the place in  ",
                  "which all our thoug-", "hts are formed.     ", "The reason I believe", "this is that I can't",
                  "find any part of the", "brain, except this, ", "which is not double.", " Since we see only  ", //First page
                  "one thing with two  ", "eyes, and hear only ", "one voice with two  ", "ears, and in short  ",
                  "have never more than", "one thought at times", " It must necessarily", "be the case that the",
                  "impressions which   ", "enter by the two eye", "or by the two ears, ", "and so on, unite wi-", //Second page
                  "th each other in    ", "some part of the bo-", "dy before being con-", "sidered by the soul.",
                  "                    ", "                    ", "                    ", "                    ",
                  "                    ", "                    ", "                    ", "                    "  //Third page
                 }; // 32 Lines of texts

// Text array to display during the documentation (Operate LM293 memory core as many number as possible at the same time)
char *myText_Docu[] = {"w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ",
                       "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ",
                       "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", //First page
                       "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ",
                       "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ",
                       "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", //Second page
                       "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ", "w6o_0kZg?~m>t7$/Y!U ",
                       "                    ", "                    ", "                    ", "                    ",
                       "                    ", "                    ", "                    ", "                    "  //Third page
                      }; // 32 Lines of texts

// LCD Row array
String row_array[12];
String row_array_Docu[12];

//================ Peripheral variables ================//
// Peripheral Adresses setup
const int EM_Controller_1_addr = 1;
const int EM_Controller_2_addr = 2;
const int EM_Controller_3_addr = 3;
const int EM_Controller_4_addr = 4;
const int EM_Controller_5_addr = 5;
const int EM_Controller_6_addr = 6;
const int EM_Controller_7_addr = 7;
const int Num_EM_Controller = 7;

// Pheripheral address array
const int EM_Controllers_addr[7] = {EM_Controller_1_addr,
                                    EM_Controller_2_addr,
                                    EM_Controller_3_addr,
                                    EM_Controller_4_addr,
                                    EM_Controller_5_addr,
                                    EM_Controller_6_addr,
                                    EM_Controller_7_addr
                                   };

//================ Blinking letter control variables ================//
// Number of blinks and the duration of each iteration
int blinkingNum = 25; //17 / 20
int blink_Duration = 300; //300

// Index counters for the position of the blinking letters.
int blinkCounterX = 0;
int blinkCounterY = 0;

// Counter index for every letters in the text
int textCounter = 0;

// When the blinking block reaches at the end of currently displyed page, set the flag
boolean textReload = false;

// Number of page
int pageNumber = 0;

//================ I2C communication variables ================//
// Data for sending and receiving
byte ascii_Num;
String messages_From_peri;

// I2C connection flag
boolean I2C_Initializaing = false;
boolean I2C_Handshaking = false;

// I2C peripheral variables (total number of peripherals and adresses of each peripheral)
int number_devices = 0;
byte devices_i2c_add[10];

//================ Elapsing time check Variables ================//
// Running time
unsigned long RUN_StartTime;
unsigned long RUN_CurrentTime;
unsigned long RUN_ElapsedTime;
unsigned long RUN_Duration = 600000; //300 Seconds 300000


// Sleeping time
unsigned long SLEEP_StartTime;
unsigned long SLEEP_CurrentTime;
unsigned long SLEEP_ElapsedTime;
unsigned long SLEEP_Duration = 600000; //300 Seconds 300000

// Run or Sleep check flags
boolean running_mode = false;

void setup() {
  // Serial setup
  Serial.begin(115200);
  Serial.println("ASCII CONVERT");

  // I2C devices setup (Displays and peripherals)
  Wire.begin();
  while (!I2C_Initializaing) {
    I2C_Initialize();
  }

  // Initialize LCDs
  LCD_Setup();

  // Elapsed time checking
  RUN_StartTime = millis();
  SLEEP_StartTime = millis();

  // Set running mode
  running_mode = true;
}

void loop() {
  // Running mode
  LCD_Refresh();
  LCD_Display();
  LCD_Reset();

  // new start
  textReload = false;
}


////////======= Wake up & Sleep Functions =======////////
// Check elapsed running time
void check_runtime() {
  RUN_CurrentTime = millis();
  RUN_ElapsedTime = RUN_CurrentTime - RUN_StartTime;

  if (RUN_ElapsedTime > RUN_Duration) {
    Serial.println("!!!!COOLING MODE!!!!");
    Serial.println("!!!!COOLING MODE!!!!");

    running_mode = false;
    SLEEP_StartTime = millis();

    //Before engage the Sleep mode
    LCD_Refresh();
  }
}

// Check elapsed sleep mode time
void check_sleeptime() {
  SLEEP_CurrentTime = millis();
  SLEEP_ElapsedTime = SLEEP_CurrentTime - SLEEP_StartTime;

  //========= Sleep time counter =========//
  int counter = (SLEEP_Duration - SLEEP_ElapsedTime) / 1000;
  if (counter < 0) {
    counter = 0;
  }
  int minute_num;
  int second_num;

  minute_num = int(counter / 60);
  second_num = int(counter % 60);

  // Sleep mode
  for (int i = 0; i < lcdNum; i++) {
    for (int j = 0; j < lcdColNum; j++) {    //Num Col
      lcds[i]->setCursor(0, int(j / 4));
      lcds[i]->print("Cooling..." + String(minute_num) + "min " + String(second_num) + "sec");
    }
  }

  // Running mode
  if (SLEEP_ElapsedTime > SLEEP_Duration) {
    Serial.println("!!!!RUNNING MODE!!!!");
    Serial.println("!!!!RUNNING MODE!!!!");

    running_mode = true;
    RUN_StartTime = millis();
    LCD_RunMode();
  }
}

//Send 'S' char to peripherals for sleep mode
void send_sleepSignal() {
  byte sleepSignal = 's';
  for (int EM_CON_IDX = 0; EM_CON_IDX < Num_EM_Controller; EM_CON_IDX++) {
    Wire.beginTransmission(EM_Controllers_addr[EM_CON_IDX]); // Set address to start transmission
    Wire.write(sleepSignal);
    Wire.endTransmission();
    delay(50);
  }
}

////////======= LCD Control Functions =======////////
// LCD Initialize
void LCD_Setup() {
  int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));

  for (int i = 0; i < lcdNum; i++) {
    lcds[i]->init(); //begin();
  }
  for (int i = 0; i < charBitmapSize; i++) {
    lcds[i]->createChar(i, (uint8_t *)charBitmap[i]);
  }
  for (int i = 0; i < lcdNum; i++) { //Num LCD
    for (int j = 0; j < lcdColNum; j++) {    //Num Col
      lcds[i]->setCursor(0, j);
      lcds[i]->print("Hello world ! " + String(j));
    }
    delay(1000);
  }
}

void LCD_RunMode() {
  LCD_Refresh();
  for (int i = 0; i < lcdNum; i++) { //Num LCDs
    for (int j = 0; j < lcdColNum; j++) { //Num Column for each LCD
      lcds[i]->setCursor ( 0, j);
      lcds[i]->print (row_array[(i * 4) + j]); //Page
    }
  }
}

//LCD refresh when the page is changed
void LCD_Refresh() {
  for (int i = 0; i < lcdNum; i++) {
    lcds[i]->backlight();
    lcds[i]->clear();
    lcds[i]->home();
  }
}

// LCD reset when the whole text is reloaded
void LCD_Reset() {
  for (int i = 0; i < lcdNum; i++) {
    lcds[i]->noBacklight();
    lcds[i]->clear();
    lcds[i]->home();
  }
  delay (1000);
}

// LCD Text displaying and blinking a cursor
void LCD_Display() {

  // Display total 32 lines of text
  for (int j = 0; j < totalTextArrayLength; j += lcdColNum_total) {
    LCD_Parsing_Text(j);
    //LCD_Parsing_Text_Docu(j); //Docu
    LCD_Displaying_Text();

    //Reset Blinking position
    blinkCounterX = 0;
    blinkCounterY = 0;

    //******** for every matrix of LCDs ********//
    for (int colNum = 0; colNum < lcdColNum_total; colNum++) {
      for (int rowNum = 0; rowNum < lcdRowNum; rowNum++) {

        // Sleeping mode
        while (!running_mode) {
          // send OFF('0') signal to peripherals;
          send_sleepSignal();
          // check sleeping time
          check_sleeptime();
        }

        // Blink char is not reached to the end of the page 
        if (textReload == false) {

          char blink_Char = LCD_Set_Blink_letter();
          //char blink_Char_Docu = LCD_Set_Blink_letter_Docu(); // Documentation
          
          // Send characters to peripherals
          I2C_Communication(blink_Char);
          //I2C_Communication(blink_Char_Docu); //Documentation

          LCD_Blink_Cursor(blink_Duration, blink_Char);  //Change the name of function. No.2
          LCD_CheckTextReload();                         //Change the name of function. No.3
          LCD_Set_BlinkPos_X();                          //Change the name of function. No.4
        }
        LCD_Set_BlinkPos_Y();

        // Check running time
        check_runtime();
      }
    }
    //******** for every matrix of LCD ********//
  }
}

// Store characters into string arrays
void LCD_Parsing_Text(int _index) {
  for (int i = 0; i < lcdColNum_total; i++) {
    row_array[i]  = myText[_index + i];
  }
}

void LCD_Parsing_Text_Docu(int _index) {
  for (int i = 0; i < lcdColNum_total; i++) {
    row_array_Docu[i]  = myText_Docu[_index + i];
  }
}

//
void LCD_Displaying_Text() {
  for (int i = 0; i < lcdNum; i++) { //Num LCDs
    for (int j = 0; j < lcdColNum; j++) { //Num Column for each LCD
      lcds[i]->setCursor ( 0, j );
      lcds[i]->print (row_array[(i * 4) + j]);
    }
  }
}

////////======= Text cursor blinking controls =======////////
// Set the letter which has a same position of the cursor
char LCD_Set_Blink_letter() {

  char _char;

  if (blinkCounterY == 0) {
    _char = row_array[0].charAt(blinkCounterX);
  } else if (blinkCounterY == 1) {
    _char = row_array[1].charAt(blinkCounterX);
  } else if (blinkCounterY == 2) {
    _char = row_array[2].charAt(blinkCounterX);
  } else if (blinkCounterY == 3) {
    _char = row_array[3].charAt(blinkCounterX); //LCD 1
  } else if (blinkCounterY == 4) {
    _char = row_array[4].charAt(blinkCounterX);
  } else if (blinkCounterY == 5) {
    _char = row_array[5].charAt(blinkCounterX);
  } else if (blinkCounterY == 6) {
    _char = row_array[6].charAt(blinkCounterX);
  } else if (blinkCounterY == 7) {
    _char = row_array[7].charAt(blinkCounterX); //LCD 2
  } else if (blinkCounterY == 8) {
    _char = row_array[8].charAt(blinkCounterX);
  } else if (blinkCounterY == 9) {
    _char = row_array[9].charAt(blinkCounterX);
  } else if (blinkCounterY == 10) {
    _char = row_array[10].charAt(blinkCounterX);
  } else if (blinkCounterY == 11) {
    _char = row_array[11].charAt(blinkCounterX); //LCD 3
  }
  return _char;
}

char LCD_Set_Blink_letter_Docu() {
  
  char _char;

  if (blinkCounterY == 0) {
    _char = row_array_Docu[0].charAt(blinkCounterX);
  } else if (blinkCounterY == 1) {
    _char = row_array_Docu[1].charAt(blinkCounterX);
  } else if (blinkCounterY == 2) {
    _char = row_array_Docu[2].charAt(blinkCounterX);
  } else if (blinkCounterY == 3) {
    _char = row_array_Docu[3].charAt(blinkCounterX); //LCD 1
  } else if (blinkCounterY == 4) {
    _char = row_array_Docu[4].charAt(blinkCounterX);
  } else if (blinkCounterY == 5) {
    _char = row_array_Docu[5].charAt(blinkCounterX);
  } else if (blinkCounterY == 6) {
    _char = row_array_Docu[6].charAt(blinkCounterX);
  } else if (blinkCounterY == 7) {
    _char = row_array_Docu[7].charAt(blinkCounterX); //LCD 2
  } else if (blinkCounterY == 8) {
    _char = row_array_Docu[8].charAt(blinkCounterX);
  } else if (blinkCounterY == 9) {
    _char = row_array_Docu[9].charAt(blinkCounterX);
  } else if (blinkCounterY == 10) {
    _char = row_array_Docu[10].charAt(blinkCounterX);
  } else if (blinkCounterY == 11) {
    _char = row_array_Docu[11].charAt(blinkCounterX); //LCD 3
  }
  return _char;
}

// Set the X coordinate of blinking cursor and letter in the LCD screens
void LCD_Set_BlinkPos_X() {

  blinkCounterX++;

  if (blinkCounterX == lcdRowNum) {
    blinkCounterX = 0;
    blinkCounterY++;
  }
}

// Set the Y coordinate of blinking cursor and letter in the LCD screens
void LCD_Set_BlinkPos_Y() {

  if (blinkCounterY == lcdColNum_total) {
    blinkCounterY = 0;
  }
}

// Counting the number of text to check the reload of text
void LCD_CheckTextReload() {

  textCounter++;

  if (textCounter == 560) {
    textCounter = 0;
    pageNumber = 0;
    textReload = true;
  } else {
    textReload = false;
  }
}

// Change between a letter with a blinking cursor
void LCD_Blink_Cursor(int _blink_Duration, char _char) {

  for (int blinkNum = 0; blinkNum < blinkingNum; blinkNum++) {
    if (blinkCounterY >= 0 && blinkCounterY < 4) {
      //First screen (LCD 1)
      lcds[0]->setCursor (blinkCounterX, blinkCounterY);
      lcds[0]->print (char(0));
      delay(_blink_Duration);

      lcds[0]->setCursor (blinkCounterX, blinkCounterY);
      lcds[0]->print (_char);
      delay(_blink_Duration);
    }
    else if (blinkCounterY >= 4 && blinkCounterY < 8) {
      //Second screen (LCD 2)
      lcds[1]->setCursor (blinkCounterX, blinkCounterY - 4);
      lcds[1]->print (char(0));
      delay(_blink_Duration);

      lcds[1]->setCursor (blinkCounterX, blinkCounterY - 4);
      lcds[1]->print (_char);
      delay(_blink_Duration);
    }
    else {
      //Third screen (LCD 3)
      lcds[2]->setCursor (blinkCounterX, blinkCounterY - 8);
      lcds[2]->print (char(0));
      delay(_blink_Duration);

      lcds[2]->setCursor (blinkCounterX, blinkCounterY - 8);
      lcds[2]->print (_char);
      delay(_blink_Duration);
    }
  }
}

////////======= I2C_Communication_Functions =======////////
//
void I2C_Initialize() {

  I2C_Scanning(); // number_devices = I2C_Scanning();

  if (number_devices == 0) {
    Serial.println(F("No I2C devices found\n"));
    I2C_Initializaing = false;
  }
  else if (number_devices < Num_EM_Controller) {
    Serial.print(" Devices are missing!  Number of input devices = ");
    Serial.print(number_devices, DEC);
    Serial.println(F(" Not done\n"));
    I2C_Initializaing = false;
  }
  else {
    Serial.print("Number of input devices = ");
    Serial.print(number_devices, DEC);
    Serial.println(F(" done\n"));
    I2C_Initializaing = true;
    I2C_Handshaking = true;
  }

  delay(1000);           // wait 0.1 seconds for next scan
}

// Print out scaned devices
void printKnownChips(byte address)
{
  switch (address) {
    case 0x00: Serial.print(F("FALSE")); break;
    case 0x01: Serial.print(F("EM_CON_1")); break;
    case 0x02: Serial.print(F("EM_CON_2")); break;
    case 0x03: Serial.print(F("EM_CON_3")); break;
    case 0x04: Serial.print(F("EM_CON_4")); break;
    case 0x05: Serial.print(F("EM_CON_5")); break;
    case 0x06: Serial.print(F("EM_CON_6")); break;
    case 0x07: Serial.print(F("EM_CON_7")); break;
    case 0x25: Serial.print(F("LCD_3")); break;
    case 0x26: Serial.print(F("LCD_2")); break;
    case 0x27: Serial.print(F("LCD_1")); break; // MCLK required
    default: Serial.print(F("unknown chip"));
  }
}

// Establish an initial communication between the controller and peripherals
void I2C_Scanning() {
  byte error, address;

  Serial.println(F("Scanning..."));

  number_devices = 0;
  for (address = 1; address < 40; address++) { //total number of i2c devices LCD 1,2,3 + EM controllers (3/7) = 6

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("Device found at address 0x"));

      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.print("  (");
      printKnownChips(address);
      Serial.println(")");

      devices_i2c_add[number_devices] = address;

      Serial.print(devices_i2c_add[number_devices], DEC);
      Serial.println(" Stored in devices_i2c array");

      number_devices++;

    } else if (error == 4) {
      Serial.print(F("Unknown error at address 0x"));
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
}

// Establish a connection with peripherals before send any data from the controller
void I2C_Handshake() {
  int count_em_controller = 0; //Test
  byte error, addr;

  //Test for 3 devices...
  for (int i = 0; i < Num_EM_Controller; i++) {

    addr = EM_Controllers_addr[i];
    Wire.beginTransmission(addr);
    error = Wire.endTransmission();

    if (error == 0) {
      count_em_controller++;
    } else if (error == 4) {
      I2C_Handshaking = false;
    }
  }

  if (count_em_controller == Num_EM_Controller) {
    I2C_Handshaking = true;
    //ack_Flag = true;
    Serial.println();
    Serial.println("I2C Hand-shaking completed...");
  } else {
    I2C_Handshaking = false;
  }
}

//
void I2C_Transmiting(int _index, char _char) { //  how___1?

  // Convert hex num String into number upper-> 0-15, lower 0-15
  // Split hex num string into 2 segment upper/lower
  byte upperByte = ascii_Num >> 4;
  byte lowerByte = ascii_Num & 0xF;

  Wire.beginTransmission(EM_Controllers_addr[_index]); // Set address to start transmission

  // Send 7 bit(3 bits of upper / 4 bits of lower byte) to EM Controllers one at a time
  String lowBits = hexToBin[lowerByte]; //  how___1?
  String upBits = hexToBin[upperByte];

  // Set 4 lower bits for i2c 1,2,3,4
  if (_index < 4) {
    // Reversed order of charactor in charAt...
    // ex) "1101" -> index 0 -> '1' / index 1 -> '1' / index 2 -> '0' / index 3 -> '1'
    char sendChar = lowBits.charAt(3 - _index);// 0123 -> 3210 charAt(3->2->1->0)
    Wire.write(sendChar);
    Serial.println("I2C Transmitting... Controller Index = " + String(_index + 1) + " SEND CHAR = " + String(sendChar));
  } else {
    //set 3 upper bits for i2c 5,6,7
    char sendChar = upBits.charAt((7 - _index));// 456 -> 321  + 1 -> charAt(4->3->2)
    Wire.write(sendChar);
    Serial.println("I2C Transmitting... Controller Index = " + String(_index + 1) + " SEND CHAR = " + String(sendChar));
  }
  Wire.endTransmission();
}

//
void I2C_Receiving(int _index) {

  Wire.requestFrom(EM_Controllers_addr[_index], 7);
  while (Wire.available()) {
    char c = Wire.read();
    if (c == 'A') {
      // Debugging
      Serial.print("I2C Receiving...");
      Serial.print("  From the peripharal address: ");
      Serial.println(messages_From_peri);
      Serial.println();
      messages_From_peri = "";
    }
    else if (c == 'F') {
      // Debugging
      Serial.print("I2C Receiving...");
      Serial.print("  RELAY NOT WORKING From the peripharal address: ");
      Serial.println(messages_From_peri);
      Serial.println();
      messages_From_peri = "";
    }
    else {
      messages_From_peri += c;
    }
  }
}

// I2C trans-receiving with peripherals
void I2C_Communication(char _blinkChar) {

  I2C_Handshake();

  if (!I2C_Handshaking) {
    Serial.println("I2C Hand-shaking failed...");
    I2C_Initialize(); //Re establishment
  }
  else {
    Bits_Seperation(_blinkChar); //Debugging

    // for every(7) modules (For the test, only 3 devices)
    for (int EM_CON_IDX = 0; EM_CON_IDX < Num_EM_Controller; EM_CON_IDX++) {
      I2C_Transmiting(EM_CON_IDX, _blinkChar);
      I2C_Receiving(EM_CON_IDX);
      delay(100);
    }
  }
}

////////======= Text_Parsing_Functions =======////////
// Set lower Bits and Upper bits
void Bits_Seperation(char _char) {

  ascii_Num = _char;
  if (ascii_Num < 20) {
    ascii_Num = 20;
  }

  // Convert hex num String into number upper-> 0-15, lower 0-15
  // Split hex num string into 2 segment upper/lower
  byte upperByte = ascii_Num >> 4;
  byte lowerByte = ascii_Num & 0xF;

  String ascii_Hex = String(ascii_Num, HEX);

  // Serial monitor for debugging
  Serial.println("CHAR= " + String(_char) + " // ASCII HEX NUM= " + ascii_Hex +
                 " // SPLIT HEX= " + String(upperByte, HEX) + "|" + String(lowerByte, HEX) +
                 " // UP= " + String(upperByte) + ", BIN= " + String(hexToBin[upperByte]) +
                 " // DOWN= " + String(lowerByte) + ", BIN= " + String(hexToBin[lowerByte]) +
                 " // DECIMAL= " + String((upperByte * 16) + lowerByte) +
                 " // P_X= " + String(blinkCounterX) + " // P_Y= " + String(blinkCounterY) + " // T.C= " + String(textCounter));

}

////////======= Serial_Debugging_Functions =======////////
