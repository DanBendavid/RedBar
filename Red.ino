
///////////////////////////////////////////////////////////////////
////          RED Scrolling TUBE by Dan BENDAVID       ////////////
///////////////////////////////////////////////////////////////////

#include <MD_MAX72xx.h>
#include <SPI.h>

#include <MD_REncoder.h>


//#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>   
#include <WiFiUdp.h>
//#include <NTPClient.h>

#include <TimeLib.h>
#include <NtpClientLib.h>
int8_t timeZone = 1;
int8_t minutesTimeZone = 0;

tmElements_t tm;
int Year, Month, Day, Hour, Minute, Second ;

//#include <EEPROM.h>

#define  DEBUG  1

#if  DEBUG
#define PRINT(s, x) { Serial.print(s); Serial.print(x); }
#define PRINTS(x) Serial.print(x)
#define PRINTD(x) Serial.println(x, DEC)
#define PRINTF(x) Serial.println(x)
#define PRINTX(s, v) { Serial.print(s); Serial.print(v, HEX); } 
#define PRINTLN Serial.println()

#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)
#define PRINTF(x)
#define PRINTLN
#define PRINTT 

#endif


#include <TransilienEspApi.h>
#include <RatpEspApi.h>
#include <CoinMarketCapApi.h>
#include <AirQualityApi.h>

// Text parameters
#define  CHAR_SPACING  1 // pixels between characters

String buf,bufm, ratp, transilienA, transilienR, aqi, ticker, n_time;
String message_fix ="Bonne Annee 2019 - Friendship, love, health, energy, enthusiasm, and joy     - ";
char buf2 [300];
char buf3 [30];

unsigned long min_trajetA = 10; // Duree en min Trajet entre Station et Point 
unsigned long min_trajetR = 10; // Duree en min Trajet entre Station et Point 

//------- Replace the following! ------
//char ssid[] = "iPhone de Dan";       // your network SSID (name)
//char password[] = "dan1234567";  // your network key

const char* ssid     = "FR-PASSY";
const char* password = "i8co-8l9j-fr2e";

// Paris 
char city_id[7] = "@3090";


#define MAX_DEVICES 10
#define CLK_PIN   18  // or SCK
#define DATA_PIN  23  // or MOSI
#define CS_PIN    5  // or SS


// --------------------
// Constant parameters
//

#define ANIMATION_DELAY 75  // milliseconds

int DELAYTIME = 50; 
int INTENSITY = 10;
int MESSAGE = 2;
int SCROLLING_TYPE = 1;
enum mode_btn_a { OFF = 0, SELECT_INTENSITY = 1, SELECT_SPEEED = 2, SELECT_MESSAGE = 3, SELECT_SCROLLING_TYPE = 4, REGLAGE_TransilienA = 5, REGLAGE_TransilienR = 6, MIN_mode_btn_a = 0, MAX_mode_btn_a = 6} ;
mode_btn_a        mode_btn ;



String messages = "0: transilienA + aqi \n 1:transilienA + n_time \n2:transilienA + n_time + aqi \n 3:aqi \n4:ratp \n5:ticker \n6:n_time + aqi \n7:n_time \n8:transilienB \n9:message_fix \n10:message_fix + n_time";


/////////////////////////////////////////////

//MD_MAX72XX mx = MD_MAX72XX(CS_PIN, MAX_DEVICES);                      // SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES); // Arbitrary pins


//----------------------------------------------------------------------------------
// Encoder 
//----------------------------------------------------------------------------------
#define QUEUE_SIZE 15
#define SW_DOWN 'D'
#define SW_UP 'U'
#define PIN_A 2
#define PIN_B 4
#define PIN_BUT 14

#define button_long_press    800   // ms
#define button_short_press   120   // ms

xQueueHandle xQueue;

// set up encoder object
MD_REncoder R = MD_REncoder(PIN_A, PIN_B);

long  button_press_time = 0;

// CoinMarketCap's limit is "no more than 10 per minute"
// Make sure to factor in if you are requesting more than one coin.

//----------------------------------------------------------------------------------
unsigned long api_mtbs = 60000; //mean time between api requests (1 min)
unsigned long api_due_time = 0;

