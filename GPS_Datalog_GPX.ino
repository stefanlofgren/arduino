#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

/////////////////////////// -------   GPS Configuration ---------- /////////////////
// This is specificall for: Arduino Uno, Adafruit Ultimate GPS Datalogging Shield, and the TinyGPS++ Library.  
static const int RXPin = 8, TXPin = 7;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

///////////////////////////  ------  SD Configuration ------------ ///////////////
File myFile;
const int chipSelect = 4;

void setup(){

  Serial.begin(115200);
   while (!Serial) {
  }
  ss.begin(GPSBaud);

  Serial.print("Initializing SD card...");
  
  pinMode(SS, OUTPUT);
   
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  ////////////////////////////// --------------- Open/Create File and Write GPX header Information -------------- ////////////////////

  myFile = SD.open("test.gpx", FILE_WRITE);
  
  myFile.println("<?xml version='1.0' encoding='UTF-8' standalone='no' ?>");
  myFile.println("<gpx xmlns=\"http://www.topografix.com/GPX/1/1\">");
  myFile.println("\t<trk>");
  myFile.print("\t\t");myFile.println("<name>Arduino GPX Document</name>");
  myFile.print("\t\t");myFile.println("<trkseg>");
  
  myFile.close();
  
  Serial.println("<?xml version='1.0' encoding='UTF-8' standalone='no' ?>");
  Serial.println("<gpx xmlns=\"http://www.topografix.com/GPX/1/1\">");
  Serial.println("\t<trk>");
  Serial.print("\t\t");myFile.println("<name>Arduino GPX Document</name>");
  Serial.print("\t\t");myFile.println("<trkseg>");
}

bool logData=true;
int dop=350;

void loop(){
  smartDelay(10000);
  if(gps.hdop.value()<dop && gps.satellites.value()>4 && logData==1){

    myFile = SD.open("test.gpx", FILE_WRITE);

    if (myFile) {
      myFile.print("\t\t\t<trkpt lat=\"");printFloat(gps.location.lat(), gps.location.isValid(),11,6,1);myFile.print("\" lon=\"");printFloat(gps.location.lng(), gps.location.isValid(),12,6,1);myFile.println("\">");
      myFile.print("\t\t\t\t<ele>");printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 0, 1);myFile.println("</ele>");
      myFile.print("\t\t\t\t<hdop>");printInt(gps.hdop.value(), gps.altitude.isValid(), 3, 1);myFile.println("</hdop>");
      myFile.print("\t\t\t\t<sat>"); printInt(gps.satellites.value(), gps.altitude.isValid(), 3, 1); myFile.println("</sat>");
      myFile.print("\t\t\t\t<time>");printDate(gps.date,1);myFile.print("T");printTime(gps.time,1);myFile.println("Z</time>");
      myFile.println("\t\t\t</trkpt>");


      Serial.print("\t\t\t<trkpt lat=\"");printFloat(gps.location.lat(),gps.location.isValid(),11,6,0);Serial.print("\" lon=\"");printFloat(gps.location.lng(), gps.location.isValid(),12,6,0);Serial.println("\">");
      Serial.print("\t\t\t\t<ele>");printFloat(gps.altitude.meters(),gps.altitude.isValid(),7,0,0);Serial.println("</ele>");
      Serial.print("\t\t\t\t<hdop>");printInt(gps.hdop.value(),gps.altitude.isValid(),3,0);myFile.println("</hdop>");
      Serial.print("\t\t\t\t<sat>");printInt(gps.satellites.value(),gps.altitude.isValid(),3,0);myFile.println("</sat>");
      Serial.print("\t\t\t\t<time>");printDate(gps.date,0);Serial.print("T");printTime(gps.time,0);Serial.println("Z</time>");
      Serial.println("\t\t\t</trkpt>");
   
    // close the file:
    
      myFile.close();
    }
    else{
      Serial.println("error opening test.gpx");
    }
  }
  else{
    Serial.print("HDOP:");
    Serial.print(gps.hdop.value());  
    Serial.print("     NumSats:");
    Serial.println(gps.satellites.value());  
  }
}

static void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do {
    while (ss.available()){
      gps.encode(ss.read());     
    }
  }
  while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec, bool printToMyFile){
  if (!valid){
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else{
    if(!printToMyFile){
      Serial.print(val, prec);
    }
    if(printToMyFile){
      myFile.print(val, prec);
    }
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i){ 
    }
  }
}

static void printInt(unsigned long val, bool valid, int len, bool printToMyFile)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i){
    //sz[i] = '';    
  }
  if (len > 0){
    //sz[len-1] = '';     
  }
  if(!printToMyFile){
    Serial.println(sz);
  }
  if(printToMyFile){
    myFile.print(sz);
  }
  smartDelay(0);
}

static void printDate(TinyGPSDate &d, bool printToMyFile){
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d-%02d-%02d", d.year(), d.month(), d.day());
    if(!printToMyFile){
      Serial.print(sz);
    }
    if(printToMyFile){
      myFile.print(sz);
    }
  }
  smartDelay(0);
}

static void printTime(TinyGPSTime &t, bool printToMyFile){
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
    if(!printToMyFile){
      Serial.print(sz);
    }
    if(printToMyFile){
      myFile.print(sz);
    }
  }
  smartDelay(0);
}
