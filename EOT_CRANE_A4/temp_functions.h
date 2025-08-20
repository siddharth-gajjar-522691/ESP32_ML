 /*
 http://192.168.1.100/Boiler_CMS/Home/GetFaultRegisters
 {"errorMessage":null,"responseStatus":"OK","responseMessage":{"FAULT_REGISTERS":[192,193]}}
 
 http://192.168.1.100/Boiler_CMS/Home/SaveFaultReading?faultReading={"craneId":1,"driverId":1,"driver_reading_id":1,"value":1637,"fault_register_Address":192}
 {"errorMessage":null,"responseStatus":"OK","responseMessage":{}}
 
 http://192.168.1.100/Boiler_CMS/Home/SaveDriverReading?driverReading={"id":0,"crane_id":1,"driver_id":1,"on_off":true,"output_freq":10.31,"ac_voltage":10.2,"output_current":1.5,"dc_voltage":5.5,"heat_sink":true,"reading_time":null,"system_time":null,"fault":false,"driverReading":0}
  * {"errorMessage":null,"responseStatus":"OK","responseMessage":{"id":9}}
  * 
  * 
  * 
Packet packets[TOTAL_NO_OF_PACKETS];
//DRIVE RUN
packetPointer packet1 = &packets[PACKET1];
packetPointer packet2 = &packets[PACKET2];
packetPointer packet3 = &packets[PACKET3];
packetPointer packet4 = &packets[PACKET4];
// FAULTS
packetPointer packet5 = &packets[PACKET5];
packetPointer packet6 = &packets[PACKET6];
packetPointer packet7 = &packets[PACKET7];
packetPointer packet8 = &packets[PACKET8];
//POWER CONSUMPTION
packetPointer packet9 = &packets[PACKET9];
packetPointer packet10 = &packets[PACKET10];
packetPointer packet11 = &packets[PACKET11];
packetPointer packet12 = &packets[PACKET12];


    if(millis()-tum>1000)
    { 
      Serial.println(F("Reading Modbus"));     
      tum=millis();
      count++;
      if(count>=3)
      {
        count=0;
      }            
      
      packet1->id = 2;
      packet1->function = READ_HOLDING_REGISTERS;
      packet1->no_of_registers = 1;
      packet1->register_array = regs;

      if(count==0)
        packet1->address = 32;
      else if(count==1)
        packet1->address = 64;
      else if(count==2)
        packet1->address = 93;
        
      Serial2.begin(baud,SERIAL_8E1);
      modbus_configure(baud, timeout_modbus, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
      delay(100);
    }
    delay(100);
    unsigned int connection_status = modbus_update(packets);
    delay(100);
    if(millis()-display_time > 1000)
    {
      display_time = millis();
      if (connection_status != TOTAL_NO_OF_PACKETS)
      {
        digitalWrite(connection_error_led, LOW);
        // You could re-enable the connection by:
        //packets[connection_status].connection = true;
      }
      else
      {
        digitalWrite(connection_error_led, HIGH);
      }
      
      Serial.print("packet1->address: " + String(packet1->address));Serial.print('\t');
      Serial.print(regs[0],BIN);Serial.print('\t');Serial.print(regs[0],DEC);Serial.print('\t');
      
      if(packet1->id==2)
      {
        if(packet1->address==32)
        {
          if(regs[0] & 0B0001000)
          {
            Serial.print(String(packet1->id)+ ": There is fault");
            MH_Fault=1;
          }
          else
          {
            Serial.print(String(packet1->id)+ ": No fault");
            MH_Fault=0;
          }
          Serial.print('\t');
          
          if(regs[0] & 0B0000000000000001)
          {
            Serial.println(String(packet1->id)+ ": Drive Run");
            MH_OT=1;
          }
          else
          {
            Serial.println(String(packet1->id)+ ": Drive Stop");
            MH_OT=0;
          }        
        }
        else if(packet1->address==64)
        {
              Serial.print(String(packet1->id)+ ": Freq: ");Serial.print(regs[0]);
        }
        else if(packet1->address==93)
        {// KWH
              Serial.print(String(packet1->id)+ ": KWH: ");Serial.print(regs[0]);
        }
        Serial.println();
      }
    }*/


    /*
     * 
     * 
//  for(int i=0;ids[i];i++)         // increment id 
//  {
//    packet1->function = READ_HOLDING_REGISTERS;
//    packet1->id = ids[i];
      
//    for(int j=0;Addr[j];j++)      // Address toggle 
//    {       
//      packet1->address = Addr[j];
//      packet1->no_of_registers = ReadNo[j];
//      packet1->register_array = regs;      
//      Serial2.begin(baud,SERIAL_8E1);
//      modbus_configure(baud, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
//      delay(100);

//      Serial.println(regs[0]);
//      if(Addr[j]==192)
//      {
////        Serial.print(Addr[j]+'\t');Serial.print(regs[0],BIN);Serial.print('\t');Serial.println(regs[1],BIN);  
//        if((regs[0]& 0B1111111111100110)|(regs[1] & 0B0001111111000000))
//          {
//                 if(regs[0] & 0B0000000000000010){    Drive_Fault[i]=1;}
//            else if(regs[0] & 0B0000000000000100){    Drive_Fault[i]=2;}
//            else if(regs[0] & 0B0000000000100000){    Drive_Fault[i]=3;}
//            else if(regs[0] & 0B0000000001000000){    Drive_Fault[i]=4;}
//            else if(regs[0] & 0B0000000010000000){    Drive_Fault[i]=5;}
//            else if(regs[0] & 0B0000000100000000){    Drive_Fault[i]=6;}
//            else if(regs[0] & 0B0000001000000000){    Drive_Fault[i]=7;}
//            else if(regs[0] & 0B0000010000000000){    Drive_Fault[i]=8;}
//            else if(regs[0] & 0B0000100000000000){    Drive_Fault[i]=9;}
//            else if(regs[0] & 0B0001000000000000){    Drive_Fault[i]=10;}
//            else if(regs[0] & 0B0010000000000000){    Drive_Fault[i]=11;}
//            else if(regs[0] & 0B0100000000000000){    Drive_Fault[i]=12;}
//            else if(regs[0] & 0B1000000000000000){    Drive_Fault[i]=13;}
//            else if(regs[1] & 0B0000000001000000){    Drive_Fault[i]=14;}
//            else if(regs[1] & 0B0000000010000000){    Drive_Fault[i]=15;}
//            else if(regs[1] & 0B0000000100000000){    Drive_Fault[i]=16;}
//            else if(regs[1] & 0B0000001000000000){    Drive_Fault[i]=17;}
//            else if(regs[1] & 0B0000010000000000){    Drive_Fault[i]=18;}
//            else if(regs[1] & 0B0000100000000000){    Drive_Fault[i]=19;}
//            else if(regs[1] & 0B0001000000000000){    Drive_Fault[i]=20;}
//            Serial.print(String(packet1->id)+ "fault" + Drive_Fault[i]);
////            Drive_Fault[i]=Fault;
//          }
//          else
//          {
//            Drive_Fault[i]=0;
//          }          
//      }
//      if(Addr[j]==32)
//      {
//        Serial.print(Addr[j]);Serial.print('\t');Serial.print(regs[0],BIN);Serial.print('\t');
//        if(regs[0] & 0B0000000000000001)
//          {
//            Serial.println('\t' + String(packet1->id)+ ": Drive Run");
//            if(Drive_Run[i]==0)
//            {
//              Drive_Run[i]=1;
//              Send_MODBUS_DATA=1;
//            }
//          }
//          else
//          {
//            Serial.println('\t' + String(packet1->id)+ ": Drive Stop");
//            Drive_Run[i]=0;
//            if(Drive_Run[i]==1)
//            {
//              Drive_Run[i]=0;
//              Send_MODBUS_DATA=1;
//            }
//          }
//      }
      
//      if(Addr[j]==92)
//      {
//        Power_Consumption[i] = regs[0];
////      }
////    }
////  }
//  
//  if(!(MH_OT==Drive_Run[0]| AH_OT==Drive_Run[1]| CT_OT==Drive_Run[2]|
//    LT_OT==Drive_Run[3] | MH_Fault==Drive_Fault[0] | AH_Fault==Drive_Fault[1] |
//    CT_Fault==Drive_Fault[2] | LT_Fault==Drive_Fault[3]))
//  {
//    MH_OT=Drive_Run[0];
//    AH_OT=Drive_Run[1];
//    CT_OT=Drive_Run[2];
//    LT_OT=Drive_Run[3];
//    MH_Fault=Drive_Fault[0];
//    AH_Fault=Drive_Fault[1];
//    CT_Fault=Drive_Fault[2];
//    LT_Fault=Drive_Fault[3];
//    Send_MODBUS_DATA=1;
//  }
//    
//  for(int d=0;d<4;d++)
//  {
//    Serial.print("id: ");           Serial.print(ids[d]);               delay(5);
//    Serial.print('\t');  Serial.print(" Drive: ");  Serial.print(Drive_Run[d]);         delay(5);
//    Serial.print('\t');  Serial.print(" Fault: ");  Serial.print(Drive_Fault[d]);       delay(5);  
//    Serial.print('\t');  Serial.print(" Power: ");  Serial.println(Power_Consumption[d]); delay(5);    
//  } 
     */

     
