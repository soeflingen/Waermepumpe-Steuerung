

//##############################################
//#Steuerung der Waermepumpe
//# P. Foschum, 1.8.2015
//##############################################
//#
/*
This Code has extra features 
including a XY positioning function on Display
and a Line Draw function on Nokia 3310 LCD 
It is modded from the original 
http://playground.arduino.cc/Code/PCD8544
*/

#include <OneWire.h>

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

//Temperaturschwellen / Variablen

int relais_led = 13;
char *cp_Ausgang_aktuell;

float f_temperatur;
char c_temperatur[25];
float  obere_Schaltschwelle = 50.0;
float untere_Schaltschwelle = 48.0;

// Zeiten ermitteln

unsigned long ul_actualTime_ms = 0;
unsigned long ul_last_actualTime_ms = 0;
unsigned long ul_onTimeStart_s = 0;
unsigned long ul_onTimeZaehler_s = 0;
unsigned long ul_offTimeStart_s = 0;
unsigned long ul_offTimeZaehler_s = 0;

unsigned long ul_offTimePost_s = 1;
unsigned long ul_onTimePost_s  = 1;
float         f_onTimePostVerhaeltnis  = 1.0;

boolean b_heizen;


// Temperatursensor Dalas DS 18s20

OneWire  ds(2);  // on pin 2 (a 4.7K resistor is necessary)

//Display 5110

#define PIN_SCE   7  // Über 10kOhm an 5V TTL!
#define PIN_RESET 6  // Über 10kOhm an 5V TTL!
#define PIN_DC    5  // Über 10kOhm an 5V TTL!
#define PIN_SDIN  4  // Über 10kOhm an 5V TTL!
#define PIN_SCLK  3  // Über 10kOhm an 5V TTL!

#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48

static const byte ASCII[][5] =
{
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20  
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ←
,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f →
};

void LcdCharacter(char character)
{
  LcdWrite(LCD_D, 0x00);
  for (int index = 0; index < 5; index++)
  {
    LcdWrite(LCD_D, ASCII[character - 0x20][index]);
  }
  LcdWrite(LCD_D, 0x00);
}

void LcdClear(void)
{
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
  {
    LcdWrite(LCD_D, 0x00);
  }
}

void LcdInitialise(void)
{
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xB1 );  // Set LCD Vop (Contrast). 
  LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.
  LcdWrite(LCD_C, 0x20 );
  LcdWrite(LCD_C, 0x0C );
}

void LcdString(char *characters)
{
  while (*characters)
  {
    LcdCharacter(*characters++);
  }
}

void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}


// gotoXY routine to position cursor 
// x - range: 0 to 84
// y - range: 0 to 5

void gotoXY(int x, int y)
{
  LcdWrite( 0, 0x80 | x);  // Column.
  LcdWrite( 0, 0x40 | y);  // Row.  

}



void drawLine(void)
{
  unsigned char  j;  
   for(j=0; j<84; j++) // top
	{
          gotoXY (j,0);
	  LcdWrite (1,0x01);
  } 	
  for(j=0; j<84; j++) //Bottom
	{
          gotoXY (j,5);
	  LcdWrite (1,0x80);
  } 	

  for(j=0; j<6; j++) // Right
	{
          gotoXY (83,j);
	  LcdWrite (1,0xff);
  } 	
	for(j=0; j<6; j++) // Left
	{
          gotoXY (0,j);
	  LcdWrite (1,0xff);
  }

}

// Temperatursensor

float  ReadTempSensor(void)
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(250);
    return -97;
  }

  //Serial.print("ROM =");
//  for( i = 0; i < 8; i++) {
    //Serial.write(' ');
    //Serial.print(addr[i], HEX);
//  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      //Serial.println("CRC is not valid!");
      return -98;
  }
  //Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return -99;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44);        // start conversion, use ds.write(0x44,1) with parasite power on at the end

  delay(100);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
//  Serial.print("  Temperature = ");
//  Serial.print(celsius);
//  Serial.println(" Celsius, ");
  //Serial.print(fahrenheit);
  //Serial.println(" Fahrenheit");
  
  return celsius;
}

