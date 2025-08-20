#include <ArduinoJson.h>
//#include "crane_info.h"
//#define baud 19200

#define YASKAWA 1
#define LNT_FLE 2

unsigned int LT_type = YASKAWA ;
unsigned int CT_type = YASKAWA ;
unsigned int MH_type = YASKAWA ;
unsigned int AH_type = LNT_FLE ;

#define Device_id 0

#define D1  1
#define D2  2
#define D3  3
#define D4  4

#define LT_DC_BUS   0x0314
#define LT_OP_AMP   0x0008
#define LT_AC_Vtg   0x000A
#define LT_OP_Frq   0x0009
#define LT_PO_KWH   0x032D
#define LT_O_Time   0x0342
#define LT_ST_REG   0x0304  //ON-OFF
#define LT_OTIME_FF 0x0344
#define LT_F_RESET  0X0382
#define LT_FAULT_R  0x0329
#define LT_FAN_OP   0x0344

int LNT_DRIVE_ADDR[10]={
                      LT_DC_BUS,
                      LT_OP_AMP,
                      LT_AC_Vtg,
                      LT_OP_Frq,
                      LT_PO_KWH,
                      LT_O_Time,
                      LT_ST_REG,
                      LT_OTIME_FF,
                      LT_F_RESET,
                      LT_FAULT_R
                      };
                      

#define YK_DC_BUS      49
#define YK_OP_AMP       0
#define YK_AC_Vtg       0
#define YK_OP_Frq       0
#define YK_PO_KWH      92
#define YK_O_TIME      76
#define YK_ST_REG      32
#define YK_O_TIME_FF   141
#define YK_F_RESET      0
#define YK_FAULT_R     192        
#define YK_FAN_OP      154


int YAS_DRIVE_ADDR[10]={ 
                        YK_DC_BUS,
                        YK_OP_AMP,
                        YK_AC_Vtg,
                        YK_OP_Frq,
                        YK_PO_KWH,
                        YK_O_TIME,
                        YK_ST_REG,
                        YK_O_TIME_FF,
                        YK_F_RESET,
                        YK_FAULT_R
                      };

//YK_O_TIME_FF
#define Status_Read    16
#define kwh_read       2

#define Modbus_update_loop  22


//int LT_RS485_REG_ADDR[];
//int CT_RS485_REG_ADDR[];
//int MH_RS485_REG_ADDR[];
//int AH_RS485_REG_ADDR[];

char crane_id =1;

int fault_drive_LT=0;
int fault_drive_CT=0;
int fault_drive_MH=0;
int fault_drive_AH=0;

int value =0;
unsigned int Response_fault_2 =0;
#define modbus_timeout 3000
#define polling 1000  

#define retry_count 10 
#define TxEnablePin 27
//#define Status_Reg  75

#define Fault_Reg   192
#define Fault_Read  16

//DC bus voltage
#define DC_bus_voltage 49

//Power consumption
#define KWH_L 0x005C
#define KWH_H 0x005D

unsigned int KWH1[2];
unsigned int KWH2[2];
unsigned int KWH3[2];
unsigned int KWH4[2];

unsigned int otf1[2];
unsigned int otf2[2];
unsigned int otf3[2];
unsigned int otf4[2];

unsigned int ot1[2];
unsigned int ot2[2];
unsigned int ot3[2];
unsigned int ot4[2];

unsigned int fot1[2];
unsigned int fot2[2];
unsigned int fot3[2];
unsigned int fot4[2];

unsigned int DCV1[2];
unsigned int DCV2[2];
unsigned int DCV3[2];
unsigned int DCV4[2];


//cumulative operation time
#define A_COT 0x004C
#define F_COT 0x008D

#include "SimpleModbusMaster.h"

String Drive_1_fault="";
String Drive_2_fault="";
String Drive_3_fault="";
String Drive_4_fault="";

String Drive_1_status="";
String Drive_2_status="";
String Drive_3_status="";
String Drive_4_status="";
unsigned int connection_status = 0;

enum
{
  PACKET1,
//  PACKET2,
//  PACKET3,
//  PACKET4,
  TOTAL_NO_OF_PACKETS
};

Packet packets[TOTAL_NO_OF_PACKETS];

