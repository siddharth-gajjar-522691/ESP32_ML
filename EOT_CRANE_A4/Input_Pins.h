#define LT          4  //32 
#define CT          34 //33 
#define MH          35 //25 
#define AH          14 //26
#define INPUT_5     26 //14
#define INPUT_6     25 //35 
#define INPUT_7     33 //34
#define INPUT_8     32 //4

int IN_1_Status = 0;
int IN_2_Status = 0;
int IN_3_Status = 0;
int IN_4_Status = 0;
int IN_5_Status = 0;
int IN_6_Status = 0;
int IN_7_Status = 0;
int IN_8_Status = 0;

unsigned int LT_OT;
unsigned int CT_OT;
unsigned int MH_OT;
unsigned int AH_OT;


unsigned int LT_Fault;
unsigned int CT_Fault;
unsigned int MH_Fault;
unsigned int AH_Fault;

unsigned int MH_TH_1;
unsigned int MH_TH_2;
unsigned int AH_TH_1;
unsigned int AH_TH_2;
unsigned int MH_Gear_Oil;
unsigned int AH_Gear_Oil;

unsigned int regs1[16];
unsigned int regs2[16];
unsigned int regs3[16];
unsigned int regs4[16];
unsigned int regs5[16];
unsigned int regs6[16];
unsigned int regs7[16];
unsigned int regs8[16];
unsigned int buffer_reg1[16];
unsigned int buffer_reg2[16];
unsigned int buffer_reg3[16];
unsigned int buffer_reg4[16];

// int Dat_Addr[]={43,45,63,64,65,66,67,68,69,70,73,74,103,2048,2049,2050,2051,2052,2053,2054,2055,2056,2057};


 
void init_pins(void)
{
  pinMode(LT,INPUT_PULLUP);
  pinMode(CT,INPUT_PULLUP);
  pinMode(MH,INPUT_PULLUP);
  pinMode(AH,INPUT_PULLUP);

  pinMode(INPUT_5,INPUT_PULLUP);
  pinMode(INPUT_6,INPUT_PULLUP);
  pinMode(INPUT_7,INPUT_PULLUP);
  pinMode(INPUT_8,INPUT_PULLUP); 

  Serial.println("Pin Init Complete");
}
