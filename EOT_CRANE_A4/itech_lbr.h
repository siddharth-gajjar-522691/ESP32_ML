

#define SENSOR_ID 2//"L&T_Crane_A1";

#define Server_Timeout 3000
#define Wifi_LED      15
#define DATA_PUSH     2
#define Response      "OK"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <RTClib.h>
#include "time.h"
#include <SPI.h>
#include <DS3232RTC.h>
//#include <mySD.h>
#include <SD.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
//#include "Input_Pins.h"

#define Fautlt_reg_str  "GET /Boiler_CMS/Home/GetFaultRegisters"
//#define PRINT_FILE_DATA


#define MAC1 = "24:62:AB:FD:1B:80";//A4 - BC4
#define MAC2 = "FC:F5:C4:01:4F:1C";//B2 - BC2
#define MAC3 = "24:62:AB:FD:37:7C";//B3 - BC3  SENSOR_ID = 2 DRIVER ID : 5,6,7,8
#define MAC4 = "F0:08:D1:D8:41:D4";// A3 installed

#define MAC5 = "FC:F5:C4:01:50:70";//A1


unsigned int Response_fault =0;
WiFiClient client;
RTC_DS3231 rtc; 
DS3232RTC RTC;
File myFile;
EthernetClient Ethernet_client;
IPAddress ip(192, 168, 1, 130);

//BluetoothSerial SerialBT;
//WiFiServer server(80);
//WebServer server(80);


String HTML="";
unsigned int id;
int In_Counter =0;
int Out_Counter = 0;
long Serial_Data_timeout =0;
String Data_str = "";
unsigned int Person_entered[100],Person_exit[100];
unsigned int count=0;
String ReadString="";
byte store[4];
uint8_t reader = 0;

char* Data_File = "/data.txt";
char* Ptr_File  = "/prg_ptr.txt";
long data_ptr = 0;
int Data=0; 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDE };

long restart_time =0;

int Fault_1=0;
int Fault_2=0;
long driver_reading_id=0;
long driver_reading_id_LT=0,driver_reading_id_CT=0,driver_reading_id_MH=0,driver_reading_id_AH=0;

//const char* ssid     = "WiFi_Digital";
//const char* password = "W!F!@DiGi";
//const char* IP       = "172.27.1.234";
//const int  PORT      = 9191;

//const char* ssid     = "Dwij";
//const char* password = "qwerty1234";
//const char* IP       = "192.168.43.96";
//const int  PORT      = 9191;

//const char* ssid;     //      = "i Technology";
//const char* password; //      = "qwerty1234";
//char IP[20];          //      = "192.168.1.100";
//int  PORT;            //      = 9191;

// A3 LOCAL IP : 10.7.139.120

//char* ssid     = "**LMB-HZW-WCMS**"; //"**LMB-HZW-M/C**";**LMB-HZW-WCMS**
//char* password = "Power@1234";
//char* IP       = "10.7.74.158";
//int  PORT      = 80;

char* ssid     = "i Technology"; //"**LMB-HZW-M/C**";**LMB-HZW-WCMS**
char* password = "qwerty1234";
char* IP       = "i-technology.in";
int  PORT      = 80;

void check_response_RTC(String str);
void printDirectory(File dir, int numTabs);
void config_system(void);
int Wifi_data_send(String Data_str);
void Data_Send(String Data_str,int driveid);
int Serial_command(void);
int check_connection(void);
int Save_to_SD(String Data_str);
int Read_SD_to_send(void);
void WiFi_RTC_SD_Init(void);
void WiFi_RTC_SD_Ethernet_Init(void);
void RTC_SD_Ethernet_Init(void);
void SD_init();
void LED_init();
void RTC_init();
void ETHERNET_init();  
void Prepare_HTML_page(); 
  
int SD_CS = 5;
int SD_MISO= 23;
int SD_MOSI= 19;
int SD_SCK = 18;

char file_ssid[20];// = "";
char file_pwd[20];// = "";
String PATH="";

