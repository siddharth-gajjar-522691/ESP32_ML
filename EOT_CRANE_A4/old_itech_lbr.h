//Sample Data String
//http://172.16.129.13/portal/HZW_IOT_Zayer/main/capture/SEN_ID~Current~Idle~Cutting~Breakdown~Time
//http://172.16.129.13/portal/HZW_IOT_Zayer/main/capture/zayer~1~1~1~1~1~1~1559304677"; 

#define Server_Timeout 3000
#define Wifi_LED      2
#define Data_Push     15
#define Response      "OK"

#include <WiFi.h>
#include <WiFiClient.h>
#include <RTClib.h>
#include "time.h"
#include <SPI.h>
#include <SD.h>
#include <DS3232RTC.h>

WiFiClient client; 
RTC_DS3231 rtc;
DS3232RTC RTC;
File myFile; 

char* Data_File = "/crane.txt";
char* Ptr_File  = "/prg_ptr.txt";
long data_ptr = 0;
int Data=0;

long restart_time =0;
//const char* ssid     = "WiFi_Digital";
//const char* password = "W!F!@DiGi";
//const char* IP       = "172.27.1.234";
//const int  PORT      = 9191;

//const char* ssid     = "Dwij";
//const char* password = "qwerty1234";
//const char* IP       = "192.168.43.96";
//const int  PORT      = 9191;

const char* ssid     = "i Technology";
const char* password = "qwerty1234";
const char* IP       = "192.168.1.100";
const int  PORT      = 9191;

void printDirectory(File dir, int numTabs);
void config_system(void);
int Wifi_data_send(String Data_str);
void Data_Send(String Data_str);
void Serial_command(void);
int check_connection(void);
int Save_to_SD(String Data_str);
int Read_SD_to_send(void);

void WiFi_RTC_SD_Init(void){
  if (!SD.begin(5))// if (!SD.begin(5, 23, 19, 18)) //
  {
    Serial.println(F("SD CARD FAILED!"));
  }
  else
  {
    Serial.println(F("SD CARD OK."));
  };
  myFile = SD.open("/");
  printDirectory(myFile, 0);
  pinMode(Wifi_LED,    OUTPUT);
  pinMode(Data_Push, OUTPUT);

  Serial.print(" Wifi Mac Address: ");  Serial.println(WiFi.macAddress());
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
  Serial.println(ssid);

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
  }
  int count = 0;
  long  timecount = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if ((millis() - timecount) > 5000)
      break;
  }
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  rtc.begin();
  RTC.begin();
  DateTime now = rtc.now();
  Serial.println(String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + ',' + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
   myFile = SD.open(Data_File);
  if (myFile) {
    Serial.println(Data_File);

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening");
  }
}

int check_connection(void){
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    digitalWrite(Wifi_LED, HIGH);
    Serial.println("WIFI : NO\t");
    if((millis()-restart_time)/1000>120)
    {
      ESP.restart();
    }
    return 0;
  }
  else
  {
    restart_time=millis();
    digitalWrite(Wifi_LED, LOW);
    Serial.println("WIFI : YES \t");
    return 1;
  }
}

int Wifi_data_send(String Data_str){ 
  
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Connected.");
    if (client.connect(IP, PORT) | client.available())
    {
      Serial.print(F("Server Connected, Pushing local data to server: "));
      Serial.print("Connecting to :");
      Serial.println(client.remoteIP());
      client.println(Data_str);
         
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
        
        if (str.indexOf(F(Response)) > -1)
        {
          Serial.print("\r\nString Captured : ");
          Serial.println(str);
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

void Serial_command(void){
  DateTime now = rtc.now();
  if (Serial.available())
  {
    String Serial_str = Serial.readString();
    if ((Serial_str.indexOf("restart") > -1)|(Serial_str.indexOf("RESTART") > -1))
      ESP.restart();
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
  }
}

void Data_Send(String Data_str){
    if (Wifi_data_send(Data_str))
    {
      Serial.println("Data Sent through WiFi");
      Read_SD_to_send();
       digitalWrite(Data_Push, !digitalRead(Data_Push));
    }
    else
    {
      Save_to_SD(Data_str);
      Serial.println("Data Send Failed !!!");
      Serial.println("Storing Data !!!");
      digitalWrite(Data_Push, HIGH); 
    }
    
}

int Save_to_SD(String Data_str){  
  if (SD.exists(Data_File)){
    Serial.println(F(" File exists -> Save SD Card Loop"));
    Serial.println(F("Appending to file:"));
    File file = SD.open(Data_File, FILE_APPEND);
    if (!file)
      Serial.println(F("Failed to open file for append."));
    Data_str.concat("@\r\n");
    if (file.print(Data_str))
      Serial.println(F("Data append success"));
    else
      Serial.println(F("Data append failed"));

    Serial.println("File Size: " + String(file.size()));
    file.close();
  }
  else
  {
    Serial.println(F("file not exists -> Writing to file:"));
    File file = SD.open(Data_File, FILE_WRITE);
    if (file){
      Data_str.concat("@\r\n");
      if (file.print(Data_str))
        Serial.println(F("Data write success."));
      else
        Serial.println(F("Data write failed"));
    }
    else
      Serial.println(F("Failed to open file for write."));
    Serial.println("File Size: " + String(file.size()));
    file.close();
  }
}

int Read_SD_to_send(void){ //  Serial.println(F("Readig SD card"));
  delay(50);
  myFile.close();
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
        if (Wifi_data_send( Data_Read_str ) == 1)
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
          Serial.println("prg_ptr.txt opening Error....");
        }
        delay(100);
        Serial.println("\r\n*********************** Read & Send SD Program End ***********************\r\n");
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

void printDirectory(File dir, int numTabs)
{
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
