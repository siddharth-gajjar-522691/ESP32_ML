//422674
// AT BOILERS A3 Local IP: 10.7.139.120
#include "FS.h"
#include "OTA.h"
#include "SPIFFS.h"
#include "itech_lbr.h"
#include "Input_Pins.h" 
#include "itech_modbus.h"
//#include "crane_info.h"

#define FORMAT_SPIFFS_IF_FAILED true 
#define Saperator "~"
#define TAB       '\t'

//#define REMOVE_FILES

#define Server_Timeout 8000
#define int_Pin 12

#define KWH_timeout 1800
#define OTF_timeout 1800
#define OT_timeout  1800
#define FOT_timeout 1800
#define DCV_timeout 300

float Power_Consumption[4];
float Weight=0;
String Fault_Flag[21] = {"NoF", "Uv1", "Uv2", "GF", "oC", "ov", "oH", "oH1", "oL1", "oL2", "oL3", "oL4", "rr", "rH", "FAn", "os", "dEv", "PGo", "PF", "LF", "oH3"};
String Status_Flag[10] = {"DRun", "DZS", "DRR", "FRS", "DSA", "DR", "ALARM", "Fault", "DOE", "DMPL"};

int LT_status=1;
int CT_status=1;
int MH_status=1;
int AH_status=1;


int fault_flag = 0;
int restart_count = 0;
int Drive_Status = 0;
int Send_Data = 0;
int act_Int = 0;
String Data_String ="";
long time2=0;
int interrupt_send=0;
long check = 0;
int Send_MODBUS_DATA = 0;
long kwh_=0,OTF_=0,OT_=0,FOT_=0,DC_=0;
int testing_flag = 0;


//void IRAM_ATTR LT_Detect() {
//  Serial.println("LT Detected. :)");  delay(500);
//   interrupt_send=1;
//   
////  Prepare_Data(); 
//}

void setup(){
  Serial.begin(9600);
//  //SerialBT.begin(SENSOR_ID); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println(F("Start the Task.."));
  
  delay(100);
  init_pins();
//  attachInterrupt(LT, LT_Detect, CHANGE);
//  attachInterrupt(CT, LT_Detect, CHANGE);
//  attachInterrupt(AH, LT_Detect, CHANGE);
//  attachInterrupt(MH, LT_Detect, CHANGE);
  
//  detachInterrupt(LT);
//  detachInterrupt(CT);
//  detachInterrupt(AH);
//  detachInterrupt(MH);

  digitalWrite(LT,INPUT_PULLUP);
  digitalWrite(CT,INPUT_PULLUP);
  digitalWrite(MH,INPUT_PULLUP);
  digitalWrite(AH,INPUT_PULLUP);

  WiFi_RTC_SD_Init();
 
  delay(500);
  modbus_configuration();
  WiFi.softAP("Crane_Monitor", "qwerty1234");
//  IPAddress IP = WiFi.softAPIP();
//  Serial.print("Setting AP (Access Point)…");  Serial.print("AP IP address: ");  Serial.println(IP);  
 
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());

  server.begin();
#ifdef  REMOVE_FILES
  SD.remove("/data.txt");
  SD.remove("/prg_ptr.txt");
#endif

  init_modbus_packet();
  
  Data_Send(Fautlt_reg_str,1);
  
  LED_init();
  KWH_packets();
  operation_time_from_fault_packets();
  operation_time_packets();
  DC_BUS_VTG(); 
  Fan_operation_time_packets(); 
  ota_setup();
  data_str_send();
  
    LT_status = digitalRead(LT);
    CT_status = digitalRead(CT);
    MH_status = digitalRead(MH);
    AH_status = digitalRead(AH);
}