/*
void Client_Function(void){
  WiFiClient client = server.available();
//  server.handleClient();
  String header = "";
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    long client_timeout = millis();
    while (client.connected()){            
      if (client.available()){              
        char c = client.read();             // read a byte, then
//        Serial.write(c);                    // print it out the serial monitor
        header.concat(String(c)); 
        if(c == '\n'){
          if (currentLine.length() == 0){
            Prepare_HTML_page();     //           Serial.println(HTML);
            delay(1000);
            client.println(HTML);
            client.println();
            client.stop();
            Serial.println("client disconnected");
            break;
          }
          else {                    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r'){
          currentLine += c;      // add it to the end of the currentLine
        }
        

        if(currentLine.endsWith("GET /S")){
          Serial.println(currentLine);
          //send SMS from web page
          Serial.println("SMS Flag Set to send... :)");
//          msg_sent = 1;
        }
//          file_ssid=file_ssid&file_pwd=file_pwd
          if(currentLine.endsWith("HTTP")){          
            Serial.println(currentLine);
            
          if(currentLine.indexOf("ip=")>-1){
            int pos1 = currentLine.indexOf("ip=")+3;
            int pos2 = currentLine.indexOf("&p");
            String valuestring = "";//"+91";
            valuestring.concat(currentLine.substring(pos1,pos2));
            valuestring.concat("\r\n");
            SD.remove("/IP.txt");
            File myF = SD.open("/IP.txt",FILE_WRITE);
              if (myF.print(valuestring))
              {
                Serial.println(F("Data write success."));
                myF.close();
              }
              else
                Serial.println(F("Data write failed"));     
            Serial.print("IP : " + String(valuestring));           
            Serial.println(">>\r\n");
            readFile("/IP.txt");
            Serial.println(ReadString);
            myF.close();
          }
          
          if(currentLine.indexOf("port=")>-1){
            int pos1 = currentLine.indexOf("port=")+5;
            int pos2 = currentLine.indexOf(" HT");
            String valuestring = currentLine.substring(pos1,pos2);
            valuestring.concat("\r\n");
            SD.remove("/PORT.txt");
//            Store_Number(valuestring,"/PORT.txt"); 
             File myF = SD.open("/PORT.txt",FILE_WRITE);
              if (myF.print(valuestring))
              {
                Serial.println(F("Data write success."));
                myF.close();
              }                
              else
                Serial.println(F("Data write failed"));
                               
            Serial.print("PORT : " + String(valuestring));           
            Serial.println(">>\r\n");
            readFile("/PORT.txt");
            Serial.println(ReadString);
            ESP.restart();
//            Read_Data_SD();
          }
          
            if(currentLine.indexOf("file_ssid=")>-1){
              #define CLIENT
              int pos1 = currentLine.indexOf("file_ssid")+10;
              int pos2 = currentLine.indexOf("&file_pwd");
              String valuestring = currentLine.substring(pos1,pos2);
              valuestring.concat("\r\n");
              SD.remove("/file_ssid.txt");              
              delay(100);
              File myF = SD.open("/file_ssid.txt",FILE_WRITE);
              if (myF.print(valuestring))
                Serial.println(F("Data write success."));
              else
                Serial.println(F("Data write failed"));
              
//              Store_Number(valuestring,"/file_ssid.txt");                   
              Serial.print("Wifi SSID : " + String(valuestring));           
              Serial.println(">>\r\n");
              myF.close();
            }
            if(currentLine.indexOf("file_pwd")>-1){
              int pos1 = currentLine.indexOf("&file_pwd=")+10;
              int pos2 = currentLine.indexOf(" HT");
              String valuestring = currentLine.substring(pos1,pos2);
              valuestring.concat("\r\n");
              
              SD.remove("/file_pwd.txt");              
              delay(100);
              File myF = SD.open("/file_pwd.txt",FILE_WRITE);
              if (myF.print(valuestring))
                Serial.println(F("Data write success."));
              else
                Serial.println(F("Data write failed"));
              
  //            Store_Number(valuestring,"/file_pwd.txt");
              Serial.print("Wifi PWD : " + String(valuestring));           
              Serial.println(">>\r\n"); 
              myF.close();
              ESP.restart();
            } 
           
            if(currentLine.indexOf("path=")>-1){
              int pos1 = currentLine.indexOf("path=")+5;
              int pos2 = currentLine.indexOf(" HT");
              String valuestring = "";//"+91";
              valuestring.concat(currentLine.substring(pos1,pos2));
              valuestring.concat("\r\n");
              valuestring.replace("%2F", "/");
              SD.remove("/PATH.txt");
              File myF = SD.open("/PATH.txt",FILE_WRITE);
                if (myF.print(valuestring))
                {
                  Serial.println(F("Data write success."));
                  myF.close();
                }
                else
                  Serial.println(F("Data write failed"));     
              Serial.print("PATH : " + String(valuestring));           
              Serial.println(">>\r\n");
              readFile("/PATH.txt");
              Serial.println(ReadString);
              myF.close();
            }
           
          }
      }
      if(millis()-client_timeout>10000)
      {
        break;
      }
    }
//    String str= header.readStringUntil("\r\n");
//    Serial.println(header);
    header="";
    client.stop();
    Serial.println(".Client Disconnected."); 
  }
}
*/
