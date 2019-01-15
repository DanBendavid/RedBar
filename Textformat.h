String printAirQualityData(String Cityid) {
  buf ="?";
  Serial.println("---------------------------------");
  Serial.println("Getting ticker data for " + Cityid);

  // Go to AirQuality.com and select the city you would like to check
  // Need to obtain a Token

  AirQualityResponse response = apiB.GetAirQualityInfo(Cityid);
  if (response.error == "") {

    //buf =  String(response.data_city_name) + "- AQI: " + String(response.data_aqi) + ", T:" + String(response.data_iaqi_t_v) + "C, " ;
        buf = " AQI: " + String(response.data_iaqi_pm25_v) + ", T:" + String(response.data_iaqi_t_v) + "C " ;
    Serial.print("City ID: ");
    Serial.println(response.data_idx);
    Serial.print("AQI: ");
    Serial.println(response.data_aqi);
    Serial.print("Time v: ");
    Serial.println(response.data_time_v);
    Serial.print("Time: ");
    Serial.println(response.data_time_s);
    Serial.print("Time Zone: ");
    Serial.println(response.data_time_tz);

    Serial.print("City name: ");
    Serial.println(response.data_city_name);
    Serial.print("City URL: ");
    Serial.println(response.data_city_url);

    Serial.print("City Long: ");
    Serial.println(response.data_city_geo0);
    Serial.print("City Lat: ");
    Serial.println(response.data_city_geo1);

    Serial.print("PM2.5: ");
    Serial.println(response.data_iaqi_pm25_v);
    Serial.print("PM10: ");
    Serial.println(response.data_iaqi_pm10_v);
    Serial.print("O3: ");
    Serial.println(response.data_iaqi_o3_v);
    Serial.print("SO2: ");
    Serial.println(response.data_iaqi_so2_v);
    Serial.print("CO: ");
    Serial.println(response.data_iaqi_co_v);
    Serial.print("Temp: ");
    Serial.println(response.data_iaqi_t_v);
    Serial.print("Pression: ");
    Serial.println(response.data_iaqi_p_v);
    Serial.print("H: ");
    Serial.println(response.data_iaqi_h_v);
    Serial.print("W: ");
    Serial.println(response.data_iaqi_w_v);
  

  } else {
    Serial.print("Error getting data: ");
    Serial.println(response.error);
  }
  Serial.println("---------------------------------");
  return buf;
}



String printTickerData(String ticker) {
  buf ="?";
  Serial.println("---------------------------------");
  Serial.println("Getting ticker data for " + ticker);


  // Ticker unfortunately is not the symbol for some reason.
  // Go to CoinMarketCap.com and select the coin you would like to check
  // The ticker name makes up the last part of the URL
  // e.g: http://coinmarketcap.com/currencies/bitcoin/ , "bitcoin" is the ticker value

  // Currency is optional, so you can pass only ticker if you want.
  // Check out the currency drop down on CoinMarketCap.com to get available values
  CMCTickerResponse response = apiA.GetTickerInfo(ticker, "eur");
  if (response.error == "") {
    
    
    //dtostrf(response.price_usd, 0, 1, buf);
     if (response.price_usd > 1000) 
    {buf =  response.symbol + ": " + round(response.price_usd) + "$ (" + response.percent_change_7d +"), " ;}
    else 
    {buf =  response.symbol + ": " + response.price_usd + "$ (" + response.percent_change_7d +"), " ;}
    
   /* Serial.print("ID: ");
    Serial.println(response.id);
    Serial.print("Name: ");
    Serial.println(response.name);
    Serial.print("Symbol: ");
    
    Serial.println(response.symbol);

    Serial.print("Rank: ");
    Serial.println(response.rank);

    Serial.print("Price in USD: ");
    Serial.println(response.price_usd);

    Serial.print("Price in BTC: ");
    Serial.println(response.price_btc);

    Serial.print("24h Volume USD: ");
    Serial.println(response.volume_usd_24h);
    Serial.print("Market Cap USD: ");
    Serial.println(response.market_cap_usd);

    Serial.print("Available Supply: ");
    Serial.println(response.available_supply);
    Serial.print("Total Supply: ");
    Serial.println(response.total_supply);

    Serial.print("Percent Change 1h: ");
    Serial.println(response.percent_change_1h);
    Serial.print("Percent Change 24h: ");
    Serial.println(response.percent_change_24h);
    Serial.print("Percent Change 7d: ");
    Serial.println(response.percent_change_7d);
    Serial.print("Last Updated: ");
    Serial.println(response.last_updated);

    // These fields will not come back if you do not request a currency
    Serial.print("Price in requested currecy: ");
    Serial.println(response.price_currency);
    Serial.print("24h Volume in requested currency: ");
    Serial.println(response.volume_currency_24h);
    Serial.print("Market Cap in requested currency: ");
    Serial.println(response.market_cap_currency);
*/

  } else {
    Serial.print("Error getting data: ");
    Serial.println(response.error);
  }
  Serial.println("---------------------------------");

return buf;
}