packetPointer packet1 = &packets[PACKET1];
//packetPointer packet2 = &packets[PACKET2];
//packetPointer packet3 = &packets[PACKET3];
//packetPointer packet4 = &packets[PACKET4];


//String Prepare_Data_String(int id, char crane_id,int driver_id,int run_,int Drive_Output_freq,int Drive_output_vtg,int Drive_output_crnt,int DC_vtg,int Power,int Heat_Sink,int _Fault,int Drive_status);
String Prepare_Data_String(int id,  char crane_id,int driver_id,int run_,int Drive_output_vtg,int Drive_output_crnt,int Power,int Heat_Sink, int _Fault,int Drive_status);
void KWH_packets(void);
void operation_time_from_fault_packets(void);
void operation_time_packets(void);
void init_modbus_packet(void);
void Fault_packet_set(void);
void Fan_operation_time_packets(void);
int find_faults(int regC0, int regC1);
int status_faults(void);
void modbus_configuration(void);
 int Modbus_Update(void);
void Modbus_Data_Com(void);
void data_str_send(void);



void modbus(void)
{
  delay(10);
  for(int k=0;k<Modbus_update_loop;k++)
  {
    connection_status = modbus_update(packets);
    delay(50);
  }
}

void check_response_fault(int Addr){
   Serial2.begin(19200,SERIAL_8E1);  
  packet1->id = D1; 
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Addr;
  packet1->no_of_registers = 16;
  packet1->register_array = buffer_reg1;
  modbus();

  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Addr;
  packet1->no_of_registers = 16;
  packet1->register_array = buffer_reg2;
  modbus();
  
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Addr;
  packet1->no_of_registers = 16;
  packet1->register_array = buffer_reg3;
  modbus();
  
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Addr;//Addr; //
  packet1->no_of_registers = 16;
  packet1->register_array = buffer_reg4;
  modbus();
  
//  delay(10);
//  for(int k=0;k<Modbus_update_loop;k++)
//  {
//    connection_status = modbus_update(packets);
//    delay(50);
//  }

  Serial.println("Buffer Read : ");
  for(int i=0;i<16;i++){
   Serial.print(buffer_reg1[i]);Serial.print('\t');Serial.print(buffer_reg2[i]);Serial.print('\t');Serial.print(buffer_reg3[i]);Serial.print('\t');Serial.print(buffer_reg4[i]);Serial.println();
  }
  Serial.println();
  
//  http://localhost:50903/Boiler_CMS/Home/SaveFaultReading?faultReading={"craneId":1,"driverId":1,"driver_reading_id":1,"value":1637,"fault_register_Address":192}
//  http://192.168.1.100/Boiler_CMS/Home/SaveDriverReading?driverReading={"id":0,"crane_id":1,"driver_id":1,"on_off":true,"output_freq":10.31,"ac_voltage":10.2,"output_current":1.5,"dc_voltage":5.5,"heat_sink":true,"reading_time":null,"system_time":null,"fault":false,"driverReading":0}

//  String Data_str = "GET /Boiler_CMS/Home/SaveFaultReading?faultReading="
//                    "{\"craneId\":" +String(1)+
//                    ",\"driverId\":" + String(1)+
//                    ",\"driver_reading_id\":"+String(1)+
//                    ",\"value\":"+String(1637)+
//                    ",\"fault_register_Address\":" +String(192) + 
//                    "}";
  
}

