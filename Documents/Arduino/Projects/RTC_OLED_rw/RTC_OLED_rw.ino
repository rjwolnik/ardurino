// Arduino real time clock with DS1307 and SSD1306 OLED
 
#include <Wire.h>                        // Include Wire library (required for I2C devices)
#include <Adafruit_GFX.h>                // Include Adafruit graphics library
#include <Adafruit_SSD1306.h>            // Include Adafruit SSD1306 OLED driver
 
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
 
#define button1    9                       // Button B1 is connected to Arduino pin 9
#define button2    8                       // Button B2 is connected to Arduino pin 8

// define DS18B20 data pin connection
#define DS18B20_PIN   A0 
char Time[]     = "  :  :  ";
char Calendar[] = "  /  /20  ";
byte i, second, minute, hour, day, date, month, year;
unsigned int ds18b20_temp;
char Buffer[] = "        ";
char Buffer_Tz[] = "Tzad=   C";
byte Thi, Tlo;
byte Tz;

void setup(void) {
  
  Serial.begin(9600);   //only for debug
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(1000);
 
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
 
  // Clear the display buffer.
  display.clearDisplay();
  display.display();
 
  display.setTextColor(WHITE, BLACK);
  Tz=25;
}
 

void display_day(){
  switch(day){
    case 1:  draw_text(40, 0, " SUNDAY  ", 1); break;
    case 2:  draw_text(40, 0, " MONDAY  ", 1); break;
    case 3:  draw_text(40, 0, " TUESDAY ", 1); break;
    case 4:  draw_text(40, 0, "WEDNESDAY", 1); break;
    case 5:  draw_text(40, 0, "THURSDAY ", 1); break;
    case 6:  draw_text(40, 0, " FRIDAY  ", 1); break;
    default: draw_text(40, 0, "SATURDAY ", 1);
  }
}
 
void DS1307_display(){
  // Convert BCD to decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour   = (hour >> 4)   * 10 + (hour & 0x0F);
  date   = (date >> 4)   * 10 + (date & 0x0F);
  month  = (month >> 4)  * 10 + (month & 0x0F);
  year   = (year >> 4)   * 10 + (year & 0x0F);
  //Tz     = (Tz>>4) * 10 + (Tz & 0x0F);
  // End conversion
 
  Time[7]     = second % 10 + 48;
  Time[6]     = second / 10 + 48;
  Time[4]     = minute % 10 + 48;
  Time[3]     = minute / 10 + 48;
  Time[1]     = hour   % 10 + 48;
  Time[0]     = hour   / 10 + 48;
  Calendar[9] = year   % 10 + 48;
  Calendar[8] = year   / 10 + 48;
  Calendar[4] = month  % 10 + 48;
  Calendar[3] = month  / 10 + 48;
  Calendar[1] = date   % 10 + 48;
  Calendar[0] = date   / 10 + 48;
  Buffer_Tz[5] = Tz / 10 + 48;
  Buffer_Tz[6] = Tz % 10 +48;
  
  draw_text(4,  10, Calendar, 1);
  draw_text(70, 10, Time, 1);
  draw_text(4, 20, Buffer, 1);
  draw_text(70, 20, Buffer_Tz, 1);
}
 
void blink_parameter(){
  byte j = 0;
  while(j < 10 && digitalRead(button1) && digitalRead(button2)){
    j++;
    delay(25);
  }
}
 
byte edit(byte x_pos, byte y_pos, byte parameter){
  char text[3];
  sprintf(text,"%02u", parameter);
  while(!digitalRead(button1));                      // Wait until button B1 released
  while(true){
    while(!digitalRead(button2)){                    // If button B2 is pressed
      parameter++;
      if(i == 0 && parameter > 31)                   // If date > 31 ==> date = 1
        parameter = 1;
      if(i == 1 && parameter > 12)                   // If month > 12 ==> month = 1
        parameter = 1;
      if(i == 2 && parameter > 99)                   // If year > 99 ==> year = 0
        parameter = 0;
      if(i == 3 && parameter > 23)                   // If hours > 23 ==> hours = 0
        parameter = 0;
      if(i == 4 && parameter > 59)                   // If minutes > 59 ==> minutes = 0
        parameter = 0;
      if(i == 5 && parameter > 99)                   // If Tz > 59 ==> Tz = 0
        parameter = 0;
        
      sprintf(text,"%02u", parameter);
      draw_text(x_pos, y_pos, text, 1);
      delay(200);                                    // Wait 200ms
    }
    draw_text(x_pos, y_pos, "  ", 1);
    blink_parameter();
    draw_text(x_pos, y_pos, text, 1);
    blink_parameter();
    if(!digitalRead(button1)){                       // If button B1 is pressed
      i++;                                           // Increament 'i' for the next parameter
      return parameter;                              // Return parameter value and exit
    }
  }
}
 