void SD_init(void){
  if (!SD.begin(5))//if (!SD.begin(SD_CS, SD_MISO, SD_MOSI, SD_SCK)) //
  {
    Serial.println(F("SD CARD FAILED!"));
  }
  else{
    Serial.println(F("SD CARD OK."));
  }
  
  myFile = SD.open("/");
  printDirectory(myFile, 0);
  myFile = SD.open(Data_File);

#ifdef PRINT_FILE_DATA
  if (myFile) {
    Serial.println(Data_File); 
    
    while (myFile.available()) {
      Serial.write(myFile.read());
      if(Serial_command()==0) break;
    }
    myFile.close();
  }
  else {   
    Serial.println("error opening");
  }
#endif
}

void LED_init(void){  
  pinMode(Wifi_LED,    OUTPUT);
  pinMode(DATA_PUSH, OUTPUT);
}

void RTC_init(void){
  rtc.begin();
  RTC.begin();
  DateTime now = rtc.now();
  Serial.println(String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + ',' + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
}

void WIFI_init(char* wifi_ssid, char* wifi_pwd){
  Serial.print(" Wifi Mac Address: ");  Serial.println(WiFi.macAddress());
//  if(WiFi.macAddress()==MAC1)
//     SENSOR_ID="L&T_Crane_A4";
     
  int n = WiFi.scanNetworks();
  Serial.println("\r\nscan done\r\n");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(" networks found: "); Serial.println(n);
    for (int i = 0; i < n; ++i)
    {
      Serial.print(i + 1); Serial.print(": "); Serial.print(WiFi.SSID(i)); Serial.print(" (");  Serial.print(WiFi.RSSI(i)); Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");            delay(100);
    }
  } 
  Serial.println("\r\n");
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(wifi_ssid, wifi_pwd);
  }
  int count = 0;
  long  timecount = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if ((millis() - timecount) > 10000)
      break;
  }
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  
}

void ETHERNET_init(void){
  Serial.println(F("Configuring Ehternet....."));
  Ethernet.init(5);
  for (int i = 0; i < 3; i++)
  {
    Serial.print("Hardware Status: ");
    auto Hardware = Ethernet.hardwareStatus();
    switch (Hardware) {
      case EthernetNoHardware:
        Serial.println("No Hardware");
        break;
      case EthernetW5100:
        Serial.println("Ethernet W5100");
        break;
      case EthernetW5200:
        Serial.println("Ethernet W5200");
        break;
      case EthernetW5500:
        Serial.println("Ethernet W5500");
        break;
    }
    auto link = Ethernet.linkStatus();
    Serial.print("Link status: ");
    switch (link) {
      case Unknown:
        Serial.println("Unknown");
        break;
      case LinkON:
        Serial.println("ON");
        break;
      case LinkOFF:
        Serial.println("OFF");
        break;
    }
  }
  if (Ethernet.begin(mac) == 0){
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    Ethernet.begin(mac, ip);
  }
  else{
    Serial.println(F("Ethernet Initialized"));
  }
//  delay(1000);
  Serial.print(F("Ethernet IP : "));
  Serial.println(Ethernet.localIP());
}

int check_connection(void){
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    digitalWrite(Wifi_LED, HIGH);
    Serial.println("WIFI : NO\t");
    delay(500);
    if((millis()-restart_time)/1000>900)
    {
      ESP.restart();
    }
  }
  else
  {
    restart_time=millis();
    digitalWrite(Wifi_LED, LOW);
    Serial.print("WIFI : YES \t");Serial.println(WiFi.RSSI());
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    
  }

#ifdef ETHERNET
  Ethernet.init(5);
  for (int i = 0; i < 3; i++)
  {
    Serial.print("Hardware Status: ");
    auto Hardware = Ethernet.hardwareStatus();
    switch (Hardware) {
      case EthernetNoHardware:
        Serial.println("No Hardware");
        break;
      case EthernetW5100:
        Serial.println("Ethernet W5100");
        break;
      case EthernetW5200:
        Serial.println("Ethernet W5200");
        break;
      case EthernetW5500:
        Serial.println("Ethernet W5500");
        break;
    }
    auto link = Ethernet.linkStatus();
    Serial.print("Link status: ");
    switch (link) {
      case Unknown:
        Serial.println("Unknown");
        break;
      case LinkON:
        Serial.println("ON");
        break;
      case LinkOFF:
        Serial.println("OFF");
        break;
    }
  }
