/*
Version 1.0 supports OLED display's with either SDD1306 or with SH1106 controller
*/

#include <ESP8266WiFi.h>
#include <Wire.h>
#include "font.h"
#define offset 0x00    // SDD1306                      // offset=0 for SSD1306 controller
//#define offset 0x02    // SH1106                       // offset=2 for SH1106 controller
#define OLED_address  0x3c                             // all the OLED's I have seen have this address
#define SSID "Patiya"                              // insert your SSID
#define PASS "patiyapatiya"                              // insert your password
// ******************* String form to sent to the client-browser ************************************
String form =
  "<p>"
  "<center>"
  "<h1>Talk to me :-)</h1>"
  "<img src='http://i.imgur.com/qu8lDEu.jpg'>"
  "<form action='msg'><p>Wassup? <input type='text' name='msg' size=50 autofocus> <input type='submit' value='Submit'></form>"
  "</center>";

 long period;
static void clear_display(void);
static void sendCharXY(unsigned char data, int X, int Y);
static void init_OLED(void);
static void reset_display(void);
static void clear_display(void);
static void SendChar(unsigned char data); 
static void sendCharXY(unsigned char data, int X, int Y);
static void sendcommand(unsigned char com);
static void setXY(unsigned char row,unsigned char col);
static void sendStr(unsigned char *string);
static void sendStrXY( char *string, int X, int Y);
static void init_OLED(void);
void displayOff(void);
void displayOn(void);
String getLEDFromOrion();
byte server[] = {130,206,80,47};
/*
  handles the messages coming from the webbrowser, restores a few special characters and 
  constructs the strings that can be sent to the oled display
*/
WiFiClient client;
int ledCountArduino=0;
void setup(void) {
//ESP.wdtDisable();                               // used to debug, disable wachdog timer, 
  Serial.begin(115200);                           // full speed to monitor
  Wire.begin(4,5);                                // Initialize I2C and OLED Display
  init_OLED();                                    // 
  reset_display();
  WiFi.begin(SSID, PASS);                         // Connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {         // Wait for connection
    delay(500);
    Serial.print(".");
  }
  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
//  server.on("/", []() {
//    server.send(200, "text/html", form);
//  });
//  server.on("/msg", handle_msg);                  // And as regular external functions:
//  server.begin();                                 // Start the server 
  clear_display();

  Serial.print("SSID : ");                        // prints SSID in monitor
  Serial.println(SSID);                           // to monitor             
  sendStrXY("SSID :" ,0,0);  sendStrXY(SSID,0,7); // prints SSID on OLED
 
  char result[16];
  sprintf(result, "%3d.%3d.%3d.%3d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  Serial.println();Serial.println(result);
  sendStrXY(result,2,0);

  Serial.println("WebServer ready!   ");
  sendStrXY("WebServer ready!",4,0);              // OLED first message 
  Serial.println(WiFi.localIP());                 // Serial monitor prints localIP
  Serial.print(analogRead(A0));
  Wire.beginTransmission(9); //transmit to device 9 (Arduino);
  Wire.write(ledCountArduino);
  Wire.endTransmission();
}

//Get led state from Orion
String getLEDfromOrion(){
  Serial.println("\nStarting connection with the Server...");
  // If I connect with the server in the port (1026) report via serial                                   
  if (client.connect(server, 1026)) {
    Serial.println("Connected to the Server");
    // Make a HTTP request:                                                                              
    client.println("GET /v1/contextEntities/LEDREVERTERIS HTTP/1.1");
    client.println("Accept: application/json");
    client.println("Content-Type: application/json");
    //    client.print("X-Auth-Token: ");                                                                
    // client.println("59CO7vvjVhv7_06oJTmTinV5t2A73q0IDLdcaib3_K7hRk6rF2lli-DZUKIljnU9T2mIY9DtwGxFKeuQY\
qVtGg");                                                                                                 
    client.println("Connection: close");
    client.println();
  }
  // if the server's disconnected, stop the client:                                                      
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnected from the server");
    client.flush();
    client.stop();
    // do nothing forevermore:                                                                           
    //while(true);                                                                                       
  }
  // Read and print bytes comming from the server if available                                           
  String buffer;
  buffer = client.readString();
  Serial.println(buffer);
//  while (client.available()) {                                                                         
//    char c = client.read();                                                                            
//    Serial.print(c);                                                                                   
//    buffer = buffer + c;                                                                               
//  }                                                                                                    
  client.flush();
  client.stop();
  return buffer;
}

void loop(void) {
   delay(3000);
  // Declare a string to store the request buffer that arduino is listening to                                                                                      
  String buf;
  //Execute the call to the server                                                                       
  buf = getLEDfromOrion();
  Serial.println(buf);
  //remove blank space
  buf.replace("\n","");
  buf.replace("\t","");
  //search for the string we are interested in
  int startIndex = buf.indexOf("{");
  int endIndex = buf.length();
  String json = buf.substring(startIndex, endIndex);
  Serial.println(json);
  //Search is the value is set to true (that means the led is ON according to the db in orion)
  int val = json.indexOf("\"value\" : \"true\"");
  Serial.println(val);
  if(val == -1){
  //server.handleClient();                        // checks for incoming messages
    Wire.beginTransmission(9); //transmit to device 9 (Arduino);
    Wire.write(0); //Send 0 over i2c to the arduino with adress 9 -> switch off led
    Wire.endTransmission();
  }
  else{
    Wire.beginTransmission(9); //transmit to device 9 (Arduino);
    Wire.write(1); //Send 0 over i2c to the arduino with adress 9 -> switch off led
    Wire.endTransmission();
  }
  delay(1500);
}