void draw_text(byte x_pos, byte y_pos, char *text, byte text_size) {
  display.setCursor(x_pos, y_pos);
  display.setTextSize(text_size);
  display.print(text);
  display.display();
}
 
void loop() {
 
  if(!digitalRead(button1)){                         // If button B1 is pressed
    i = 0;
    while(!digitalRead(button1));                    // Wait for button B1 release
    while(true){
      while(!digitalRead(button2)){                  // While button B2 pressed
        day++;                                       // Increment day
        if(day > 7) day = 1;
        display_day();                               // Call display_day function
        delay(200);                                  // Wait 200 ms
      }
      draw_text(40, 0, "         ", 1);
      blink_parameter();                             // Call blink_parameter function
      display_day();                                 // Call display_day function
      blink_parameter();                             // Call blink_parameter function
      if(!digitalRead(button1))                      // If button B1 is pressed
        break;
    }
 
    date   = edit(4, 10, date);                      // Edit date
    month  = edit(22, 10, month);                    // Edit month
    year   = edit(52, 10, year);                    // Edit year
    hour   = edit(70, 10, hour);                     // Edit hours
    minute = edit(88, 10, minute);                   // Edit minutes
    Tz = edit(100,  20, Tz);                         // Edit Temp
 
    // Convert decimal to BCD
    minute = ((minute / 10) << 4) + (minute % 10);
    hour = ((hour / 10) << 4) + (hour % 10);
    date = ((date / 10) << 4) + (date % 10);
    month = ((month / 10) << 4) + (month % 10);
    year = ((year / 10) << 4) + (year % 10);
    // End conversion
 
    // Write data to DS1307 RTC
    Wire.beginTransmission(0x68);               // Start I2C protocol with DS1307 address
    Wire.write(0);                              // Send register address
    Wire.write(0);                              // Reset sesonds and start oscillator
    Wire.write(minute);                         // Write minute
    Wire.write(hour);                           // Write hour
    Wire.write(day);                            // Write day
    Wire.write(date);                           // Write date
    Wire.write(month);                          // Write month
    Wire.write(year);                           // Write year
    Wire.endTransmission();                     // Stop transmission and release the I2C bus
    delay(200);                                 // Wait 200ms
  }
    // temperature measurment
    if(ds18b20_read(&ds18b20_temp))
    {
      if (ds18b20_temp & 0x8000)          // if temperature < 0
      {
        ds18b20_temp = ~ds18b20_temp + 1;  // change temperature value to positive form
        //sprintf(Buffer, "-%2u.%2u C", (ds18b20_temp/16) % 100, (ds18b20_temp & 0x0F) * 625);
            Thi = (ds18b20_temp/16) % 100;
            Tlo = ((ds18b20_temp & 0x0F) * 625)/100;
            Buffer[0]='T';
            Buffer[1]= '=';
            Buffer[2]='-';
            Buffer[3]= Thi % 10 + 48;
            Buffer[4]= Thi / 10 + 48;
            Buffer[5]='.';
            Buffer[6]= Tlo % 10 + 48;
            Buffer[7]= Tlo / 10 + 48;
            Buffer[8]= ' ';
            Buffer[9]= 'C';
      }
      else
      {      // otherwise (temperature >= 0)
        if (ds18b20_temp/16 > 100) {          // if temperature >= 100 °C
          //sprintf(Buffer, "%03u.%2u C", ds18b20_temp/16, (ds18b20_temp & 0x0F) * 625);
            Thi = (ds18b20_temp/16) % 100;
            Tlo = ((ds18b20_temp & 0x0F) * 625)/100;
            Buffer[0]= 'T';
            Buffer[1]= '=';
            Buffer[2]= Thi / 100 + 48;
            Buffer[3]= Thi / 10 + 48;
            Buffer[4]= Thi % 10 + 48;
            Buffer[5]='.';
            Buffer[6]= Tlo / 10 + 48;
            Buffer[7]= Tlo % 10 + 48;
            Buffer[8]= ' ';
            Buffer[9]= 'C';
        }
        else {      // otherwise ( 0 <= temperature < 100)
            //sprintf(Buffer, " %2d.%d C", ds18b20_temp/16, (unsigned char)((ds18b20_temp & 0x0F) * 625));
            Thi = (ds18b20_temp/16);
            Tlo = ((ds18b20_temp & 0x0F) * 625)/100;
            Buffer[0]= 'T';
            Buffer[1]= '=';
            Buffer[2]= ' ';
            Buffer[3]= Thi / 10 + 48;
            Buffer[4]= Thi % 10 + 48;
            Buffer[5]='.';
            Buffer[6]= Tlo / 10 + 48;
            Buffer[7]= Tlo % 10 + 48;
            Buffer[8]= ' ';
            Buffer[9]= 'C';
        }
      }
   }
   else 
   {
    sprintf(Buffer, "ERROR");
    draw_text(4, 20, "ERROR", 1);
   }
  delay(50); 
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS1307 address
  Wire.write(0);                                // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 7);                    // Request 7 bytes from DS1307 and release I2C bus at end of reading
  second = Wire.read();                         // Read seconds from register 0
  minute = Wire.read();                         // Read minuts from register 1
  hour   = Wire.read();                         // Read hour from register 2
  day    = Wire.read();                         // Read day from register 3
  date   = Wire.read();                         // Read date from register 4
  month  = Wire.read();                         // Read month from register 5
  year   = Wire.read();                         // Read year from register 6
 
  display_day();
  DS1307_display();                             // Diaplay time & calendar
 Serial.print(Tz);
 Serial.print(Thi);
 if(Thi<Tz) 
  digitalWrite(LED_BUILTIN, HIGH); 
 else 
  digitalWrite(LED_BUILTIN, LOW); 
  delay(50);                                    // Wait 50ms 
}