void KWH_packets(void)
{
  Serial2.begin(19200,SERIAL_8E1);  
  packet1->id = D1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_PO_KWH;
  packet1->no_of_registers = kwh_read;
  packet1->register_array = KWH1;
  modbus();
  
  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_PO_KWH;
  packet1->no_of_registers = kwh_read;
  packet1->register_array = KWH2;
  modbus();
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_PO_KWH;
  packet1->no_of_registers = kwh_read;
  packet1->register_array = KWH3;
  modbus();
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_PO_KWH; //  YK_PO_KWH;//
  packet1->no_of_registers = kwh_read;
  packet1->register_array = KWH4;
  modbus();
//  delay(10);
//  for(int k=0;k<Modbus_update_loop;k++)
//  {
//    connection_status = modbus_update(packets);
//    delay(50);
//  }

  Serial.print("\nKWH 1 : " +String(KWH1[1])+String(KWH1[0]));
  Serial.print("\tKWH 2 : " +String(KWH2[1])+String(KWH2[0]));
  Serial.print("\tKWH 3 : " +String(KWH3[1])+String(KWH3[0]));
  Serial.println("\tKWH 4 : " +String(KWH4[1])+String(KWH4[0]*0.1));
  Serial.println("");
  
//  //SerialBT.print("\nKWH 1 : " +String(KWH1[1])+String(KWH1[0]));
//  SerialBT.print("\tKWH 2 : " +String(KWH2[1])+String(KWH2[0]));
//  SerialBT.print("\tKWH 3 : " +String(KWH3[1])+String(KWH3[0]));
//  SerialBT.println("\tKWH 4 : " +String(KWH4[1])+String(KWH4[0]));
}

void DC_BUS_VTG(void)
{
  Serial2.begin(19200,SERIAL_8E1);  
  packet1->id = D1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_DC_BUS;
  packet1->no_of_registers = 2;
  packet1->register_array = DCV1;
modbus();
  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_DC_BUS;
  packet1->no_of_registers = 2;
  packet1->register_array = DCV2;
  modbus();
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_DC_BUS;
  packet1->no_of_registers = 2;
  packet1->register_array = DCV3;
modbus();
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_DC_BUS; //YK_DC_BUS;//
  packet1->no_of_registers = 2;
  packet1->register_array = DCV4;
modbus();
//  delay(10);
//  for(int k=0;k<10;k++)
//  {
//    connection_status = modbus_update(packets);
//    delay(50);
//  }

  Serial.print("\nDCV 1 : " +String(DCV1[0]));
  Serial.print("\tDCV 2 : " +String(DCV2[0]));
  Serial.print("\tDCV 3 : " +String(DCV3[0]));
  Serial.print("\tDCV 4 : " +String(DCV4[0]));
  Serial.println("\r\n");
return;
  //SerialBT.print("\nDCV 1 : " +String(DCV1[0]));
  //SerialBT.print("\tDCV 2 : " +String(DCV2[0]));
  //SerialBT.print("\tDCV 3 : " +String(DCV3[0]));
  //SerialBT.print("\tDCV 4 : " +String(DCV4[0]));
  //SerialBT.println("\r\n");

  delayMicroseconds(50);  delayMicroseconds(50);
}

void operation_time_from_fault_packets(void)
{  
  Serial2.begin(19200,SERIAL_8E1);  
  packet1->id = D1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_O_TIME_FF;
  packet1->no_of_registers = 2;
  packet1->register_array = otf1;
modbus();
  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_O_TIME_FF;
  packet1->no_of_registers = 2;
  packet1->register_array = otf2;
  modbus();
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_O_TIME_FF;
  packet1->no_of_registers = 2;
  packet1->register_array = otf3;
modbus();
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_O_TIME_FF;//YK_O_TIME_FF; //
  packet1->no_of_registers = 2;
  packet1->register_array = otf4;
modbus();

//  delay(10);
//  for(int k=0;k<Modbus_update_loop;k++)
//  {
//    connection_status = modbus_update(packets);
//    delay(50);
//  }

  Serial.print("OT frm Fault 1 : "+ String(otf1[0]));
  Serial.print("\tOT frm Fault 2 : "+ String(otf2[0]));
  Serial.print("\tOT frm Fault 3 : "+ String(otf3[0]));
  Serial.println("\tOT frm Fault 4 : "+ String(otf4[0]));
  Serial.println();
return;

  //SerialBT.print("\tOT frm Fault 1 : "+ String(otf1[0]));
  //SerialBT.print("\tOT frm Fault 2 : "+ String(otf2[0]));
  //SerialBT.print("\tOT frm Fault 3 : "+ String(otf3[0]));
  //SerialBT.println("\tOT frm Fault 4 : "+ String(otf4[0]*24));
  //SerialBT.println();

  delayMicroseconds(50);  delayMicroseconds(50);
}