//  if (Ethernet.begin(mac) == 0)
//  {
//    Serial.println(F("Failed to configure Ethernet using DHCP"));
    Ethernet.begin(mac, ip);
//  }
//  else{
//    Serial.println(F("Ethernet Initialized"));
//  }
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
//    digitalWrite(Ethernet_LED, HIGH);
    Serial.print("ETHER N");
  }
  else
  {
    //    digitalWrite(Ehternet_LED,LOW);   // harware found
    if (Ethernet.linkStatus() == LinkOFF)
    { //
//      digitalWrite(Ethernet_LED, HIGH);
      Serial.println("ETHER N");
    }
    else
    {
//      digitalWrite(Ethernet_LED, LOW);
      Serial.println("ETHER Y");
    }
  }
  #endif
}

void check_response(String str){
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, str);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  Fault_1=0;
  Fault_2=0;
  const char* errorMessage = doc["errorMessage"];
  String responseStatus = doc["responseStatus"];
  Fault_1 = doc["responseMessage"]["FAULT_REGISTERS"][0];
  Fault_2 = doc["responseMessage"]["FAULT_REGISTERS"][1];
  Serial.print("errorMessage:\t ");   Serial.print(errorMessage);
  Serial.print("responseStatus:\t "); Serial.println(responseStatus);
  Serial.print("Fault_1:\t ");        Serial.println(Fault_1);
  Serial.print("Fault_2:\t ");        Serial.println(Fault_2);

  if(Fault_1 !=0)
  {
    Response_fault = 1;
  }
}

void check_status_response(String str,int driveid){
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, str);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  
  const char* errorMessage = doc["errorMessage"];
  String responseStatus = doc["responseStatus"];
  driver_reading_id = doc["responseMessage"]["id"];

  if(driveid == 1){ driver_reading_id_LT = doc["responseMessage"]["id"];}
  if(driveid == 2){ driver_reading_id_CT = doc["responseMessage"]["id"];}
  if(driveid == 3){ driver_reading_id_MH = doc["responseMessage"]["id"];}
  if(driveid == 4){ driver_reading_id_AH = doc["responseMessage"]["id"];}
  
  Serial.print("errorMessage:\t ");             Serial.print(errorMessage);
  Serial.print("responseStatus:\t ");           Serial.println(responseStatus);
  Serial.print("driver_reading_id:\t ");        Serial.println(driver_reading_id);
 
}

int Wifi_data_send(String Data_str, int driveid){ 
  if (WiFi.status() == WL_CONNECTED)  {
    DateTime now = rtc.now();
    Serial.println("WiFi Connected.");
    if (client.connect(IP, PORT) | client.available())
    { 
      Serial.print(F("Server Connected, Pushing local data to server: "));
      Serial.print("Connecting to :");
      Serial.println(client.remoteIP());
//       Data_str = "{\"device\":\"SDP_TEST\",\"data\":[{\"tag\":\"UID_DEC\",\"value\":" +String(id)+"},{\"tag\":\"UID_HEX\",\"value\":\""+String(id,HEX)+"\"},{\"tag\":\"Reader\",\"value\":"+String(reader)+"},{\"tag\":\"MEDIUM\",\"value\":\""+String("WIFI")+"\"}],\"time\":"+String(now.unixtime())+"000}";
      delay(2000);
      client.println(Data_str);
//      Serial.print("Wifi_Data_send: ");
//    Serial.println(Data_str);   
      long timeout3 = millis();

      while (client.available() == 0)      {
        if (millis() - timeout3 > Server_Timeout)
        {
          Serial.println(F(">>> Client Timeout !"));
          client.stop();
          return 0;
        }
        String str = client.readString();
        Serial.print(str);
        
        if(Data_str.indexOf("SaveDriverReading")>-1)
        {
          check_status_response(str,driveid);
        }
        else if(Data_str.indexOf("GetFaultRegisters")>-1)
        {
          check_response(str);
        }
                
        if (str.indexOf(F(Response)) > -1)
        {
          Serial.print("\r\nString Captured : ");
          Serial.println(str);
          check_response_RTC(str);
          client.stop();
          return 1;
        }
      }
      if (client.available())
      {
        String str = client.readString();
        Serial.print(str);
        client.stop();
        if (str.indexOf(F(Response)) > -1)
        {
          Serial.print("\r\nString Captured : ");
          Serial.println(str);
          client.stop();
          return 1;
        }
      }
      client.stop();
    }
    else
    {
      
      Serial.println("Client connect failed");
      client.stop();
      delay(50);
      return 0;
    }
  }
  else
  {
    Serial.println("Wifi Not connected");
    return 0;
  }
}