//==========================================================//
// Resets display depending on the actual mode.
static void reset_display(void)
{
  displayOff();
  clear_display();
  displayOn();
}

//==========================================================//
// Turns display on.
void displayOn(void)
{
  sendcommand(0xaf);        //display on
}

//==========================================================//
// Turns display off.
void displayOff(void)
{
  sendcommand(0xae);    //display off
}

//==========================================================//
// Clears the display by sendind 0 to all the screen map.
static void clear_display(void)
{
  unsigned char i,k;
  for(k=0;k<8;k++)
  { 
    setXY(k,0);    
    {
      for(i=0;i<(128 + 2 * offset);i++)     //locate all COL
      {
        SendChar(0);         //clear all COL
        //delay(10);
      }
    }
  }
}

//==========================================================//
// Actually this sends a byte, not a char to draw in the display. 
// Display's chars uses 8 byte font the small ones and 96 bytes
// for the big number font.
static void SendChar(unsigned char data) 
{
  //if (interrupt && !doing_menu) return;   // Stop printing only if interrupt is call but not in button functions
  
  Wire.beginTransmission(OLED_address); // begin transmitting
  Wire.write(0x40);//data mode
  Wire.write(data);
  Wire.endTransmission();    // stop transmitting
}

//==========================================================//
// Prints a display char (not just a byte) in coordinates X Y,
// being multiples of 8. This means we have 16 COLS (0-15) 
// and 8 ROWS (0-7).
static void sendCharXY(unsigned char data, int X, int Y)
{
  setXY(X, Y);
  Wire.beginTransmission(OLED_address); // begin transmitting
  Wire.write(0x40);//data mode
  
  for(int i=0;i<8;i++)          
    Wire.write(pgm_read_byte(myFont[data-0x20]+i));
    
  Wire.endTransmission();    // stop transmitting
}

//==========================================================//
// Used to send commands to the display.
static void sendcommand(unsigned char com)
{
  Wire.beginTransmission(OLED_address);     //begin transmitting
  Wire.write(0x80);                          //command mode
  Wire.write(com);
  Wire.endTransmission();                    // stop transmitting
}

//==========================================================//
// Set the cursor position in a 16 COL * 8 ROW map.
static void setXY(unsigned char row,unsigned char col)
{
  sendcommand(0xb0+row);                //set page address
  sendcommand(offset+(8*col&0x0f));       //set low col address
  sendcommand(0x10+((8*col>>4)&0x0f));  //set high col address
}


//==========================================================//
// Prints a string regardless the cursor position.
static void sendStr(unsigned char *string)
{
  unsigned char i=0;
  while(*string)
  {
    for(i=0;i<8;i++)
    {
      SendChar(pgm_read_byte(myFont[*string-0x20]+i));
    }
    *string++;
  }
}

//==========================================================//
// Prints a string in coordinates X Y, being multiples of 8.
// This means we have 16 COLS (0-15) and 8 ROWS (0-7).
static void sendStrXY( char *string, int X, int Y)
{
  setXY(X,Y);
  unsigned char i=0;
  while(*string)
  {
    for(i=0;i<8;i++)
    {
      SendChar(pgm_read_byte(myFont[*string-0x20]+i));
    }
    *string++;
  }
}


//==========================================================//
// Inits oled and draws logo at startup
static void init_OLED(void)
{
  sendcommand(0xae);    //display off
  sendcommand(0xa6);            //Set Normal Display (default)
    // Adafruit Init sequence for 128x64 OLED module
    sendcommand(0xAE);             //DISPLAYOFF
    sendcommand(0xD5);            //SETDISPLAYCLOCKDIV
    sendcommand(0x80);            // the suggested ratio 0x80
    sendcommand(0xA8);            //SSD1306_SETMULTIPLEX
    sendcommand(0x3F);
    sendcommand(0xD3);            //SETDISPLAYOFFSET
    sendcommand(0x0);             //no offset
    sendcommand(0x40 | 0x0);      //SETSTARTLINE
    sendcommand(0x8D);            //CHARGEPUMP
    sendcommand(0x14);
    sendcommand(0x20);             //MEMORYMODE
    sendcommand(0x00);             //0x0 act like ks0108
    
    //sendcommand(0xA0 | 0x1);      //SEGREMAP   //Rotate screen 180 deg
    sendcommand(0xA0);
    
    //sendcommand(0xC8);            //COMSCANDEC  Rotate screen 180 Deg
    sendcommand(0xC0);
    
    sendcommand(0xDA);            //0xDA
    sendcommand(0x12);           //COMSCANDEC
    sendcommand(0x81);           //SETCONTRAS
    sendcommand(0xCF);           //
    sendcommand(0xd9);          //SETPRECHARGE 
    sendcommand(0xF1); 
    sendcommand(0xDB);        //SETVCOMDETECT                
    sendcommand(0x40);
    sendcommand(0xA4);        //DISPLAYALLON_RESUME        
    sendcommand(0xA6);        //NORMALDISPLAY             

  clear_display();
  sendcommand(0x2e);            // stop scroll
  //----------------------------REVERSE comments----------------------------//
    sendcommand(0xa0);    //seg re-map 0->127(default)
    sendcommand(0xa1);    //seg re-map 127->0
    sendcommand(0xc8);
    delay(1000);
  //----------------------------REVERSE comments----------------------------//
  // sendcommand(0xa7);  //Set Inverse Display  
  // sendcommand(0xae);   //display off
  sendcommand(0x20);            //Set Memory Addressing Mode
  sendcommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
  //  sendcommand(0x02);         // Set Memory Addressing Mode ab Page addressing mode(RESET)  
}