void operation_time_packets(void)
{
  Serial2.begin(19200,SERIAL_8E1);  
  packet1->id = D1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_O_TIME;
  packet1->no_of_registers = 2;
  packet1->register_array = ot1;
  modbus();
  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_O_TIME;
  packet1->no_of_registers = 2;
  packet1->register_array = ot2;
  modbus();
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_O_TIME;
  packet1->no_of_registers = 2;
  packet1->register_array = ot3;
  modbus();
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address =  YK_O_TIME;//  YK_O_TIME; //
  packet1->no_of_registers = 2;
  packet1->register_array = ot4;
  modbus();
  
//  delay(10);
//  for(int k=0;k<Modbus_update_loop;k++)
//  {
//    connection_status = modbus_update(packets);
//    delay(80);
//  }

      Serial.print("O time 1 : " +String(ot1[0]));
    Serial.print("\tO time 2 : " +String(ot2[0]));
    Serial.print("\tO time 3 : " +String(ot3[0]));
  Serial.println("\tO time 4 : " +String(ot4[0]));
  return;
  //SerialBT.print("O time 1 : " +String(ot1[0]));
  //SerialBT.print("O time 2 : " +String(ot2[0]));
  //SerialBT.print("O time 3 : " +String(ot3[0]));
  //SerialBT.println("O time 4 : " +String(ot4[0]));

  delayMicroseconds(50);  delayMicroseconds(50);
}

void init_modbus_packet(void)
{
  Serial2.begin(19200,SERIAL_8E1); 
  
  packet1->id = D1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_ST_REG;
  packet1->no_of_registers = Status_Read;
  packet1->register_array = regs1;
  modbus();
  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_ST_REG;
  packet1->no_of_registers = Status_Read;
  packet1->register_array = regs2;
  modbus();
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_ST_REG;
  packet1->no_of_registers = Status_Read;
  packet1->register_array = regs3;
  modbus();
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = YK_ST_REG; //YK_ST_REG; //
  packet1->no_of_registers = Status_Read;
  packet1->register_array = regs4;
  modbus();
  
}

void Fault_packet_set(int Fault)// set packet at Fault_1 & Fault_2
{
  if(!Fault)  {
    Fault = Fault_Reg;
  }
  Serial2.begin(19200,SERIAL_8E1);  
  packet1->id = D1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Fault_Reg;
  packet1->no_of_registers = Fault_Read;
  packet1->register_array = regs5;
  modbus();
  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Fault_Reg;
  packet1->no_of_registers = Fault_Read;
  packet1->register_array = regs6;
  modbus();
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Fault_Reg;
  packet1->no_of_registers = Fault_Read;
  packet1->register_array = regs7;
  modbus();
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = Fault_Reg;
  packet1->no_of_registers = Fault_Read;
  packet1->register_array = regs8;
  modbus();
}

void Fan_operation_time_packets(void)
{  
  Serial2.begin(19200,SERIAL_8E1);
  packet1->id = D1;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 154;
  packet1->no_of_registers = 2;
  packet1->register_array = fot1;
modbus();
  packet1->id = D2;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 154;
  packet1->no_of_registers = 2;
  packet1->register_array = fot2;
 modbus();
  packet1->id = D3;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 154;
  packet1->no_of_registers = 2;
  packet1->register_array = fot3;
modbus();
  packet1->id = D4;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 154;
  packet1->no_of_registers = 2;
  packet1->register_array = fot4;
modbus();
//  delay(10);
//  for(int k=0;k<Modbus_update_loop;k++)
//  {
//    connection_status = modbus_update(packets);
//    delay(50);
//  }

  Serial.println("FanO time 1 : " +String(fot1[1]) +String(fot1[0]));
  Serial.println("FanO time 2 : " +String(fot2[1]) +String(fot2[0]));
  Serial.println("FanO time 3 : " +String(fot3[1]) +String(fot3[0]));
  Serial.println("FanO time 4 : " +String(fot4[1]) +String(fot4[0]));
return;
  //SerialBT.println("FanO time 1 : " +String(fot1[1]) +String(fot1[0]));
  //SerialBT.println("FanO time 2 : " +String(fot2[1]) +String(fot2[0]));
  //SerialBT.println("FanO time 3 : " +String(fot3[1]) +String(fot3[0]));
  //SerialBT.println("FanO time 4 : " +String(fot4[1]) +String(fot4[0]*24)); // day to hour

  delayMicroseconds(50);  delayMicroseconds(50);
}