int Ehternet_data_send(String Data_string){
  Serial.println("connecting serever through Ethernet");
  DateTime now = rtc.now();
  if (Ethernet_client.connect(IP, PORT))
  {
    Serial.println("connected");
//    Data_string = "{\"device\":\"SDP_TEST\",\"data\":[{\"tag\":\"UID_DEC\",\"value\":" +String(id)+"},{\"tag\":\"UID_HEX\",\"value\":\""+String(id,HEX)+"\"},{\"tag\":\"Reader\",\"value\":"+String(reader)+"},{\"tag\":\"MEDIUM\",\"value\":\""+String("ETHERNET")+"\"}],\"time\":"+String(now.unixtime())+"000}";

    Ethernet_client.println(Data_string);
    long timeout2 = millis();
    Serial.println("Ethernet waiting");
    while (Ethernet_client.available() == 0)
    {
      if (millis() - timeout2 > Server_Timeout)
      {
        Serial.println(F(">>> Client Timeout !"));
        Ethernet_client.stop();
        return 0;
      }
      String str_2 = Ethernet_client.readString();
      Serial.print(str_2);

      if (str_2.indexOf(F("Data Received")) > -1)
      {
        Serial.print("\r\nString Captured : ");
        Serial.println(str_2);
        Ethernet_client.stop();
        return 1;
      }
    }
    Ethernet_client.stop();
  }
  else
  {
    Serial.println("Ethernet connection failed");
    return 0;
  }
}

int Serial_command(void){
  DateTime now = rtc.now();
  if (Serial.available())
  {
    String Serial_str = Serial.readString();
    if ((Serial_str.indexOf("restart") > -1)|(Serial_str.indexOf("RESTART") > -1))
      ESP.restart();
    if ((Serial_str.indexOf("STOP") > -1)|(Serial_str.indexOf("stop") > -1))
      return 0;
    if (Serial_str.indexOf("RTC") > -1)
    {
      Serial.println(Serial_str);
      int rt = Serial_str.indexOf("(") + 1;

      String def;
      for (rt; Serial_str[rt]; rt++)
      {
        if (Serial_str[rt] >= '0' && Serial_str[rt] <= '9')
          def.concat(Serial_str[rt]);
      }
      String dt_str, tm_str;
      for (int i = 0; def[i]; i++)
      {
        if (i < 6)
          dt_str.concat(def[i]);
        else
          tm_str.concat(def[i]);
      }
      Serial.println("Date:" + dt_str);
      Serial.println("Time:" + tm_str);
      long timeVar = tm_str.toInt();
      long dateVar = dt_str.toInt();
      Serial.println(timeVar);

      int yr =  dateVar / 10000;
      int mo = ((int)(dateVar / 100)) % 100;
      int dy =  dateVar % 100;
      int hr = timeVar / 10000;
      int mn = ((int)(timeVar / 100)) % 100;
      int se = timeVar % 100;

      Serial.println(String(yr) + "/" + String(mo) + "/" + String(dy) + " " + String(hr) + ":" + String(mn) + ":" + String(se));
      rtc.adjust(DateTime(yr, mo, dy, hr, mn, se));
      delay(500);
      now = rtc.now();

      Serial.println(String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + ',' + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));

      //  rtc.adjust(DateTime(19,12,25,12,52,00));
      //  RTC(19,12,25,12,52,00)
    }
    if ((Serial_str.indexOf("DELETE") > -1)|(Serial_str.indexOf("delete") > -1))
    {
      SD.remove("/data.txt");
      SD.remove("/prg_ptr.txt");
      Serial.println(F("\r\n\r\n /data.txt and /prg_ptr.txt Files deleted"));
    }
  }
  return 1;
}

