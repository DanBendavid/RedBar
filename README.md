# RedBar
RED is a tube with 10 MAX72XX scrolling various Data like Air Quality Index, Bitcoins, Time and Schedule for Metro and RER in Paris
Using  TransilienEspApi.h , RatpEspApi.h, CoinMarketCapApi.h, and AirQualityApi.h
The Red Tube is based on ESP32 (should be ok with ESP8266 but not yet tested) and it is controlled with a Rotary Encoder (with the MD_REncoder from https://github.com/MajicDesigns/MD_REncoder). 
Pending devlopment : 
1 - Additional Flow : API for Alaphavantage Stock market, RSS, etc.....
2 - Webserver to control/manage initial configuration the RED Tube in addition, in particular to get station id.....