WiFiClientSecure clientS;
RatpEspApi apiC(clientS);
CoinMarketCapApi apiA(clientS);
TransilienEspApi apiD(clientS);

WiFiClient client;
AirQualityApi apiB(client);

#include "Textformat.h"


String select_text(int MESSAGE)
  {
String bufm = "";
    switch(MESSAGE) {
    case  0:     
    bufm = "OFF"; 
    break;
    
    case  1: 
    bufm = "Luminosity"; 
    break;
    
    case  2: 
    bufm = "Speed";
    break;
    
    case  3: 
    bufm = "Message";
    break;
    
    case  4: 
    bufm = "Scrolling Mode";
    break;
    
    case  5: 
    bufm = "Reglage Temps RER A";
    break;
    
    case  6: 
    bufm = "Reglage Temps RER R";
     break;
     
  }
  return bufm;
  }


String mode_text(int MESSAGE)
  {
String bufm = "";
    switch(MESSAGE) {
    case  0:     
    bufm = "0 - RER - AQI"; 
    break;
    
    case  1: 
    bufm = "1 - RER - TIME"; 
    break;
    
    case  2: 
    bufm = "2 - RER - TIME - AQI";
    break;
    
    case  3: 
    bufm = "3 - AQI";
    break;
    
    case  4: 
    bufm = "4 - Ligne 9 - Muette";
    break;
    
    case  5: 
    bufm = "5 - COINS";
    break;
    
    case  6: 
    bufm = "6 - TIME - AQI";
     break;
     
    case  7: 
     bufm = "7 - RER Retour";
     break;
     
    case  8: 
     bufm = "8 - TIME";
     break;
     
    case  9: 
     bufm = "9 - MESSAGE";break;
     
    case 10: 
     bufm = "10 - MESSAGE - TIME";
     break;
  }
  return bufm;
  }



//----------------------------------------------------------------------------------
void resetMatrix(void)
{
  mx.control(MD_MAX72XX::INTENSITY, INTENSITY);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
}

void ScrollText(char *p)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts
  
  //PRINTS("\nScrolling text");
  //mx.clear();

  while (*p != '\0')
  {
    charWidth = mx.getChar(*p++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);

    for (uint8_t i=0; i<charWidth + 1; i++) // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      if (i < charWidth)
        mx.setColumn(0, cBuf[i]);
      delay(DELAYTIME);
    }
 
  }

}


void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t    curLen;
  uint16_t  showLen;
  uint8_t   cBuf[8];
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        mx.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3: // display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}


//----------------------------------------------------------------------------------
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


//----------------------------------------------------------------------------------
// Mode Menu
//----------------------------------------------------------------------------------
mode_btn_a operator ++( mode_btn_a &id, int )
{
   mode_btn_a currentID = id;

   if ( MAX_mode_btn_a < id + 1 ) id = MIN_mode_btn_a;
   else id = static_cast<mode_btn_a>( id + 1 );
   return ( currentID );
}