int find_faults(int regC0, int regC1)
{
  int Fault=0;
         if(regC0 & 0B0000000000000010){    Fault=1;  }
    else if(regC0 & 0B0000000000000100){    Fault=2;  }
    else if(regC0 & 0B0000000000100000){    Fault=3;  }
    else if(regC0 & 0B0000000001000000){    Fault=4;  }
    else if(regC0 & 0B0000000010000000){    Fault=5;  }
    else if(regC0 & 0B0000000100000000){    Fault=6;  }
    else if(regC0 & 0B0000001000000000){    Fault=7;  }
    else if(regC0 & 0B0000010000000000){    Fault=8;  }
    else if(regC0 & 0B0000100000000000){    Fault=9;  }
    else if(regC0 & 0B0001000000000000){    Fault=10; }
    else if(regC0 & 0B0010000000000000){    Fault=11; }
    else if(regC0 & 0B0100000000000000){    Fault=12; }
    else if(regC0 & 0B1000000000000000){    Fault=13; }
    else if(regC1 & 0B0000000001000000){    Fault=14; }
    else if(regC1 & 0B0000000010000000){    Fault=15; }
    else if(regC1 & 0B0000000100000000){    Fault=16; }
    else if(regC1 & 0B0000001000000000){    Fault=17; }
    else if(regC1 & 0B0000010000000000){    Fault=18; }
    else if(regC1 & 0B0000100000000000){    Fault=19; }
    else if(regC1 & 0B0001000000000000){    Fault=20; }
    Serial.print(String(packet1->id)+ "\tfault" + Fault);            
    delayMicroseconds(50);
    return Fault;
}

int status_faults(void)
{
      Fault_packet_set(Fault_1);
//      check_response_fault(Fault_1);
      delay(10);
      for(int k=0;k<Modbus_update_loop;k++){
        connection_status = modbus_update(packets);
        delay(50);
      }
      LT_Fault = find_faults(regs5[0],regs5[1]);
      CT_Fault = find_faults(regs6[0],regs6[1]);
      MH_Fault = find_faults(regs7[0],regs7[1]);
      AH_Fault = find_faults(regs8[0],regs8[1]);
}

int check_status(int Status_reg)
{
  int During_run=0,zero_speed=0,reverse_run=0,fault_reset=0,DataS_err=0,drive_ready=0,Alarm=0,fault=0,operation_error=0,powerloss=0;
  if((Status_reg& 0B0000001111111111))//|(regs5[1] & 0B0001111111000000))
  {
     if(Status_reg & 0B0000000000000001)  {    During_run=1;    Serial.print("Drive Run\t");    }
     if(Status_reg & 0B0000000000000010)  {    reverse_run=1;   Serial.print("Reverse Run\t");  }
     if(Status_reg & 0B0000000000000100)  {    drive_ready=1;   Serial.print("Drive Ready\t");  }
     if(Status_reg & 0B0000000000001000)  {    fault=1;         Serial.print("Fault\t");        }
     if(Status_reg & 0B0000000000010000)  {    DataS_err=1;     Serial.print("Data Set Err\t"); }
      Serial.println(String(packet1->id)+ "\tfault" + MH_Fault); 
      Serial.println();
//    Send_MODBUS_DATA=1;
    delayMicroseconds(50);
    if(fault==1 ){
      return 1;
    }
    else{ 
      return 0;
    }
  }
}

void modbus_configuration(void){
  modbus_configure(19200, modbus_timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);         
}

int Modbus_Update(void){
  connection_status = modbus_update(packets);
}

void Modbus_Data_Com(void){
  //    init_modbus_packet();
  delay(10);
  for(int k=0;k<Modbus_update_loop;k++)
  {
    connection_status = modbus_update(packets);
    delay(50);
  } 
  Serial.println("Status: ");
  for(int j=0;j<16;j++){
    Serial.print(regs1[j]);Serial.print('\t');
    Serial.print(regs2[j]);Serial.print('\t');
    Serial.print(regs3[j]);Serial.print('\t');
    Serial.print(regs4[j]);Serial.println();
  }
  Serial.println();  
//  data_str_send();
}