void loop(){                  //    system_OTA();
  
  Serial.println("\r\nLOOP BEGIN\r\n");//  Client_Function();
  digitalWrite(DATA_PUSH,!digitalRead(DATA_PUSH));
  check_connection();
  ota_loop();
  
  long client_time = millis();   
  Read_Input();

   if(interrupt_send==0){//        Client_Function();
      DateTime now = rtc.now();
      Serial_command();  
//      Modbus_Update();
      if (connection_status != TOTAL_NO_OF_PACKETS){
        Serial.println("FAIL");
        packets[connection_status].connection = true;
      }
      else{
          Serial.println("CONNECTION OK ");
      }
      init_modbus_packet();
      Modbus_Data_Com();    
      Read_Input(); 
   }
/*  
   if(((millis()-kwh_)/30000) > KWH_timeout & interrupt_send==0){
      kwh_ = millis(); 
        KWH_packets();  
    }
    
    Read_Input();    
    if(((millis()-OTF_)/1000) > OTF_timeout & interrupt_send==0)  {
      OTF_ = millis(); 
        operation_time_from_fault_packets();
    }
    
    Read_Input();
    if(((millis()-OT_)/1000) > OT_timeout & interrupt_send==0)    {
      OT_ = millis(); 
        operation_time_packets();
    }
    Read_Input();
    if(((millis()-DC_)/1000) > DCV_timeout & interrupt_send==0)  {
      DC_ = millis();
        DC_BUS_VTG(); 
    }
    Read_Input();
    if(((millis()-FOT_)/1000) > FOT_timeout & interrupt_send==0)  {
      FOT_ = millis();
        Fan_operation_time_packets(); 
    }
    Read_Input();  
*/
    if(fault_flag != Response_fault_2){ 
      fault_find_from_response(); 
      fault_flag = Response_fault_2;
    }

    if((millis()-client_time)/1000>3600){ ESP.restart();}
//    if(restart_count>200){ ESP.restart();}
    Serial.println();
    Serial.println();
    
//     if(now.hour()==)
//      Read_SD_to_send();
    
}

void Read_Input(void){
  Serial.print(String(digitalRead(LT)) + "\t" + String(digitalRead(CT)) + "\t" + String(digitalRead(AH)) + "\t" + String(digitalRead(MH)));
  Serial.println("\t" + String(digitalRead(INPUT_5)) + "\t" + String(digitalRead(INPUT_6)) + "\t" + String(digitalRead(INPUT_7)) + "\t" + String(digitalRead(INPUT_8)));
  Drive_Status = digitalRead(LT)^digitalRead(CT)^digitalRead(AH)^digitalRead(MH);
  DateTime now = rtc.now();
  String time_str = "";
  time_str.concat(String(now.year()));    
  time_str.concat(F("-"));                          time_str.concat(String(now.month()));
  time_str.concat(F("-"));                          time_str.concat(String(now.day()));  
  time_str.concat(F("%20"));                        time_str.concat(String(now.hour())); 
  time_str.concat(F(":"));                          time_str.concat(String(now.minute()));
  time_str.concat(F(":"));                          time_str.concat(String(now.second()));
  Serial.println(time_str);
//  //SerialBT.println("\r\n");
//  //SerialBT.print("ID : ");        //SerialBT.println(SENSOR_ID);
//  //SerialBT.print("Drive: ");      //SerialBT.print(digitalRead(LT));
//                                  //SerialBT.print(digitalRead(CT));
//                                  //SerialBT.print(digitalRead(MH));
//                                  //SerialBT.println(digitalRead(AH));
//  
  if((digitalRead(LT)!= LT_status | digitalRead(CT) != CT_status |digitalRead(AH)!= AH_status | digitalRead(MH) != MH_status)){
    Serial.println(F("Input Detected... :)"));delay(1000);
    LT_status = digitalRead(LT);
    CT_status = digitalRead(CT);
    MH_status = digitalRead(MH);
    AH_status = digitalRead(AH);
    
    init_modbus_packet();
    delay(1500);
    Modbus_Data_Com();
    interrupt_send =1; 
    data_str_send();
    
/*  KWH_packets();
    operation_time_from_fault_packets();
    operation_time_packets();
    DC_BUS_VTG();           
    Fan_operation_time_packets();*/
  }
  
  if(((millis()-time2)/1000)>60 | interrupt_send==1){
        time2=millis();
        interrupt_send=0;
        data_str_send();
        //http://192.168.1.100/Boiler_CMS/Home/SaveDriverReading?driverReading={"id":0,"crane_id":1,"driver_id":1,"on_off":true,"output_freq":10.31,"ac_voltage":10.2,"output_current":1.5,"dc_voltage":5.5,"heat_sink":true,"reading_time":null,"system_time":null,"fault":false,"driverReading":0}
        
        String path_addr = "GET /Boiler_CMS/Home/SaveDriverReading?driverReading="; 
        Data_Send((String(path_addr) + String(Drive_1_status)),1);  
        Data_Send((String(path_addr) + String(Drive_2_status)),2);  if(Response_fault_2 ==1)fault_find_from_response();
        Data_Send((String(path_addr) + String(Drive_3_status)),3);  if(Response_fault_2 ==1)fault_find_from_response();
        Data_Send((String(path_addr) + String(Drive_4_status)),4);  if(Response_fault_2 ==1)fault_find_from_response();
        
        Drive_1_status = "";  Drive_2_status = "";Drive_3_status = "";Drive_4_status = "";
        restart_count++;
        Serial.println("Restart : "+ String(restart_count));
       
      }
}