void processButton() {
   static volatile uint8_t SWPrev = 1;


   uint8_t state =  digitalRead(PIN_BUT);
   uint8_t x;
   if (state == 0) {
      if (SWPrev == 1) {
        SWPrev = 0;
        x = SW_DOWN;
        button_press_time = millis();
        //Q.push((uint8_t *)&x);
        xQueueSendToBackFromISR( xQueue, (uint8_t *)&x, NULL );


        //PRINTS("\nDown\n");
        
      } 
   } else { 
      if (SWPrev == 0) {
        SWPrev = 1;
        x = SW_UP;
        
        //Q.push((uint8_t *)&x);
        xQueueSendToBackFromISR( xQueue, (uint8_t *)&x, NULL );

        if((millis() - button_press_time) >= button_long_press) {
        
        Serial.println ("long press: ");
        Serial.print (mode_btn_a(mode_btn));
        }
        else if((millis() - button_press_time) >= button_short_press) {
        mode_btn++; 
        Serial.println ("short press: ");
        Serial.print (mode_btn_a(mode_btn));
        
        bufm=select_text(mode_btn_a(mode_btn));
        bufm.toCharArray(buf3, 30);
        printText(0, MAX_DEVICES-1, buf3);
        }
        
        //PRINTS("\nUp\n");
      } 
   }
}
void processEncoder () {
  uint8_t x = R.read();
  if (x) {
    //PRINTS((x == DIR_CW ? "\n+1\n" : "\n-1\n"));
    if (x == DIR_CW) 
    {
     if (mode_btn == SELECT_INTENSITY) { 
    
      if (INTENSITY <17) {INTENSITY ++;} else {INTENSITY = 17;}
      
      Serial.println ("Intensity: ");
      Serial.print (INTENSITY); 
      mx.control(MD_MAX72XX::INTENSITY, INTENSITY);
      }
    
     else if (mode_btn == SELECT_SPEEED) {
     if (DELAYTIME <60) {DELAYTIME ++;} else {DELAYTIME = 60;}
    
      Serial.println (DELAYTIME);
      }

     else if (mode_btn == REGLAGE_TransilienA) {
     min_trajetA ++;
      Serial.println (min_trajetA);
      }

      else if (mode_btn == REGLAGE_TransilienR) {
     min_trajetR ++;
      Serial.println (min_trajetR);
      }
      
     else if (mode_btn == SELECT_MESSAGE) {
     MESSAGE = (MESSAGE +1)% 11;
      Serial.println ("Message: ");
      Serial.print (MESSAGE);
      
      Serial.println (mode_text(MESSAGE));
      
      bufm=mode_text(MESSAGE);
      bufm.toCharArray(buf3, 30);
      printText(0, MAX_DEVICES-1, buf3);
      }
     }  
    
    else if  (x == DIR_CCW)
    {
    
     if (mode_btn ==  SELECT_INTENSITY) { 
    
      if (INTENSITY >0) {INTENSITY --;} else {INTENSITY = 0;}
      
      Serial.println ("Intensity: ");
      Serial.print (INTENSITY);
      mx.control(MD_MAX72XX::INTENSITY, INTENSITY);
      }
      
     else if (mode_btn ==  SELECT_SPEEED) {
     if (DELAYTIME >20) {DELAYTIME --;} else {DELAYTIME = 20;}
     Serial.println ("Speed: ");
     Serial.print (DELAYTIME);
      }

     else if (mode_btn == REGLAGE_TransilienA) {
     min_trajetA --;
      Serial.println (min_trajetA);
      }
      else if (mode_btn == REGLAGE_TransilienR) {
     min_trajetR --;
      Serial.println (min_trajetR);
      }
     else if (mode_btn == SELECT_MESSAGE) {
     if (MESSAGE == 0) {
      MESSAGE = 10;
      } else  
      { MESSAGE = (MESSAGE - 1)% 11;
      }
     Serial.println ("Message: ");
     Serial.print (MESSAGE);
     Serial.println (mode_text(MESSAGE));
      
      bufm=mode_text(MESSAGE);
      bufm.toCharArray(buf3, 30);
      printText(0, MAX_DEVICES-1, buf3);
     
     }  
    }  
    //Q.push(&x);
    xQueueSendToBackFromISR( xQueue, (uint8_t *)&x, NULL );
  }
}

//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
void processSyncEvent (NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print ("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println ("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println ("Invalid NTP server address");
    } else {
        Serial.print ("Got NTP time: ");
        Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}

boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

String digits (int digits) {
 // utility function for digital clock display: prints preceding colon and leading 0
 String digit_format ="";
 if (digits < 10) {
  digit_format = "0"+String(digits); 
  } else {
  digit_format = String(digits);
                  }
 return digit_format;
}



void setup() {

Serial.begin(115200);

xQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));
pinMode(PIN_BUT, INPUT_PULLUP); // button to operate menu
attachInterrupt(digitalPinToInterrupt(PIN_BUT), processButton, CHANGE);
 
R.begin();
attachInterrupt(digitalPinToInterrupt(PIN_A), processEncoder, CHANGE);
attachInterrupt(digitalPinToInterrupt(PIN_B), processEncoder, CHANGE);