void data_str_send(void){   
    int LT_st_flag = check_status(regs1[0]);
    int CT_st_flag = check_status(regs2[0]);
    int MH_st_flag = check_status(regs3[0]);
    int AH_st_flag = check_status(regs4[0]);    

    int Drive_status = regs1[0]; 
    int Drive_output_vtg  = regs1[5]*(0.1);
    int Drive_output_crnt = regs1[6]*(0.1);
  //  int Drive_output_power= regs1[7];
    int id = Device_id;
    int driver_id = D1;
  //  int DC_vtg = DCV1[0];
    int run_ = digitalRead(LT);
    int Power = (KWH1[1]*10000)+KWH1[0];
    String fan_ =(String(fot4[1])+String(fot4[0])); 
    int Heat_Sink = fan_.toInt();   
  Drive_1_status = "";
  Drive_1_status  = Prepare_Data_String(id,
                                        crane_id,
                                        driver_id, 
                                        run_, 
                                        Drive_output_vtg,
                                        Drive_output_crnt,
//                                        DC_vtg,
                                        Power,
                                        Heat_Sink,
                                        LT_st_flag,
                                        Drive_status
                                        );
    
  Drive_status = regs2[0]; 
  Drive_output_vtg  = regs2[5]*(0.1);
  Drive_output_crnt = regs2[6]*(0.1);
//  Drive_output_power= regs2[7];
  driver_id = D2;
//  DC_vtg = DCV2[0];
  run_ = digitalRead(CT);
  Power = (KWH2[1]*10000)+KWH2[0];
  fan_ =(String(fot2[1])+String(fot2[0])); 
  Heat_Sink = fan_.toInt();   
  Drive_2_status = "";
  Drive_2_status  = Prepare_Data_String(id,
                                        crane_id,
                                        driver_id, 
                                        run_,
                                        Drive_output_vtg,
                                        Drive_output_crnt,
//                                        DC_vtg,
                                        Power,
                                        Heat_Sink,
                                        CT_st_flag,
                                        Drive_status 
                                        ); 
 
  Drive_status = regs3[0]; 
  Drive_output_vtg  = regs3[5]*(0.1);
  Drive_output_crnt = regs3[6]*(0.1);
//  Drive_output_power= regs3[7]; 
  driver_id = D3;
//  DC_vtg = DCV3[0];
  run_ = digitalRead(MH);
  Power = (KWH3[1]*10000)+KWH3[0];
  fan_ =(String(fot3[1])+String(fot3[0])); 
  Heat_Sink = fan_.toInt(); 
  Drive_3_status = "";
  Drive_3_status  = Prepare_Data_String(id,
                                        crane_id,
                                        driver_id, 
                                        run_,
                                        Drive_output_vtg,
                                        Drive_output_crnt,
//                                        DC_vtg,
                                        Power,
                                        Heat_Sink,
                                        MH_st_flag,
                                        Drive_status
                                        ); 
 
  Drive_status = regs4[0]; 
  Drive_output_vtg  = regs3[5]*(0.1);
  Drive_output_crnt = regs3[6]*(0.1);
//  Drive_output_power= regs4[7];
  driver_id = D4;
//  DC_vtg = DCV4[0];
  run_ = digitalRead(AH);
  Power = (KWH4[1]*10000)+KWH4[0];
  fan_ =(String(fot4[1])+String(fot4[0])); 
  Heat_Sink = fan_.toInt();
  Drive_4_status = "";
  Drive_4_status  = Prepare_Data_String(id,
                                        crane_id, 
                                        driver_id,  
                                        run_,  
                                        Drive_output_vtg,
                                        Drive_output_crnt,
//                                        DC_vtg,
                                        Power,
                                        Heat_Sink,
                                        AH_st_flag,
                                        Drive_status); 

  if(LT_st_flag | CT_st_flag | MH_st_flag | AH_st_flag ){
    if(LT_st_flag){fault_drive_LT = 1;}   else{fault_drive_LT = 0;}
    if(CT_st_flag){fault_drive_CT = 1;}   else{fault_drive_CT = 0;}
    if(MH_st_flag){fault_drive_MH = 1;}   else{fault_drive_MH = 0;}
    if(AH_st_flag){fault_drive_AH = 1;}   else{fault_drive_AH = 0;}
    
    Response_fault_2 = 1;     //status_faults();      //init_modbus_packet();
  }
  else  {
    Response_fault_2 = 0;
  } 
} 