String fault_str_prepare(int crane_id,int fault_drive,int value,long reading_id,int Fault_1)
{
  Serial.println("Fault Addr : " + String(Fault_1));
  Serial.println("Faulty Drive: "+ String(fault_drive));
  String Fault_Register_str = "GET /Boiler_CMS/Home/SaveFaultReading?faultReading="
                              "{\"craneId\":"                +String(crane_id)    +        
                              ",\"driverId\":"               +String(fault_drive) +
                              ",\"driver_reading_id\":"      +String(reading_id)  +
                              ",\"value\":"                  +String(value)       +
                              ",\"fault_register_Address\":" +String(Fault_1)     + "}";  
  return Fault_Register_str;
}

void fault_find_from_response(void)
{ 
      Serial.println("//**************************** FAULTS ****************************//");
      String Fault_Register_str = "";
      check_response_fault(Fault_1);
          if(fault_drive_LT){  value = buffer_reg1[0];  Data_Send(fault_str_prepare(crane_id,1, value,driver_reading_id_LT, Fault_1),D1);}
          if(fault_drive_CT){  value = buffer_reg2[0];  Data_Send(fault_str_prepare(crane_id,2, value,driver_reading_id_CT, Fault_1),D2);}
          if(fault_drive_MH){  value = buffer_reg3[0];  Data_Send(fault_str_prepare(crane_id,3, value,driver_reading_id_MH, Fault_1),D3);}
          if(fault_drive_AH){  value = buffer_reg4[0];  Data_Send(fault_str_prepare(crane_id,4, value,driver_reading_id_AH, Fault_1),D4);}       
          
      check_response_fault(Fault_1);
          if(fault_drive_LT){  value = buffer_reg1[1];  Data_Send(fault_str_prepare(crane_id,1, value,driver_reading_id_LT, Fault_2),D1);}
          if(fault_drive_CT){  value = buffer_reg2[1];  Data_Send(fault_str_prepare(crane_id,2, value,driver_reading_id_CT, Fault_2),D2);}
          if(fault_drive_MH){  value = buffer_reg3[1];  Data_Send(fault_str_prepare(crane_id,3, value,driver_reading_id_MH, Fault_2),D3);}
          if(fault_drive_AH){  value = buffer_reg4[1];  Data_Send(fault_str_prepare(crane_id,4, value,driver_reading_id_AH, Fault_2),D4);}       
          Response_fault_2 =0; 
}


//void Prepare_Data(void)
//{
//  ///Sensor/Save?ManualSetting=true&AutoSetting=true&SetupSwitch=false&DeadManSwithc=false&currentInput=0.11&sensorId=CLOOS_ROBOT_MONITOR&sensorInputTime=202165-165-165 165:165:85
//  DateTime now = rtc.now();
//  Data_str = "";
//  Data_str.concat(PATH);
//  Data_str.concat("LT=");
//  Data_str.concat(digitalRead(LT)?"RUN":"STOP");
//  Data_str.concat(F("&CT="));                       Data_str.concat(digitalRead(CT)?"RUN":"STOP");
//  Data_str.concat(F("&MH="));                       Data_str.concat(digitalRead(MH)?"RUN":"STOP");
//  Data_str.concat(F("&AH="));                       Data_str.concat(digitalRead(AH)?"RUN":"STOP");
//  Data_str.concat("LTFault=" + String(Fault_Flag[LT_Fault]));
//  Data_str.concat("CTFault=" + String(Fault_Flag[CT_Fault]));
//  Data_str.concat("MHFault=" + String(Fault_Flag[MH_Fault]));
//  Data_str.concat("Weight=" + String(Weight));
//  Data_str.concat(F("&sensorInputTime="));          Data_str.concat(String(now.year()));    
//  Data_str.concat(F("-"));                          Data_str.concat(String(now.month()));
//  Data_str.concat(F("-"));                          Data_str.concat(String(now.day()));  
//  Data_str.concat(F("%20"));                        Data_str.concat(String(now.hour())); 
//  Data_str.concat(F(":"));                          Data_str.concat(String(now.minute()));
//  Data_str.concat(F(":"));                          Data_str.concat(String(now.second()));
//  Serial.println(Data_str);
//}
