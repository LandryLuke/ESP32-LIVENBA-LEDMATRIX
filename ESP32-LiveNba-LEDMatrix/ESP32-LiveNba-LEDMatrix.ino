/*****************************************************************************
    A sample project for making a HTTPS GET request on an ESP32
    and parsing it from JSON to be able to update scores on a
    32x64 LED Matrix.
    Will connect to the the given request, parse the JSON, then 
    save values to be displayed on the 32x64 matirx.
    Parts:
    ESP32 Dev Board
       Amazon: https://www.amazon.com/KeeYees-ESP8266-ESP-12F-Development-Compatible/dp/B08ZY7Q7TW/ref=pd_sbs_sccl_2_2/134-1453380-0613927?pd_rd_w=oZI2b&pf_rd_p=3676f086-9496-4fd7-8490-77cf7f43f846&pf_rd_r=PMVFN5EDY92BJR1FKNT8&pd_rd_r=3b018dab-eb14-462b-9565-1a0dcd9e2be1&pd_rd_wg=0oAPi&pd_rd_i=B08ZY7Q7TW&psc=1
    32x64 LED Matrix Panel
       Amazon: https://www.amazon.com/waveshare-Full-Color-Adjustable-Brightness-Displaying/dp/B09MB426K5/ref=sr_1_5?crid=3CQ8316P315JC&keywords=32x64+led+matrix&qid=1652112503&sprefix=32x64+LED+%2Caps%2C185&sr=8-5 
 *****************************************************************************/

//=============================
// SPECIAL THANKS TOO:
// BRIAN LOUGH 
// Github:
//  https://github.com/witnessmenow
//=============================


//=============================
// Libraries
//=============================
#include <IRrecv.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>    // Library used for parsing Json from the API responses
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>


#define PANEL_RES_X 64      // Number of pixels wide of each panel. 
#define PANEL_RES_Y 32     // Number of pixels tall of each panel.
#define PANEL_CHAIN 1      // Total number of panels chained together

MatrixPanel_I2S_DMA *dma_display = nullptr;

//=============================
// Colors 
//=============================
uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
uint16_t myYELLOW = dma_display->color565(150, 125, 0);

//========WIFI========

char ssid[] = "<SSID>";       // your network SSID (name)
char password[] = "<PASSWORD>";  // your network passsword

// For HTTPS requests
WiFiClientSecure client;


// BASE URL
#define TEST_HOST "api-basketball.p.rapidapi.com"

// SERVER CERT
// You can use this tutorial to learn how to find Root CA's:
// - https://techtutorialsx.com/2017/11/18/esp32-arduino-https-get-request/

const char* server_cert = "-----BEGIN CERTIFICATE-----\n"\
"-----END CERTIFICATE-----\n";

String gID = "";
String htname, atname, hscore, ascore, hdisp, adisp;  //What will be displayed
String JsonBUFFER; // Buffer needed to parse

int RECV_PIN = 33;       // set input of reciver pin for use of IR remote

IRrecv irrecv(RECV_PIN);     
decode_results results; 


void setup() {

  Serial.begin(115200);
    // Configure module
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );

  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;

  // Set Up Display
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); //0-255
  dma_display->clearScreen();
  dma_display->fillScreen(dma_display->color444(0, 0, 0));

  // Enable IR pin
  irrecv.enableIRIn();         


  // Connect to WiFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //--------

  // Checking the cert is the best way on an ESP32
  // !! Without proper syntax for cert, client will not connect !!

  client.setInsecure();

}

//==============================================
// Method to make the get request and store data
//==============================================

void makeHTTPRequest() {

  // Opening connection to server (Port 443 for https)
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }

  // give the esp a breather
  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http; //Object of class HTTPClient
    http.begin("https://api-basketball.p.rapidapi.com/games?id=" + gID, server_cert);
    http.addHeader("x-rapidapi-key", "<Insert Rapid-API key here>");

    int httpCode = http.GET();
    Serial.println(httpCode);


    if (httpCode > 0) 
    {
      JsonBUFFER = (http.getString());
      Serial.println(JsonBUFFER);
      
      StaticJsonDocument<240> filter;

      JsonObject filter_response_0 = filter["response"].createNestedObject();

      JsonObject filter_response_0_status = filter_response_0.createNestedObject("status");
      filter_response_0_status["short"] = true;
      filter_response_0_status["timer"] = true;

      JsonObject filter_response_0_teams = filter_response_0.createNestedObject("teams");
      filter_response_0_teams["home"]["name"] = true;
      filter_response_0_teams["away"]["name"] = true;

      JsonObject filter_response_0_scores = filter_response_0.createNestedObject("scores");
      filter_response_0_scores["home"]["total"] = true;
      filter_response_0_scores["away"]["total"] = true;

      StaticJsonDocument<384> doc;

      DeserializationError error = deserializeJson(doc, JsonBUFFER, DeserializationOption::Filter(filter));

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      JsonObject response_0 = doc["response"][0];

      //To get quarter and time left
      //const char* response_0_status_short = response_0["status"]["short"]; // "Q2"
      //makequarter(response_0_status_short);
      //int response_0_status_timer = response_0["status"]["timer"]; // "3"
      //maketimel(response_0_status_timer);


      for (JsonPair response_0_team : response_0["teams"].as<JsonObject>()) {
        String response_0_team_key = response_0_team.key().c_str(); // "home", "away"

        String response_0_team_value_name = response_0_team.value()["name"]; // Team names
        makeTName(response_0_team_value_name, response_0_team_key);
      }
      for (JsonPair response_0_score : response_0["scores"].as<JsonObject>()) {
        String response_0_score_key = response_0_score.key().c_str(); // "home", "away"

        int response_0_score_value_total = response_0_score.value()["total"]; // get scores
        makeTScore(response_0_score_value_total, response_0_score_key);
      }
      http.end();

    }
  }
}
//==============================================
// Method to make abbreviation for Team Names
//==============================================