String Prepare_Data_String(int id, 
                           char crane_id, 
                           int driver_id,   
                           int run_, 
                           int Drive_output_vtg,
                           int Drive_output_crnt,
//                           int DC_vtg,
                           int Power,
                           int Heat_Sink,
                           int _Fault,
                           int Drive_status 
                           )
{
 
  DateTime now = rtc.now();
  String time_str = "";
  time_str.concat(String(now.year()));    
  time_str.concat(F("-"));                          time_str.concat(String(now.month()));
  time_str.concat(F("-"));                          time_str.concat(String(now.day()));  
  time_str.concat(F(" "));                        time_str.concat(String(now.hour())); 
  time_str.concat(F(":"));                          time_str.concat(String(now.minute()));
  time_str.concat(F(":"));                          time_str.concat(String(now.second()));
//  Serial.println(time_str);
  DynamicJsonDocument  doc(400);
  int operation_time=0;
  int Drive_Output_freq=0;
  int DC_vtg=0;

       if(driver_id==D1){operation_time = ot1[0]; Drive_Output_freq = regs1[3]; DC_vtg = DCV1[0];}
  else if(driver_id==D2){operation_time = ot2[0]; Drive_Output_freq = regs2[3]; DC_vtg = DCV2[0];}
  else if(driver_id==D3){operation_time = ot3[0]; Drive_Output_freq = regs3[3]; DC_vtg = DCV3[0];}
//  else if(driver_id==D4){operation_time = ot4[0]*24; Drive_Output_freq = regs4[12]; DC_vtg = DCV4[0];}
    else if(driver_id==D4){operation_time = ot4[0]; Drive_Output_freq = regs4[3]; DC_vtg = DCV4[0];}
  
  doc["id"]               =     id;
  doc["crane_id"]         =     SENSOR_ID;
  doc["driver_id"]        =     ((SENSOR_ID-1)*4)+driver_id;
  
//  if(driver_id == D4){
//    doc["drive_ready"]      =     (((Drive_status & 0xF000)|0xF000)==0x0000)?true:false;
//  }
//  else{  
    doc["drive_ready"]      =     (Drive_status & 0B0000000000000100)?true:false;
//  }
  
  doc["on_off"]           =     !((bool)run_);
  doc["output_freq"]      =     Drive_Output_freq;  //random(1000,9000);//
  doc["ac_voltage"]       =     Drive_output_vtg;   //random(0,220);//
  doc["output_current"]   =     Drive_output_crnt;  //random(0,100);//
  doc["dc_voltage"]       =     DC_vtg;             //random(0,440);//
  doc["drive_power"]      =     Power;              //random(400,460);//
  doc["operation_time"]   =     operation_time;     //random(400,460);//
//  doc["cum_power"]        =     Power;            //random(400,460);// change remaining 
  doc["heat_sink"]        =     (bool)Heat_Sink;    //random(0,1);//
  doc["reading_time"]     =     time_str;
//  doc["system_time"]      =     time_str;
  doc["fault"]            =     (bool)_Fault;
  doc["driverReading"]    =     Drive_status;
  
  String p_str="";
  serializeJson(doc, p_str);
  return p_str;
}  

//  prepare all strings here
//  http://localhost:50903/Boiler_CMS/Home/SaveDriverReading?driverReading={"id":0,"crane_id":1,"driver_id":1,"on_off":true,"output_freq":10.31,"ac_voltage":10.2,"output_current":1.5,"dc_voltage":5.5,"heat_sink":true,"reading_time":null,"system_time":null,"fault":false,"driverReading":0}
//  http://192.168.1.100/Boiler_CMS/Home/SaveDriverReading?driverReading={"id":0,"crane_id":1,"driver_id":2,"on_off":1,"output_freq":3000,"ac_voltage":204,"output_current":0,"dc_voltage":0,"heat_sink":0,"reading_time":"2020-10-5%2018:56:45","system_time":"2020-10-5%2018:56:45","fault":0,"driverReading":37}