/*///////////////////////////////////////////////////////////////////////////
*  floatToString.h
*
*  Usage: floatToString(buffer string, float value, precision, minimum text width)
*
*  Example:
*  char test[20];   string buffer
*  float M;         float variable to be converted
*  byte precision   precision -> number of decimal places
*  byte width       min text width -> character output width, 0 = no right justify
*
*  Serial.print(floatToString(test, M, 3, 7)); // call for conversion function
*  
*/

char * floatToString(char * outstr, double val, byte precision, byte widthp){
 char temp[16]; //increase this if you need more digits than 15
 byte i;

 temp[0]='\0';
 outstr[0]='\0';

 if(val < 0.0){
   strcpy(outstr,"-\0");  //print "-" sign
   val *= -1;
 }

 if( precision == 0) {
   strcat(outstr, ltoa(round(val),temp,10));  //prints the int part
 }
 else {
   unsigned long frac, mult = 1;
   byte padding = precision-1;
   
   while (precision--)
     mult *= 10;

   if ((float)mult == 0) {
     strcpy(outstr,"Div0!");
     return outstr;
   } else
   val += 0.5/(float)mult;      // compute rounding factor
   
   strcat(outstr, ltoa(floor(val),temp,10));  //prints the integer part without rounding
   strcat(outstr, ".\0"); // print the decimal point

   frac = (val - floor(val)) * mult;

   unsigned long frac1 = frac;

   while(frac1 /= 10)
     padding--;

   while(padding--)
     strcat(outstr,"0\0");    // print padding zeros

   strcat(outstr,ltoa(frac,temp,10));  // print fraction part
 }

 // generate width space padding
 if ((widthp != 0)&&(widthp >= strlen(outstr))){
   byte J=0;
   J = widthp - strlen(outstr);

   for (i=0; i< J; i++) {
     temp[i] = ' ';
   }

   temp[i++] = '\0';
   strcat(temp,outstr);
   strcpy(outstr,temp);
 }

 return outstr;
}



//////////////////////////////////////////////////////


void setup(void)
{
  
  // Serial
  Serial.begin(9600);

  // relais_led Ausgang
  
  pinMode(relais_led, OUTPUT);     
  
  // LCD
  LcdInitialise();
  LcdClear();
  gotoXY(0,0);
  LcdString("Thermcontrol");
  gotoXY(0,1);
  LcdString("V1.1");
  gotoXY(0,2);
  LcdString("P. Foschum");
  gotoXY(0,3);
  LcdString("1.8.2015-");
  gotoXY(0,4);
  LcdString(" 2.8.2015");
  delay(3000);
  LcdClear();
  
  b_heizen = false;
  cp_Ausgang_aktuell = "Aus";

}

////////////////////////////////////////////////////////////