void Data_Send(String Data_str,int drive){
  Serial.print("Data_send: ");
  Serial.println(Data_str);
  Read_SD_to_send();
  
  if (Wifi_data_send(Data_str,drive))
  {
    Serial.println("Data Sent through WiFi");      
  }
//    else if(Ehternet_data_send(Data_str))
//    {
//      Serial.println("Data Sent through Ehternet");
////      Read_SD_to_send();
//    }
 
  else
  {
    if(Data_str.indexOf("GetFaultRegisters")>-1)
    {
      return ;
    }
    else
    {
      Save_to_SD(Data_str);
      Serial.println("Data Send Failed !!!");
      Serial.println("Storing Data !!!"); 
    }
  }    
}

int Save_to_SD(String Data_str){  
  if (SD.exists(Data_File)){
    Serial.println(F(" File exists -> Save SD Card Loop"));
    Serial.println(("Appending to file to :")+ String(Data_File));
    File file = SD.open(Data_File, FILE_APPEND);// "FILE_APPEND" FOR SD.H
    if (!file)
      Serial.println(F("Failed to open file for append."));
    Data_str.concat("@\r\n");
    if (file.print(Data_str))
      {Serial.println(F("Data append success"));}
    else
      Serial.println(F("Data append failed"));

    Serial.print("File Size: "); Serial.println(file.size(),DEC);
    Serial.println();
    file.close();

    
   
  }
  else
  {
    Serial.println(F("file not exists -> Writing to file:"));
    File file = SD.open(Data_File, FILE_WRITE);
    if (file){
      Data_str.concat("@\r\n");
      if (file.print(Data_str))
      {  Serial.println(F("Data write success.")); }        
      else
        Serial.println(F("Data write failed"));
    }
    else
      Serial.println(F("Failed to open file for write."));
    Serial.println("File Size: " + String(file.size()));
    file.close();
    
  }
}

int readFile( const char * path){
   Serial.printf("Reading file: %s\r\n", path);
    File file = SD.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return 0;
    }
    Serial.println("- read from file:");
    ReadString="";
    while(file.available()){
      char ch = file.read();
        ReadString.concat(ch);
    }
    file.close();
    Serial.print("\t"); Serial.print(ReadString);
    return 1;
}

void Store_Number (String data_str, const char * path){
  Serial.printf("Storing Number : %s\r\n",path);
  File file = SD.open(path);
   if (file){
      Data_str.concat("@\r\n");
      if (file.print(Data_str))
        Serial.println(F("Data write success."));
      else
        Serial.println(F("Data write failed"));
   }
    Serial.println("File Size: " + String(file.size()));
    file.close();
}