String makeabrv(String tname){
  if (tname == "New Orleans Pelicans"){
    return "NOP";
  }
  else if (tname == "Minnesota Timberwolves"){
    return "MIN";
  }
  else if (tname == "Memphis Grizzlies"){
    return "MEM";
  }
  else if (tname == "Brooklyn Nets"){
    return "BKN";
  }
  else if (tname == "Boston Celtics"){
    return "BOS";
  }
  else if (tname == "Atlanta Hawks"){
    return "ATL";
  }
  else if (tname == "Miami Heat"){
    return "MIA";
  }
  else if (tname == "Toronto Raptors"){
    return "TOR";
  }
  else if (tname == "Philadelphia 76ers"){
    return "PHI";
  }
  else if (tname == "Golden State Warriors"){
    return "GSW";
  }
  else if (tname == "Denver Nuggets"){
    return "DEN";
  }
  else if (tname == "Dallas Mavericks"){
    return "DAL";
  }
  else if (tname == "Utah Jazz"){
    return "UTA";
  }
  else if (tname == "Chicago Bulls"){
    return "CHI";
  }
  else if (tname == "Milwaukee Bucks"){
    return "MIL";
  }
  else if (tname == "Phoenix Suns"){
    return "PHX";
  }
  else if (tname == "Indiana Pacers"){
    return "IND";
  }
  else{
    return "NUL";
  }
}
//==============================================
// Method to Make tname variables
//==============================================
void makeTName(String tname, String tkey){
  if(tkey == "home"){
    htname = makeabrv(tname);
  }
  else{
    atname = makeabrv(tname);
  }
}
void makeTScore(int tscore, String tk){
  if(tk == "home"){
    hscore = tscore;
  }
  else{
    ascore = tscore;
  }
}
//==============================================
// Method to Make time variables
//==============================================
/*void maketimel(int tl){
  timel = tl;
}
void makequarter(const char*q){
  if (q == "FT"){
    quarter == " OVER!";
  }
  else{
    quarter == q;
  }
}*/
//==============================================
// Method to Display Data on LED Matrix
//==============================================
void displayscore(){
  
  dma_display->clearScreen();
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap
  // DISPLAY AWAY
  dma_display->setTextColor(myWHITE);
  dma_display->setCursor(5, 3);    // start at 5,3
  dma_display->print(adisp);
  //DISPLAY HOME
  dma_display->setCursor(5, 12);    // start at 5,12
  dma_display->setTextColor(myRED);
  dma_display->print(hdisp);
  //DISPLAY TMELEFT AND QUARTER
  dma_display->setCursor(5, 21);    // start at 5,21
  dma_display->setTextColor(myYELLOW);
  dma_display->print(qat);
}
//==============================================
// Method to Make disp variables
//==============================================
void makestring(){
  hdisp = (htname + "   " + hscore);
  adisp = (atname + "   " + ascore);
  qat = ("GAME OVER"); 
}
//==============================================
// Method to decode IR signal into chars
//==============================================
void IRdecode(int ircode){
  switch(ircode){
    case 16738455: //0
    gID = gID + '0';
    break;
    case 16724175: //1
    gID = gID + '1';
    break;
    case 16718055: //2
    gID = gID + '2';
    break;
    case 16743045: //3
    gID = gID + '3';
    break;
    case 16716015: //4
    gID = gID + '4';
    break;
    case 16726215: //5
    gID = gID + '5';
    break;
    case 16734885: //6
    gID = gID + '6';
    break;
    case 16728765: //7
    gID = gID + '7';
    break;
    case 16730805: //8
    gID = gID + '8';
    break;
    case 16732845: //9
    gID = gID + '9';
    break;
    default:
    break;
  }
}
//==============================================
// Method to Print gID while entering
//==============================================
void printgID()
{
  dma_display->clearScreen();
  dma_display->setTextSize(1);     // size 2 = 16 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
  dma_display->setTextColor(myWHITE);
  dma_display->setCursor(1, 1);    // start at 1,8
  dma_display->print("ENTER");
  dma_display->setCursor(1, 10);    // start at 1,8
  dma_display->print("GAME ID");
  dma_display->setCursor(1, 19);    // start at 1,8
  dma_display->print(gID);  
}
void loop() {
    if (irrecv.decode(&results) && results.value != 16712445  && results.value != 16720605){
      unsigned int value = results.value;
      Serial.println(results.value);
      IRdecode(results.value);
      printgID();                    //ADD new number to gID and display #
      irrecv.resume();
      
    }
    else if(irrecv.decode(&results) && (16712445 == results.value)){
      Serial.println(results.value);
      makeHTTPRequest();
      makestring();
      displayscore();             // Display scores and update every 15 seconds
      delay(15000);
      irrecv.resume();
      j++;
    }
    else if(irrecv.decode(&results) && (16720605 == results.value)){
      gID = "";
      printgID();                // Enter a new gID
      irrecv.resume();
    }

    
    delay(1000);
}