bool ds18b20_start()
{
  bool ret = 0;
  digitalWrite(DS18B20_PIN, LOW);  // send reset pulse to the DS18B20 sensor
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(500);          // wait 500 us
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(100);          // wait to read the DS18B20 sensor response
  if (!digitalRead(DS18B20_PIN))
  {
    ret = 1;                  // DS18B20 sensor is present
    delayMicroseconds(400);   // wait 400 us
  }
  return(ret);
}
 
void ds18b20_write_bit(bool value)
{
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  digitalWrite(DS18B20_PIN, value);
  delayMicroseconds(80);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(2);
}
 
void ds18b20_write_byte(byte value)
{
  byte i;
  for(i = 0; i < 8; i++)
    ds18b20_write_bit(bitRead(value, i));
}
 
bool ds18b20_read_bit(void)
{
  bool value;
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(5);
  value = digitalRead(DS18B20_PIN);
  delayMicroseconds(100);
  return value;
}
 
byte ds18b20_read_byte(void)
{
  byte i, value;
  for(i = 0; i < 8; i++)
    bitWrite(value, i, ds18b20_read_bit());
  return value;
}
 
bool ds18b20_read(int *raw_temp_value)
{
  if (!ds18b20_start())  // send start pulse
    return(0);
  ds18b20_write_byte(0xCC);   // send skip ROM command
  ds18b20_write_byte(0x44);   // send start conversion command
  while(ds18b20_read_byte() == 0);  // wait for conversion complete
  if (!ds18b20_start())             // send start pulse
    return(0);                      // return 0 if error
  ds18b20_write_byte(0xCC);         // send skip ROM command
  ds18b20_write_byte(0xBE);         // send read command
  
  *raw_temp_value = ds18b20_read_byte();   // read temperature LSB byte and store it on raw_temp_value LSB byte
  *raw_temp_value |= (unsigned int)(ds18b20_read_byte() << 8);   // read temperature MSB byte and store it on raw_temp_value MSB byte
  
  return(1);  // OK --> return 1
}

// End of code.