int SD_Wifi_Configuration(void){
   
   readFile("/file_pwd.txt");   
//   file_pwd = "";
   for(int i=0;ReadString[i];i++)
   {
     char ch = ReadString[i];
      if(ch!='\r')
      {
        file_pwd[i]=ch;
      }    
   }     
   Serial.println("wifi pwd : " + String(file_pwd));
   
   readFile("/file_ssid.txt");
//   file_ssid = "";
   for(int i=0;ReadString[i];i++)
   {
     char ch = ReadString[i];
    if(ch!= '\r')
    {
      if(ch=='+')
        file_ssid[i]=' ';
      else
        file_ssid[i]=ch;
    }
   }
   Serial.println("Wifi SSID: " + String(file_ssid)); 

   readFile("/IP.txt");   
//   file_pwd = "";
   for(int i=0;ReadString[i];i++)
   {
     char ch = ReadString[i];
      if(ch!='\r')
      {
        IP[i]=ch;
      }    
   }     
   Serial.println("sever IP : " + String(IP));


   readFile("/PORT.txt");   
   String PORT_str ="";// ReadString.substring(pos1,pos2);
     for(int i=0;ReadString[i];i++)
     {
       char ch = ReadString[i];
      if(ch>='0' & ch<='9')
      {
        PORT_str.concat(ch);
      }
     }
     PORT = PORT_str.toInt();
     Serial.println("PORT: " + String(PORT));

    File pfile = SD.open("/PATH.txt",FILE_READ);
    if(pfile){
      PATH = "";
      while(pfile.available())
      {
        char ch = pfile.read();
        if(ch!='\r' & ch !='\n')
        {
          PATH.concat(ch);
        }
      }
      pfile.close();
      Serial.println("Server Path: " + PATH);
    }
    else{
      Serial.println(F("path.txt not found"));
    }
    
    Serial.print("Connecting to :");delay(5);  
    Serial.println(file_ssid);delay(5);
                                                //  Serial.println(ssid);
                                                //  WiFi.begin(ssid, password);
    WiFi.begin(file_ssid, file_pwd); 
    long timeout=millis();
   
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print("."); 
        if((millis()-timeout)>20000)
          break;
    }
    
    if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
     ip = WiFi.localIP();
    Serial.println(ip);
    }
//    file_ssid = "**LMB-HZW-WCMS**";
//    file_pwd =  "Power@1234";
//    IP = "10.7.74.158";
//    PORT      = 80;
}

int Read_SD_to_send(void){
//  Serial.println(F("Readig SD card"));
  delay(50);
  myFile.close();
  if(!check_connection())  {return 0;}

  if (SD.exists(Data_File))/**/
  {
    if (SD.exists(Ptr_File))
    {
      String ptr = "";
      myFile = SD.open(Ptr_File);
      if (myFile)
      {
        while (myFile.available())
        {
          ptr.concat((char)myFile.read());
        }
        data_ptr = ptr.toInt();
        Serial.println("Pointer position: " + String(data_ptr));
        myFile.close();
      }
      else
      {
        Serial.println("Prg_ptr.txt opening error");
      }
    }

    File  myFile = SD.open(Data_File);
    if (myFile.size() == data_ptr)
    {
      myFile.close(); delay(100);
      SD.remove(Data_File);
      SD.remove(Ptr_File);
      data_ptr = 0;
      Serial.println("File Deleted !!");
      myFile = SD.open("/");
      printDirectory(myFile, 0);       
      return 0;
    }
    
     if (myFile.size() < data_ptr)
    {
      data_ptr=0;
    }
    String Data_Read_str = "";

    myFile.seek(data_ptr);
    while (myFile.available())
    {
      char ch = myFile.read();
      if (ch == '@')
      {
        Data = Data + 1;
        myFile.read();
        myFile.read();                 //send data to serever
//        if(Ehternet_data_send(Data_str))
//        {
//          Serial.println(F("Data Sent -> Ehternet"));
//        }
        if (Wifi_data_send( Data_Read_str,1) == 1)
        {
          Serial.println(F("Data Sent over WiFi"));
        }
        else
        {
          myFile.close();
          Serial.println(F("Client connect failed returning.."));
          return 0;
        }
        Serial.println("Number of Data is : " + String(Data));
        int pos = myFile.position();
        myFile.close();

        Serial.println("Current position is: " + String(pos));
        SD.remove(Ptr_File);
        delay(100);
        
        myFile = SD.open(Ptr_File, FILE_WRITE);
        if (myFile)
        {
          myFile.print(pos);
          myFile.close();
        }
        else
        {
          Serial.println(F("prg_ptr.txt opening Error...."));
        }
        delay(100);
        Serial.println("\r\n* Read & Send SD Program End *\r\n");
        return 1;
      }
      else
      {
        Data_Read_str.concat(ch);
      }
    }
  }
  else
  {
    //    Serial.println(F("Data File does not exists"));
  }
}

