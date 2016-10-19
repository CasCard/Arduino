#include <SoftwareSerial.h>
#include <TinyGPS.h>
SoftwareSerial GPS(2,3); // RX, TX

//String str="";
TinyGPS gps;
void gpsdump(TinyGPS &gps);
bool feedgps();
void getGPS();
long lat, lon;
float LAT, LON;

int pinButton = 7; 
int pinButton_panic = 8;


String latitude="No Range      ";
String longitude="No Range     ";

int temp=0,i;


void setup() 
{
  pinMode(pinButton, INPUT);
   pinMode(pinButton_panic, INPUT);
  Serial.begin(9600);
  GPS.begin(9600);

  Serial.println("Vehicle Tracking System");
  delay(2000);
  gsm_init();
  
  Serial.println("AT+CNMI=2,2,0,0,0");
  Serial.println("GPS Initializing");
  Serial.println("  No GPS Range  ");
  delay(2000);
  Serial.print("Latitude : ");
  Serial.print(LAT/1000000,8);
  Serial.print(" :: Longitude : ");
  Serial.println(LON/1000000,8);
  get_gps();
  Serial.println("GPS Range Found");
  Serial.println("GPS is Ready");
  delay(2000);
  
  Serial.println("System Ready");
 
  temp=0;
}

void loop()
{
   int stateButton = digitalRead(pinButton);
  int stateButton_panic = digitalRead(pinButton_panic);
  if(stateButton==1 && stateButton_panic==0)
  {
    serialEvent();
    get_gps();
    tracking();
  }
  if(stateButton_panic==1 && stateButton==1){
    serialEvent();
    get_gps();
    panictracking();
  }
}

void serialEvent()
{
  int stateButton = digitalRead(pinButton);
  int stateButton_panic = digitalRead(pinButton_panic);
  while(stateButton==1){
    if(stateButton==1)
    {
      temp=1;
      break;
    }
    else
    temp=0;
   
  }
}



void gsm_init()
{
  
  Serial.println("Finding Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }

  
  Serial.println("Module Connected..");
  delay(1000);
 
  Serial.println("Disabling ECHO");
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }

  
  Serial.println("Echo OFF");
  delay(1000);
 
  Serial.println("Finding Network..");
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
  
  Serial.println("Network Found..");
  delay(1000);
  
}

void get_gps()
{
bool newdata = false;
  unsigned long start = millis();
  // Every 1 seconds we print an update
  while (millis() - start < 1000)
  {
    if (feedgps ()){
      newdata = true;
    }
  }
  if (newdata)
  {
    gpsdump(gps);
  }
  Serial.print("Latitude : ");
  Serial.print(LAT/1000000,8);
  Serial.print(" :: Longitude : ");
  Serial.println(LON/1000000,8);
}

bool feedgps(){
  while (GPS.available())
  {
    if (gps.encode(GPS.read()))
      return true;
  }
  return 0;
}
void gpsdump(TinyGPS &gps)
{
  //byte month, day, hour, minute, second, hundredths;
  gps.get_position(&lat, &lon);
  LAT = lat;
  LON = lon;
  {
    feedgps(); // If we don't feed the gps during this long 
//routine, we may drop characters and get checksum errors
  }
}

void init_sms()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"+917736560297\"");   // use your 10 digit cell no. here
  delay(400);
}

void send_data(String message)
{
  Serial.println(message);
  delay(200);
}
void send_coord(float message)
{
  Serial.println(message/1000000,8);
  delay(200);
}

void send_sms()
{
  Serial.write(26);
}

void serial_status()
{
  Serial.println("Message Sent");
  delay(2000);
  Serial.println("System Ready");
  return;
}

void tracking()
{
    init_sms();
    send_data("Accident Alert !");
   // send_data("Accident Current Location is:");
    //float latitude=(LAT);
   // float value_long=longitude;
   // String latitude_conv=String(latitude);
    //send_data(latitude_conv);
    //Serial.print("Longitude:");
    //float longitude=(LON);
    //float value_long=longitude;
   // String longitude_conv=String(longitude);
    //send_data(longitude_conv);
    Serial.print("Latitude:");
    send_coord(LAT);
   Serial.print("Longitude:");
   send_coord(LON);
    
    float latitude = LAT;
  float longitude = LON;
  String buf;
  buf += F("www.google.co.in/#q=");
  buf += String(latitude/1000000,7);
  buf += F("++");
  buf += String(longitude/1000000,7);
  Serial.print("Location Map:");
  send_data(buf);
    //send_data("Please do take some action soon..\nThank You");
    send_sms();
    delay(2000);
    serial_status();
}
void panictracking()
{
    init_sms();
   send_data("False Alert !");
    //send_data("Your Vehicle Current Location is:");
    Serial.print("Latitude:");
    send_coord(LAT);
    Serial.print("Longitude:");
    send_coord(LON);
     float latitude = LAT;
     float longitude = LON;
     String buf;
     buf += F("www.google.co.in/#q=");
     buf += String(latitude/1000000,7);
     buf += F("++");
     buf += String(longitude/1000000,7);
     Serial.print("Location Map:");
     send_data(buf);
    //send_data("Sorry for false alert \nThank You");
    send_sms();
    delay(2000);
    serial_status();
}