mx.begin();
resetMatrix();

  
//WiFiManager
//Local intialization. Once its business is done, there is no need to keep it around

  WiFiManagerParameter p_hint("<small>*Hint: if you want to reuse the currently active WiFi credentials, leave SSID and Password fields empty</small><br>"); 
  WiFiManagerParameter p_text("City id must include @ - "); 
  WiFiManagerParameter custom_city_id("City", "City Id", city_id, 5);

    
    WiFiManager wifiManager;

    
    //reset saved settings
    //wifiManager.resetSettings();
delay(2000);

      // check if the pushbutton is pressed.
      // if it is, the buttonState is LOW:
      if (digitalRead(0) == LOW) {
          Serial.println("Button Pushed");
          delay(100);
          wifiManager.resetSettings();}


  wifiManager.addParameter(&p_hint);
  wifiManager.addParameter(&p_text);
  wifiManager.addParameter(&custom_city_id);
  
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    
    wifiManager.autoConnect("Red_Setup");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();
 
  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%, 10% here
  wifiManager.setMinimumSignalQuality(40);

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(120);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    
  /* Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  */

  strcpy(city_id, custom_city_id.getValue());

  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
 
  
//  timeClient.begin();
  NTP.begin ("pool.ntp.org", timeZone, true, minutesTimeZone);
  NTP.setInterval (60);

}


void loop() {
//timeClient.update();
//readEncoder();

// Time
n_time = digits(hour())+":"+digits(minute())+":"+digits(second());

unsigned long timeNow = millis();
buf ="";  
switch(MESSAGE)
  {
    case  0:     
    if ((timeNow > api_due_time))  {
    aqi = printAirQualityData (city_id);
    transilienA = printTransilienEspData ("gare/87543181/depart/87111278", min_trajetA ); // Boulainvillier to Porte de Clichy
    api_due_time = timeNow + api_mtbs; 
    }
    buf = transilienA + aqi;

    break;
    case  1:
    if ((timeNow > api_due_time))  { 
    transilienA = printTransilienEspData ("gare/87543181/depart/87111278", min_trajetA ); // Boulainvillier to Porte de Clichy
    api_due_time = timeNow + api_mtbs; 
    }
    buf = transilienA + n_time;

    break;
    case  2:
    if ((timeNow > api_due_time))  {
    transilienA = printTransilienEspData ("gare/87543181/depart/87111278", min_trajetA ); // Boulainvillier to Porte de Clichy 
    api_due_time = timeNow + api_mtbs; 
    }
    buf = transilienA + n_time + aqi;

    break;
    case  3: 
    if ((timeNow > api_due_time))  {
    buf = aqi;
    api_due_time = timeNow + api_mtbs; 
    }
    break;
    
    case  4:
    if ((timeNow > api_due_time))  {
    ratp = printRatpEspData ("metros/9/la+muette/R"); 
    api_due_time = timeNow + api_mtbs; 
    }
    buf = ratp;

    break;
    case  5:
    if ((timeNow > api_due_time))  {
    ticker = printTickerData("bitcoin") + printTickerData("ethereum"); 
    api_due_time = timeNow + api_mtbs; 
    }
    buf = ticker;
    break;
    
    case  6: 
    if ((timeNow > api_due_time))  {
    aqi = printAirQualityData (city_id);
    api_due_time = timeNow + api_mtbs; 
    }
    buf = n_time + aqi;
    break;
    
    case  7:
    if ((timeNow > api_due_time))  {
    transilienR = printTransilienEspData ("gare/87111278/depart/87543181", min_trajetR); // Porte de Clichy-> Boulainvillier 
    api_due_time = timeNow + api_mtbs; 
    }
    buf = transilienR;
    break;
    
    case  8: 
    buf = n_time;
    break;
    
    case  9: 
    buf =  message_fix;
    break;
    
    case 10: 
    buf =  message_fix + n_time;
    break;
    
    case 11: 
    MESSAGE = 0; buf = transilienA + aqi;     break;
  
  }
  buf = "*"+String(MESSAGE)+ "*" + buf + "*"; 
  buf.toCharArray(buf2, 300);
  ScrollText(buf2);                    
}