void printDirectory(File dir, int numTabs){
  // Begin at the start of the directory
  dir.rewindDirectory();

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');   // we'll have a nice indentation
    }
    // Print the 8.3 name
    Serial.print(entry.name());
//    SD.remove(entry.name());
    // Recurse for directories, otherwise print the file size
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void Prepare_HTML_page(void){
    Serial.println("preparing web page now");
    DateTime now= rtc.now(); 
    HTML="";
    HTML.concat("<!DOCTYPE html>");
    HTML.concat("<html lang=\"en\">");
    HTML.concat("<head>");
    HTML.concat("<title>i Technology </title>");
    HTML.concat("<meta charset=\"UTF-8\">");
    HTML.concat("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    HTML.concat("<style>");
    HTML.concat("table,th,td{border: 1px solid black;border-collapse:collapse;padding:15px;margin-left:auto;margin-right:auto;text-align: center;}");
    HTML.concat("body {font-family: Arial;  margin: 0;}");
    HTML.concat(".header {padding: 60px;text-align:center;background: #000000;color:white;font-size: 20px;}");
    HTML.concat(".content {padding:20px;}</style></head><body onload=\"myFunction();\">");
    HTML.concat("<div class=\"header\">  <h1>i Technology</h1><h3>Machine Operation monitor. </h3>");
    HTML.concat("<h6>" + String(now.day())+"/"+String(now.month())+"/"+String(now.year())+','+String(now.hour())+":"+String(now.minute())+":"+String(now.second())+ " C</h6>");
    HTML.concat("<p> Total Machines : ");
//    HTML.concat(Machines);
    HTML.concat("</p></div>");
//    HTML.concat("</p></div>");
    HTML.concat("<br><br>");    

    HTML.concat("<table style=\"width:50%\"><tr><th>Sr No.</th><th>MACHINE ID</th><th>Machine UP TIME</th><th>DOWN TIME</th></tr>"); 
    
    String person_table = "";
    Serial.println("preparing data in person table");
    
    HTML.concat("<tr>");
    
    HTML.concat("<td>");
    HTML.concat("</td>");
    
    HTML.concat("<td>");
    HTML.concat("</td>");
    
    HTML.concat("<td>");
    HTML.concat("</td>");
    
    HTML.concat("<td>");
    HTML.concat("</td></tr>");
    HTML.concat(person_table + "</table>");
    HTML.concat("<br>");
    HTML.concat("<br><br>");
//    HTML.concat("<a href=\"/O\">Restore</a>  ");
//    HTML.concat("<script>function myFunction() {setTimeout(function(){ window.location.reload();}, 10000);}</script>");
    HTML.concat("<br><br>");
    
    HTML.concat("<form action=\"/\"><fieldset>");
    HTML.concat("<label for=\"file_ssid\">SSID : </label><br>");
    HTML.concat("<input type=\"text\" id=\"file_ssid\" name=\"file_ssid\" value=\"");
    HTML.concat(String(file_ssid));
    HTML.concat("\"><br><br>");    
    
    HTML.concat("<label for=\"file_pwd\">Password : </label><br>");
    HTML.concat("<input type=\"text\" id=\"file_pwd\" name=\"file_pwd\" value=\"");
    HTML.concat(String(file_pwd));
    HTML.concat("\"><br><br>");
    HTML.concat("<input type=\"submit\" value=\"Connect\"></fieldset></form>");
    
    HTML.concat("<br><br>"); 

    HTML.concat("<form action=\"/\"><fieldset>");   
    HTML.concat("<label for=\"ip\">Server IP :</label><br>");
    HTML.concat("<input type=\"text\" id=\"ip\" name=\"ip\" value=\"");
    HTML.concat(String(IP));
    HTML.concat("\"><br><br>");
    
    HTML.concat("<label for=\"port\">Server Port:</label><br>");
    HTML.concat("<input type=\"text\" id=\"port\" name=\"port\" value=\"");
    HTML.concat(String(PORT));
    HTML.concat("\"><br><br>");

    HTML.concat("<input type=\"submit\" value=\"Submit\"></fieldset></form>");

    HTML.concat("<br><br>");

    HTML.concat("<form action=\"/\"><fieldset>");   
    HTML.concat("<label for=\"path\">Server Path :</label><br>");
    HTML.concat("<input type=\"text\" id=\"path\" name=\"path\" value=\"");
    HTML.concat(String(PATH));
    HTML.concat("\"><br><br>");
    
    HTML.concat("<input type=\"submit\" value=\"Submit\"></fieldset></form>");

    HTML.concat("<br><br>");
    

    
    HTML.concat("<footer>");
    HTML.concat("  Mac address  : " + WiFi.macAddress());
    HTML.concat("<br>  local address: ");
    String LocalIP = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
    HTML.concat(LocalIP);
    
    HTML.concat("<br>Developed By: i Technology ");
    HTML.concat("<p><br>itechnology1242@gmail.com");
    HTML.concat("<br>Surat, India</p>");
    HTML.concat("</footer>");
    HTML.concat("</body></html>");
    Serial.println("returning to main loop");
}

void WiFi_RTC_SD_Ethernet_Init(void){
  SD_init();
  LED_init();
//  WIFI_init(ssid,password);
  RTC_init();
  ETHERNET_init();
  Serial.println(F("done!"));
}

void WiFi_RTC_SD_Init(void){
  SD_init();
  LED_init();
//  SD_Wifi_Configuration();
  WIFI_init(ssid,password);
  RTC_init();
  Serial.println(F("done!"));
}

void RTC_SD_Ethernet_Init(void){
  SD_init();
  LED_init();
  RTC_init();
  ETHERNET_init();  
  Serial.println(F("done!"));
}
void check_response_RTC(String str){
        DateTime now = rtc.now();
        String Buff = "";
        int str_len =str.length();
        for(int i=1;i<str_len-1;i++)
        {          
          char ch = str[i];
          if(ch!=0x5C)
            Buff.concat(ch);
        }
        str = "";        str.concat(Buff);        Buff="";
  
        Serial.println("\r\n");
        if(str.indexOf("\"readindTime\":\"")>-1)
        {
            String settime="";
            int ptr = str.indexOf("\"readindTime\":\"")+15;
            for(int i=ptr;str[i]!='.';i++)
            {
              char ch = str[i];//              Serial.print(ch);
              settime.concat(ch);
            }
           String set_hr="",set_mn="",set_sec="",set_date="",set_month="",set_year="";
           int i=ptr;
           for(;str[i];i++)          {
              char ch = str[i];
              if(ch=='-')
                break;//              Serial.print(ch);
              set_year.concat(ch);
            }i++;
           for(;str[i];i++)          {
              char ch = str[i];
              if(ch=='-')
                break;//              Serial.print(ch);
              set_month.concat(ch);
            }i++;
            for(;str[i];i++)          {
              char ch = str[i];
              if(ch=='T')
                break;//              Serial.print(ch);
              set_date.concat(ch);
            }i++;
            
            for(;str[i];i++)          {
              char ch = str[i];
              if(ch==':')
                break;//              Serial.print(ch);
              set_hr.concat(ch);
            }i++;
            for(;str[i];i++)          {
              char ch = str[i];
              if(ch==':')
                break;//              Serial.print(ch);
              set_mn.concat(ch);
            }i++;
            for(;str[i];i++)          {
              char ch = str[i];
              if(ch=='.')
                break;//              Serial.print(ch);
              set_sec.concat(ch);
            }
            Serial.println("SET Parameter: ");
            Serial.print("Date: "); Serial.print(set_year);Serial.print("/");Serial.print(set_month);Serial.print("/");Serial.println(set_date);
            Serial.print("Time: "); Serial.print(set_hr);  Serial.print(":");Serial.print(set_mn);   Serial.print(":");Serial.println(set_sec);
            if(now.minute()==15 | now.minute()==30 | now.minute()==45 | now.minute()==00){
              Serial.println(F("Updating RTC Time"));
              rtc.adjust(DateTime(set_year.toInt(), set_month.toInt(), set_date.toInt(), set_hr.toInt(), set_mn.toInt(), set_sec.toInt()));
            }
        }
}