void loop(void)
{
  String ausgabe_on;
  String ausgabe_off;
  int sekunden;
  int minuten;
  int stunden;
  char charBuf[25];

 
  // Temperatur auslesen
  int i_timeout = 0;
  do
  {
    f_temperatur = ReadTempSensor();
    i_timeout++;
    Serial.print(".");
  } while ( (f_temperatur < -60.0) && (i_timeout < 10));

  
  Serial.print("  Temperature = ");
  Serial.print(f_temperatur);
  Serial.println(" Celsius, ");
  
  // Zeit holen und Überlauf Erkennung
  ul_actualTime_ms = millis();
  if (ul_actualTime_ms < ul_last_actualTime_ms) {
    ul_last_actualTime_ms = 0;    // Zähler ist übergelaufen! Zeit falsch!
    ul_onTimeStart_s = 0;
    ul_offTimeStart_s = 0;
  } else {
    ul_last_actualTime_ms = ul_actualTime_ms;
  }
  
  // 2-Punkt Regler
  
  if( f_temperatur > obere_Schaltschwelle)        // nicht heizen
  {
    if (b_heizen == true) {  // Umgeschalten
      b_heizen = false;
      ul_offTimePost_s =ul_offTimeZaehler_s; // Zaehler für Verhältnis speichern
      ul_offTimeStart_s = ul_actualTime_ms / 1000; // Startwert speichern
      digitalWrite(relais_led, LOW);                      // Therme aus
      cp_Ausgang_aktuell = "Aus";
      Serial.println("Max Temp erreiecht. Aggregat aus.");
    }

  } else if( f_temperatur < untere_Schaltschwelle)   //  heizen
  {
    if (b_heizen == false) {  // Umgeschalten
      b_heizen = true;
      ul_onTimePost_s =ul_onTimeZaehler_s; // Zaehler für Verhältnis speichern
      ul_onTimeStart_s = ul_actualTime_ms / 1000; // Startwert speichern
      digitalWrite(relais_led, HIGH);                      // Therme an
      cp_Ausgang_aktuell = "An ";
      Serial.println("Min Temp erreiecht. Aggregat ein.");
    }
  }
  
  // Zaehler aktualisieren
  if (b_heizen == true) {
    ul_onTimeZaehler_s = (ul_actualTime_ms / 1000) - ul_onTimeStart_s;
  } else {
    ul_offTimeZaehler_s = (ul_actualTime_ms / 1000) - ul_offTimeStart_s;    
  }
  

// Zeiten in Strings wandeln ////////////////////////////////////

  // On Zeit
  sekunden = ul_onTimeZaehler_s  % 60;
  minuten = ul_onTimeZaehler_s  / 60;
  stunden = minuten / 60;
  minuten = minuten % 60;
  
  ausgabe_on = "";
  if (stunden < 10) ausgabe_on = String(ausgabe_on + "0");
  ausgabe_on = String(ausgabe_on + stunden);
  ausgabe_on = String(ausgabe_on + ":");
  if (minuten < 10) ausgabe_on = String(ausgabe_on + "0");
  ausgabe_on = String(ausgabe_on + minuten);
  ausgabe_on = String(ausgabe_on + ":");
  if (sekunden < 10) ausgabe_on = String(ausgabe_on + "0");
  ausgabe_on = String(ausgabe_on + sekunden);

  // OFF Zeit
  sekunden = ul_offTimeZaehler_s  % 60;
  minuten = ul_offTimeZaehler_s  / 60;
  stunden = minuten / 60;
  minuten = minuten % 60;
  
  ausgabe_off = "";
  if (stunden < 10) ausgabe_off = String(ausgabe_off + "0");
  ausgabe_off = String(ausgabe_off + stunden);
  ausgabe_off = String(ausgabe_off + ":");
  if (minuten < 10) ausgabe_off = String(ausgabe_off + "0");
  ausgabe_off = String(ausgabe_off + minuten);
  ausgabe_off = String(ausgabe_off + ":");
  if (sekunden < 10) ausgabe_off = String(ausgabe_off + "0");
  ausgabe_off = String(ausgabe_off + sekunden);

      Serial.print("Act: ");
      Serial.print(ul_actualTime_ms);
      Serial.print("  An: ");
      Serial.print(ausgabe_on);
      Serial.print("  Aus: ");
      Serial.println(ausgabe_off);


 
// LCD Ausgabe  /////////////////////////////////////////////
//  LcdClear();
  
  gotoXY(0,0);
  LcdString("Temp. ");
  sscanf (c_temperatur, "%f", f_temperatur);
  LcdString(floatToString(c_temperatur, f_temperatur, 2, 5) );
  LcdString("C");
  
  gotoXY(0,1);
  LcdString("Aggregat ");
  LcdString(cp_Ausgang_aktuell);
  
  gotoXY(0,3);
  LcdString("An  ");
  ausgabe_on.toCharArray(charBuf, 25);
  LcdString(charBuf);
  
  
  gotoXY(0,4);
  LcdString("Aus ");
  ausgabe_off.toCharArray(charBuf, 25);
  LcdString(charBuf);
  
  gotoXY(0,5);
  LcdString("= 1:");
  f_onTimePostVerhaeltnis = float(ul_offTimePost_s) / float(ul_onTimePost_s);
  LcdString(floatToString(charBuf, f_onTimePostVerhaeltnis, 3, 5));
  
  
  
  
  // Verzögerung
  delay(2000);
  //Serial.println();  
}