String printRatpEspData(String StationB) {
  buf ="?";
  Serial.println("---------------------------------");
  Serial.println("Getting Schedule data for " + StationB);

  RatpEspResponse response = apiC.GetRatpEspInfo(StationB);
  if (response.error == "") {

    buf =  "Ligne 9 " + response.schedules0_message + "/" + response.schedules1_message ;
 // buf =  "Next 52 in " + response.schedules0_message + "/ to " + response.schedules0_destination + " following " + response.schedules1_message ;
   
 /* 
    Serial.print("Prochain passage: ");
    Serial.println(response.schedules0_message );
    Serial.print("Destination: ");
    Serial.println(response.schedules0_destination);
    
    Serial.print("Passage suivant: ");
    Serial.println(response.schedules1_message );
    Serial.print("Destination: ");
    Serial.println(response.schedules1_destination);
        
    Serial.print("Call: ");
    Serial.println(response.metadata_call);
    Serial.print("Date: ");
    Serial.println(response.metadata_date);
    Serial.print("Version: ");
    Serial.println(response.metadata_version);
*/
  } else {
    Serial.print("Error getting data: ");
    Serial.println(response.error);
  }
  Serial.println("---------------------------------");
  return buf;

}

void createElements(const char *str)
{
  // Specific to Transilien
  sscanf(str, "%d/%d/%d %d:%d", &Day, &Month, &Year, &Hour, &Minute);
  tm.Year = CalendarYrToTm(Year);
  tm.Month = Month;
  tm.Day = Day;
  tm.Hour = Hour;
  tm.Minute = Minute;
  tm.Second = 0;
  //tm.Second = Second;
}


String printTransilienEspData(String StationB, unsigned long min_trajet) {

buf ="?";
char buft0 [20];
char buft1 [20];
char buft2 [20];
long t0, t1, t2;
 
Serial.println("---------------------------------");
Serial.println("Getting Schedule data for " + StationB);

tmElements_t myElements = {second(), minute(), hour(), weekday(), day(), month(), ( year()-1970) };
time_t myTime = makeTime(myElements);

TransilienEspResponse response = apiD.GetTransilienEspInfo(StationB);
  if (response.error == "") {
//char firstDateTime[] = {"04/01/2019 06:23"};

response.schedules0_message.toCharArray(buft0, 20);
createElements(buft0);// parsing tm
unsigned long schedules0 = makeTime(tm);
PRINTF(schedules0);
PRINTF(myTime);

if(schedules0 > myTime & response.schedules0_etat !="Supprimé")  
{
 t0 = (schedules0 - myTime)/60 - min_trajet;
} else 

{t0 =-1;} 

PRINTF (t0);


response.schedules1_message.toCharArray(buft1, 20); 
createElements(buft1);// parsing tm
unsigned long schedules1 = makeTime(tm);
PRINTF(schedules1);
PRINTF(myTime);

if(schedules1 > myTime & response.schedules1_etat !="Supprimé")  
{
 t1 = (schedules1 - myTime)/60 - min_trajet;
} else 

{t1 =-1;} 
PRINTF (t1);

response.schedules2_message.toCharArray(buft2, 20); 
createElements(buft2);// parsing tm
unsigned long schedules2 = makeTime(tm);

PRINTF(schedules2);
PRINTF(myTime);

if(schedules2 > myTime & response.schedules2_etat !="Supprimé")  
{
 t2 = (schedules2 - myTime)/60 - min_trajet;
} else 

{t2 =-1;} 
PRINTF(t2);


if (t0 > 0) {buf = "RER " + String (t0) + "/" + String(t1) + "mn ";}  else if 
(t0 == 0){buf = "RER Go Now or in " + String(t1) + " mn ";}
else if (t0 < 0) {
  if (t1 < 0) {
    buf = "RER " + String(t2) + " mn ";
    } else {
    buf = "RER " + String (t1) + "/" + String(t2) + " mn ";
           }   
    }

//buf =  RECresponse.schedules0_message + "/" + response.schedules1_message + "," ;
// buf =  "RER " + String (t0) + "/" + String(t1) + "/" + String(t2) + " mn" ;
    
    Serial.print("Prochain passage: ");
    Serial.println(response.schedules0_message );
    Serial.print("Destination: ");
    Serial.println(response.schedules0_destination);
    Serial.print("Etat: ");
    Serial.println(response.schedules0_etat);
    
    Serial.print("Passage suivant: ");
    Serial.println(response.schedules1_message );
    Serial.print("Destination: ");
    Serial.println(response.schedules1_destination);
    Serial.print("Etat: ");
    Serial.println(response.schedules1_etat);
    
    Serial.print("Passage suivant: ");
    Serial.println(response.schedules2_message );
    Serial.print("Destination: ");
    Serial.println(response.schedules2_destination);
    Serial.print("Etat: ");
    Serial.println(response.schedules2_etat);
    

    Serial.print("Passage suivant: ");
    Serial.println(response.schedules3_message );
    Serial.print("Destination: ");
    Serial.println(response.schedules3_destination);
    Serial.print("Etat: ");
    Serial.println(response.schedules3_etat);
         
    Serial.print("Call: ");
    Serial.println(response.metadata_call);
    Serial.print("Date: ");
    Serial.println(response.metadata_date);
    Serial.print("Version: ");
    Serial.println(response.metadata_version);

  } else {
    Serial.print("Error getting data: ");
    Serial.println(response.error);
  }
  Serial.println("---------------------------------");
  return buf;

}
