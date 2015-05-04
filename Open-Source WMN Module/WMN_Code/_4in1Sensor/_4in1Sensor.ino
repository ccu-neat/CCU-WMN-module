//=====Debug Function====
#define OPEN_ROUTINGPATH_RECORD false
#define OPEN_LCD true

//=====Node value====
#define NODE_ID 0
#define OPEN_SDCARD true
#define SAMPLE_PERIOD 1

//====Sink Function======
#if NODE_ID == 0
#define ISSINK true
#else
#define ISSINK false
#endif

//Network value - 1
#define NETWORK_MAX_NODE 20        //node數
#define NETWORK_MAX_HOP 5          //hop數
#define DUTY_CYCLE_RATE 1
#define NETWORK_CYCLE (DUTY_CYCLE_RATE * NETWORK_MAX_NODE)   //1*11=11
#define NETWORK_CYCLE_ID (DUTY_CYCLE_RATE * NODE_ID)         //1*1=1


//#define SHOW_SINK_RECEIVE_TOTAL false //很大可能會造成Beacon發送延時(90%以上)

//====Watchdog====
#include <avr/wdt.h>
/*#include <avr/sleep.h>           //MCU sleep
volatile boolean f_wdt=1;
typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;
*/
//====GY-61====
int Xread,Yread,Zread;          //XY軸加速度量測值
#define Xinput 0            //X軸腳位A2
#define Yinput 1          //Y軸腳位A0
#define Zinput 2
int AccelerometerSampleCount = 6; // Sample Count / period.

//=====SoilMoisture(Digital)====
#define SoilMoisturePin 5
int SMState =  0;
//=====Rain Gauge======
#define interruptNumber0 0 //define pin = 2
unsigned long rain_time = 0;
int rainfall = 0;
//====DHT11====
#include <dht11.h>
dht11 DHT11;  
#define DHT11PIN 4   //定義讀取資料的 Pin腳 

//=====Button=======
#define analogpin A3
int ButtonValue = 0;

#if OPEN_SDCARD == true
//====SDcard========
#include <SD.h>
File myFile;
char FileName[10] = "nRF24.txt";
#endif

//===NRF24====
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <Wire.h> 

#if OPEN_LCD == true
//LCD
#include <LiquidCrystal_I2C.h>
#endif

//const Value
boolean IS_SINK = false;
boolean IS_ROUTER = false;
boolean IS_ENDDEVICE = false;

#define BROADCAST_ID  0xFF



//TYPE                                        封包類型
#define PACKET_TYPE_BEACON 0                 //Beacon
#define PACKET_TYPE_PATH 1
#define PACKET_TYPE_UNICAST_UP 0x0F
#define PACKET_TYPE_UNICAST_DOWN 0x8F
#define PACKET_TYPE_BROADCAST_UP 0x0E
#define PACKET_TYPE_BROADCAST_DOWN 0x8E
//LEN
#define PACKET_LEN_MAX 32                   //封包長度
#define PACKET_LEN_BEACON 11
#define PACKET_LEN_PATH 11
#define PACKET_LEN_UNICAST PACKET_LEN_MAX
#define PACKET_LEN_BROADCAST PACKET_LEN_MAX

//Location                                    封包array裡每個位址代表的東東
#define LOCATION_TYPE 1                   //型態
#define LOCATION_SOU 2                    //來源節點
#define LOCATION_DEST 3                   //目的地節點
#define LOCATION_HOP 4                    //跳幾個hop
#define LOCATION_SYNC 5                   //同步時間
#define LOCATION_LEN 9
#define LOCATION_DATA 10
#define LOCATION_UNICAST_DATA (LOCATION_DATA + NETWORK_MAX_HOP)

//Network value - 2
#define NETWORK_MAX_PACKET_TEMP_COUNT 6
#define NETWORK_MAX_MES (PACKET_LEN_MAX - LOCATION_UNICAST_DATA -1)//-1:CheckSum
#define NETWORK_MAX_MES_COUNT 6
#define NETWORK_MAX_TX_DELAY 0 //ms

//Time value
//determine Parent 
#define TIME_DETERMINE_PARENT_PERIOD 3//Number of send beacon
//Path mode value
#define TIME_PERIOD (NETWORK_CYCLE * 1000)//millis
#define TIME_PATH_PERIOD 5//Number of send beacon
#define TIME_PATH_FIRST_PERIOD (NETWORK_MAX_HOP * TIME_DETERMINE_PARENT_PERIOD)//Number of send beacon



//My state value
uint8_t PacketCountValue = 0;
uint8_t *PacketCount = &PacketCountValue;
uint8_t mHop = 255;
uint8_t mParent = 255;
uint8_t type_flag = 0x00;
//Flag of packet
boolean flag_path = false;
boolean flag_unicast = false;
boolean flag_unicast_up[NETWORK_MAX_MES_COUNT] ;
boolean flag_broadcast = false;
boolean flag_broadcast_up = false;
boolean flag_sample_period = false;


//Flag of other
boolean IS_FIRST_PATH = true;

//Max addresses are 5 bytes long. 
const char* Myaddr = "broad";
const char* DesAddress = "broad";
byte TXpacket[PACKET_LEN_MAX];           //32
byte RXpacket[NETWORK_MAX_PACKET_TEMP_COUNT][PACKET_LEN_MAX];

#if OPEN_ROUTINGPATH_RECORD == true
//Path data
volatile uint8_t PathCount = 0;
byte PathLen[NETWORK_MAX_NODE-1];
byte PathData[NETWORK_MAX_NODE-1][NETWORK_MAX_NODE-1];
#endif

//uni mes data
volatile uint8_t MesCount = 0;
byte MesPath[NETWORK_MAX_MES_COUNT][NETWORK_MAX_HOP];
byte MesData[NETWORK_MAX_MES_COUNT][NETWORK_MAX_MES];

//broad mes data
//byte BroadMesData[NETWORK_MAX_MES];

//Node information
//  //High Level node
//  boolean HLnode[NETWORK_MAX_NODE];
//  //Same Level node
//  boolean SLnode[NETWORK_MAX_NODE];
//  //Low Level node
//  boolean LLnode[NETWORK_MAX_NODE];
//  //Low Level node
//  boolean CDnode[NETWORK_MAX_NODE];
  //Parent Windows
const int PW_MAX_COUNT = NETWORK_MAX_NODE;
byte PWnode[PW_MAX_COUNT];
int PWLocation = 0;
boolean PWagain = false;

//==================timer==================
unsigned long TypeStartTime = 0;
uint32_t timer_time = 0;  //timer1 time
uint32_t last_sync_time = 0;
uint32_t last_time_received_child_info = 0;
uint16_t timer1_counter = 62500;   //1000ms記數一次
boolean start = false;

//==================status Flag==================
boolean flag_receive_mode = false;               //接收模式
boolean flag_transmit_mode = false;              //傳輸
boolean flag_process_mode = false;               //處理模式
boolean flag_init_mode = true;                   //空閒
/*boolean flag_sleep_mode = false;                 //睡眠模式*/


//==================Power down -- time slot value==================
unsigned long WorkTime;
unsigned long WorkTimeLimit = 200;
/*unsigned long sleepTimeLimit = 128;*/


//==================Low power==================
const boolean flag_power_save = true;
const uint16_t nms_earlier = 10;// n ms earlier,　提前 n ms起床
const uint16_t Ncounter_earlier = (timer1_counter / 100) * (nms_earlier/10);


//==================Sample period==================

const uint8_t DATA_TYPE_FOURINONE = 0;

int SPcount = 0 ;
boolean flag_sample_data = true;

//==================LCD==================
#if OPEN_LCD == true
LiquidCrystal_I2C lcd(0x27,16,2);
#endif
//==================4 in 1 value==================
int LastTimeType = 0;
int DisplayType = 0;
uint8_t LastTimeDisplayID = 3;
uint8_t DisplaySourceID = 3;
boolean SDclose = false;
uint16_t DataCounter = 1;

#if ISSINK == true
char SensorData[32] ;   //Soil Moisture,RainGauge,DHT11
//char SensorData2[32] ;  //GY61
int SensorDL = 0 ;
//int SensorDL2 = 0 ;
int temp;
//char tmpchar[6];
uint16_t PacketCounter;
char Packetchar[11];
#endif
//==================4 in 1 value END==================

//#if SHOW_SINK_RECEIVE_TOTAL == true
////====Debug value for Sink====
//uint8_t ReceiveRecord = 0;
//uint8_t ReceiveCounter[3];
//boolean flag_display_param = true;
//#endif



// timer1 compare interrupt service
// This function write flag only.
ISR(TIMER1_COMPA_vect)                              //ISR處理區
{
  //beacon
  timer_time++;
  if(timer_time % DUTY_CYCLE_RATE == 0){            //處理off 接收on
    flag_receive_mode = true;
    flag_process_mode = false;
  }
  if(timer_time % NETWORK_CYCLE == NETWORK_CYCLE_ID){          //接收off 傳輸on
     flag_transmit_mode = true;
     flag_receive_mode = false;
//#if SHOW_SINK_RECEIVE_TOTAL == true
//     flag_display_param = true;
//#endif
 #if OPEN_ROUTINGPATH_RECORD == true    
    // transmit Path
    if(IS_ENDDEVICE && ((IS_FIRST_PATH && (abs(millis() - TypeStartTime) > 
         TIME_PERIOD * TIME_PATH_FIRST_PERIOD)))){
      TypeStartTime = millis();
      IS_FIRST_PATH=false;
      flag_path = true;
    }else if(IS_ROUTER && (PathCount > 0)
        &&(abs(millis() - TypeStartTime) > TIME_PERIOD * TIME_PATH_PERIOD)){
      TypeStartTime = millis();
      flag_path = true;
    }
    //Path End
#endif    
    if(flag_sample_data){
      SPcount++;
      if(SAMPLE_PERIOD==SPcount){
        SPcount = 0;
        flag_sample_period = true;
      }
    }
  }
} 

void setup(){
  wdt_disable();
  wdt_enable(WDTO_8S);
  Serial.begin(9600);  
  Mirf_Initialize();
#if OPEN_LCD == true
  lcd.init();
  lcd.backlight();
#endif
  if(start == false){                //start起始值是false
   enable_timer();
   start = true;
 }
 setDeviceType();                    //初始化參數設定
 if(IS_SINK){
   mHop = 0;
   flag_init_mode = false;
 }
 
  #if OPEN_SDCARD == true
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    SDclose = true;
    asm volatile ("  jmp 0");// 軟啟動系統，但不 復位寄存器和變量
  }
  #endif
//  LED debug
//  DDRD |= 0x24;

//  //Rain Gauge
//  attachInterrupt(interruptNumber0,inputPin1,FALLING);
  //Button
  attachInterrupt(1,Switch_mode,FALLING);
  wdt_enable(WDTO_8S);
  
#if ISSINK == true
  memset(SensorData,'\0',32);
#endif
  //Sensor power
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  //mcu sleep
  /*Mcusleep_set();
  setup_watchdog(wdt_128ms);*/
}

//Rain Gauge
void inputPin1(){
  if(abs(millis() - rain_time) > 1){
    rainfall++;
    rain_time = millis();
  }
}

//Button            顯示器紐
void Switch_mode(){
  ButtonValue = analogRead(analogpin);
  if(ButtonValue < 225){
    DisplayType = 0;
  }else if(ButtonValue < 285){
    DisplayType = 1;
  }else if(ButtonValue < 370){
    DisplaySourceID = 3;
  }else if(ButtonValue < 460){
    SDclose =true;
  }
}

void loop(){                                           //Loop開始 先從空閒模式開始判斷
  if(flag_init_mode){
    ReceiveMode(RXpacket[*PacketCount],PacketCount);    //接收封包  封包數
    *PacketCount = 0;
    flag_transmit_mode = false;                         //其他模式關閉
    flag_receive_mode = false;
    if(mParent != 255 && mHop != 255){                  //判斷是否已建立網路
      flag_init_mode = false;
    }
  }else if(flag_transmit_mode){                         //判斷傳輸模式
    if(flag_power_save){
      Mirf.powerUpRx();                                 //開啟接收模式
      delay(nms_earlier + 2);
    }
    if(flag_sample_period){
      DataSample();                                    //取樣data
    }
    TypeChange();                                       //更改目前模式--->LCD顯示
    #if OPEN_ROUTINGPATH_RECORD == true
    PathMode(TXpacket);
    #endif
    UnicastMode(TXpacket);
    BroadcastMode(TXpacket);//unfinsh
    BeaconMode(TXpacket);                              //判斷是否要傳Beacon
    flag_transmit_mode = false;                        //傳送off
    if(flag_power_save){                               //是否需要省電
      Mirf.powerDown();
    }
#if ISSINK == true
    //LCD Display
    LCD_PrintChar(SensorData,true);
    memset(SensorData,'0',32);
#endif
    
  }else if(flag_receive_mode){                         //接收 on
    WorkTime = millis();
    if(flag_power_save && (mParent != 255||NODE_ID==0)){
      Mirf.powerUpRx();
      delay(2);
    }
    while(flag_receive_mode){                       
      //if PacketCount == NETWORK_MAX_PACKET_TEMP_COUNT-1 : Give up the new packet
      if(*PacketCount < NETWORK_MAX_PACKET_TEMP_COUNT && ReceiveMode(RXpacket[*PacketCount],PacketCount)){//buffer是否過載
        WorkTime = millis();
      }
      if(abs(millis() - WorkTime) > WorkTimeLimit){
        flag_receive_mode = false;
      }
    };
    if(flag_power_save && (mParent != 255||NODE_ID==0)){             //接收完，省電
      Mirf.powerDown();
    }
    if(*PacketCount > 0)flag_process_mode = true;
    //*PacketCount :下一個要寫入的陣列
    //ProcessMode() == false : flag_process_mode has changed
    while(flag_process_mode){
      if(!ProcessMode(RXpacket[*PacketCount - 1],PacketCount))break;
    }
  }
//  #if SHOW_SINK_RECEIVE_TOTAL == true
//  else if(!flag_transmit_mode && !flag_receive_mode && flag_display_param){
//    printNodeParameter();
//    flag_display_param = false;
//    ReceiveRecord == 2 ? ReceiveCounter[0] = 0 : ReceiveCounter[ReceiveRecord + 1] = 0;
//    ReceiveRecord == 2 ? ReceiveRecord = 0: ReceiveRecord++;
//  }
//  #endif
  SelectDeviceType();                   //標記EndDevice或Init
  wdt_reset();
}
void TypeChange(){                   //改變模式--->LCD顯示
  char waitchar[] = "Waiting";
   if((LastTimeType != DisplayType) && ButtonValue < 225){
     LastTimeType = DisplayType;
     LCD_PrintChar(waitchar,true);
  }else if((LastTimeType != DisplayType) && ButtonValue < 285){
     LastTimeType = DisplayType;
     LCD_PrintChar(waitchar,true);
  }else if(ButtonValue < 460){
    if(LastTimeDisplayID != DisplaySourceID ){
      LastTimeDisplayID = DisplaySourceID;
      LCD_PrintChar(waitchar,true);
    }
  }
}
boolean DataSample(){
  if(NODE_ID != 0 && mParent != 255 && mHop != 255){
    flag_sample_period = false;
    return FourInOneSample();
  }
  return false;
}


boolean FourInOneSample(){
  int SampleData_Len = 14 + 1 + 1;// DATAsize + DATAtype + Destination
  if(NETWORK_MAX_MES < SampleData_Len){
    Serial.println('L');
    return false;
  }
  byte SampleData[SampleData_Len]; 
  
  SampleData[0] = NODE_ID;
  //Data Counter
  memcpy(&SampleData[1],&DataCounter,2);
  DataCounter++;
  //SoilMoisture
  byte SoilMoisture = digitalRead(SoilMoisturePin);
  memcpy(&SampleData[3],&SoilMoisture,1);
//  SampleData[3] = mHop;
  //RainGauge
  memcpy(&SampleData[4],&rainfall,2);
  Xread = 0;
  Yread = 0;
  Zread = 0;
  for(int i  = 0 ; i < AccelerometerSampleCount ; i++){
    Xread = Xread + analogRead(Xinput);//-339;  //讀X軸加速度
    Yread = Yread + analogRead(Yinput);//-343;  //讀Y軸加速度
    Zread = Zread + analogRead(Zinput);//-415;
    delay(2);
  }
  Xread = Xread / AccelerometerSampleCount;
  Yread = Yread / AccelerometerSampleCount;
  Zread = Zread / AccelerometerSampleCount;
  //GY_61
  memcpy(&SampleData[6],&Xread,2);
  memcpy(&SampleData[8],&Yread,2);
  memcpy(&SampleData[10],&Zread,2);

  //DHT11
  if(DHT11.read(DHT11PIN) == 0){
    uint8_t Humidty =DHT11.humidity;
    uint8_t Temperatrue =DHT11.temperature;
    memcpy(&SampleData[12],&Humidty,1);
    memcpy(&SampleData[13],&Temperatrue,1);
  }
//  SampleData[16] = mParent;
//  SampleData[17] = mHop;
  
//  tmp = myDHT22.getHumidityInt();
//  SampleData[10] = (tmp) & 0xff;
//  SampleData[9] = (tmp >> 8) & 0xff;
  addSampleDataPacket(DATA_TYPE_FOURINONE,SampleData_Len,SampleData,0);
  
  #if OPEN_SDCARD == true 
  if(!SDclose){
    myFile = SD.open(FileName, FILE_WRITE);
    if (!myFile) {
      return false;
    }
    myFile.print(NODE_ID);
    myFile.print(" ");
    myFile.print(DataCounter);
    myFile.print(" ");
    myFile.print(mHop);
    myFile.print(" ");
    myFile.print(mParent);
    myFile.print(" ");
    myFile.print((uint8_t)SampleData[5]);//SoilMoistrue
    myFile.print(" ");
    myFile.print(rainfall);//Raingauge
    myFile.print(" ");
    myFile.print(DHT11.humidity);
    myFile.print(" ");
    myFile.println(DHT11.temperature);
    myFile.print(" ");
    myFile.print(Xread);
    myFile.print(" ");
    myFile.print(Yread);
    myFile.print(" ");
    myFile.println(Zread);
    myFile.close();
  }
  #endif
  return true;
}


void FalseInformationSample(){
  int SoilMoisture_Sample_Len = 8;//size
  float voltage_t = NODE_ID;
//  Serial.println(voltage_t);
  byte SampleData[SoilMoisture_Sample_Len]; 
  
  uint32_t voltage = voltage_t;
  SampleData[3] = voltage;
  voltage = voltage >> 8;
  SampleData[2] = voltage;
  voltage = voltage >> 8;
  SampleData[1] = voltage;
  voltage = voltage >> 8;
  SampleData[0] = voltage;
  
  addSampleDataPacket(0,SoilMoisture_Sample_Len,SampleData,0);
}

void addSampleDataPacket(uint8_t type,int len,byte* data, uint8_t DestID){
  flag_unicast = true;
  flag_unicast_up[MesCount] = true;
//  Serial.println(flag_unicast);
  memset(&MesPath[MesCount][0], 0xff , NETWORK_MAX_HOP);
  memset(&MesPath[MesCount][mHop-1],NODE_ID,1);
  memset(&MesData[MesCount][0], DestID,1);
  memset(&MesData[MesCount][1], type,1);
  memcpy(&MesData[MesCount][2], &data[0] , len);

  MesCount++;
//  Serial.print("MesCount :");
//  Serial.println(MesCount);
}


/**
*  Introduction : This function is used to process the RX packet when the RX packet reaches the destination.
*/
void UnicastDataReceive(byte* rx){
#if ISSINK == true
  if(IS_SINK){
    switch(rx[LOCATION_UNICAST_DATA + 1]){
      case DATA_TYPE_FOURINONE:{
//        #if SHOW_SINK_RECEIVE_TOTAL == true
//        ReceiveCounter[ReceiveRecord] = ReceiveCounter[ReceiveRecord] + 1;
//        #endif
        //省記憶體兼加速
        PacketCounter = *(uint16_t*)(&rx[LOCATION_UNICAST_DATA+3]);
        memset(&SensorData[(uint8_t)rx[LOCATION_UNICAST_DATA+2]],'1',1);
//        #if SHOW_SINK_RECEIVE_TOTAL == false
//        LCD_PrintChar(SensorData,true);
//        #endif
        Serial.write(0xEF);
        Serial.write(rx,PACKET_LEN_UNICAST);
        Serial.write(0xFE);
        #if OPEN_SDCARD == true
        uint8_t tempHop = 0;
        for (uint8_t i = 0; i < NETWORK_MAX_HOP; i++){
            if (rx[LOCATION_UNICAST_DATA + 2] == rx[LOCATION_DATA + i]){
              tempHop = i+1;
              break;
            }
        }

        if(!SDclose){
          memset(FileName,'\0',10);
          itoa_unsigned((uint8_t)rx[LOCATION_UNICAST_DATA+2],FileName);
          myFile = SD.open(FileName, FILE_WRITE);
          if (myFile) {
            myFile.print((uint8_t)rx[LOCATION_UNICAST_DATA+2]);//source ID
            myFile.print(" ");
            myFile.print(PacketCounter);
            myFile.print(" ");
            myFile.print(tempHop);//Hop
            myFile.print(" ");
            myFile.print((uint8_t)rx[LOCATION_UNICAST_DATA+5]);//Soil moisture
            myFile.print(" ");
            myFile.print(*(uint16_t*)(&rx[LOCATION_UNICAST_DATA+6]));//Raingauge
            myFile.print(" ");
            myFile.print(rx[LOCATION_UNICAST_DATA+14]);//Humidity
            myFile.print(" ");
            myFile.print(rx[LOCATION_UNICAST_DATA+15]);//Temp
            myFile.print(" ");
            myFile.print(*(int*)(&rx[LOCATION_UNICAST_DATA+8]));//X Axis
            myFile.print(" ");
            myFile.print(*(int*)(&rx[LOCATION_UNICAST_DATA+10]));//Y Axis
            myFile.print(" ");
            myFile.print(*(int*)(&rx[LOCATION_UNICAST_DATA+12]));//Z Axis
            myFile.print(" ");
            myFile.print((uint8_t)rx[LOCATION_DATA]);//Hop1
            myFile.print(" ");
            myFile.print((uint8_t)rx[LOCATION_DATA+1]);//Hop2
            myFile.print(" ");
            myFile.print((uint8_t)rx[LOCATION_DATA+2]);//Hop3
            myFile.print(" ");
            myFile.print((uint8_t)rx[LOCATION_DATA+3]);//Hop4
            myFile.print(" ");
            myFile.println((uint8_t)rx[LOCATION_DATA+4]);//Hop5
            myFile.close();
          }
        }
        #endif
//        SensorDL = 0;//Location cursor of display mode 1
//        SensorDL2 = 0;//Location cursor of Display mode 2
//        
//        //Source
//        SensorDL = charcopy(SensorData,SensorDL,"ND:");
//        itoa_unsigned((uint8_t)rx[LOCATION_UNICAST_DATA+1],Packetchar);
//        SensorDL = charcopy(SensorData,SensorDL,Packetchar);
//        
//        SensorDL2 = charcopy(SensorData2,SensorDL2,"ND:");
//        SensorDL2 = charcopy(SensorData2,SensorDL2,Packetchar);
//        
//        //Counter
//        PacketCounter = ((uint16_t)(rx[LOCATION_UNICAST_DATA+2]) << 8 | (uint16_t)(rx[LOCATION_UNICAST_DATA+3]));
//        itoa_unsigned(PacketCounter,Packetchar);
//        SensorDL = charcopy(SensorData,SensorDL," C:");
//        SensorDL = charcopy(SensorData,SensorDL,Packetchar);
//        
//        SensorDL2 = charcopy(SensorData2,SensorDL2," C:");
//        SensorDL2 = charcopy(SensorData2,SensorDL2,Packetchar);
//        
//        //Hop
//        temp = rx[LOCATION_UNICAST_DATA+4];//Data value
//        itoa(temp,tmpchar);
//        SensorDL = charcopy(SensorData,SensorDL," Hp:");
//        SensorDL = charcopy(SensorData,SensorDL,tmpchar);
//        
//        //Raingauge
//        temp = ((uint16_t)(rx[LOCATION_UNICAST_DATA+5] << 8 | (uint16_t)rx[LOCATION_UNICAST_DATA+6]));
//        itoa_unsigned((uint16_t)temp,tmpchar);
//        SensorDL = charcopy(SensorData,SensorDL," R:");
//        SensorDL = charcopy(SensorData,SensorDL,tmpchar);
//
//        //Humidity
//        temp = rx[LOCATION_UNICAST_DATA+13];
//        itoa(temp,tmpchar);
//        SensorDL = charcopy(SensorData,SensorDL," H:");
//        SensorDL = charcopy(SensorData,SensorDL,tmpchar);
//        
//        //Temp
//        temp = rx[LOCATION_UNICAST_DATA+14];
//        itoa(temp,tmpchar);
//        SensorDL = charcopy(SensorData,SensorDL," T:");
//        SensorDL = charcopy(SensorData,SensorDL,tmpchar);
//               
//
//        //X Axis
//        temp = ((int)(rx[LOCATION_UNICAST_DATA+7] << 8 | rx[LOCATION_UNICAST_DATA+8]));
//        itoa(temp,tmpchar);
//        SensorDL2 = charcopy(SensorData2,SensorDL2," X:");
//        SensorDL2 = charcopy(SensorData2,SensorDL2,tmpchar);     
//      
//        //Y Axis
//        temp = ((int)(rx[LOCATION_UNICAST_DATA+9] << 8 | rx[LOCATION_UNICAST_DATA+10]));
//        itoa(temp,tmpchar);
//        SensorDL2 = charcopy(SensorData2,SensorDL2," Y:");
//        SensorDL2 = charcopy(SensorData2,SensorDL2,tmpchar); 
//        
//        //Z Axis
//        temp = ((int)(rx[LOCATION_UNICAST_DATA+11] << 8 | rx[LOCATION_UNICAST_DATA+12]));
//        itoa(temp,tmpchar);
//        SensorDL2 = charcopy(SensorData2,SensorDL2," Z:");
//        SensorDL2 = charcopy(SensorData2,SensorDL2,tmpchar);
//        
////        if(DisplaySourceID == rx[LOCATION_SOU]){
//        if(DisplayType == 0)LCD_PrintChar(SensorData);
//        else LCD_PrintChar(SensorData2);
////        }
//        if(!SDclose){
//          myFile = SD.open(FileName, FILE_WRITE);
//          if (!myFile) {
//            break;
//          }
//          myFile.println(SensorData);
//          myFile.close();
//        }
//        memset(SensorData,'\0',35);
//        memset(SensorData2,'\0',35);
        break;
      }
      default:
//        Serial.println("data error");
        break; 
    }
  }
#endif
}

void SelectDeviceType(){
  //Did not receive the Parent beacon
  if(mParent != 255 && !IS_SINK){
    if(((timer_time - last_sync_time )  > TIME_DETERMINE_PARENT_PERIOD * NETWORK_CYCLE) && last_sync_time != 0){
      setInitDeviceType();
    }
  }
  //Did not receive the Child beacon
  if(IS_ROUTER){
    if(((timer_time - last_time_received_child_info )  > TIME_DETERMINE_PARENT_PERIOD * NETWORK_CYCLE) && last_time_received_child_info != 0){
      setEndDeviceType();
      flag_path = true;
    }
  }
}


boolean ReceiveMode(byte* rx,uint8_t* count){                    //接收模式 程式
 //receive message
 if(!Mirf.isSending() && Mirf.dataReady()){                     //收到data
    Mirf.getData(rx);
    uint8_t len = getTypeLen(rx);                               //資料長度
    //Chercksum
    uint8_t checksum = Checksum(rx,len);                        //checksum對比資料無誤
    if(checksum ==rx[len-1]){
      *count = *count + 1;                                     //封包數+1
      onPacketReceive(rx,count);                                //處理封包
    }else{
      Serial.println("CE");                                    //回傳有錯
    }
    return true;
  }else{
    return false;
  }
}
/**
*  if  return true  : continue
*      return false : break
*
*/
boolean ProcessMode(byte* rx,uint8_t* count){                      //處理模式 程式  
  
  //flag_process_mode == true: if timer1 trigger, flag_process_mode = false.
  if(flag_process_mode == true){
    onPacketProcess(rx);
    if(*count > 0){
      (*count)--;
      return true;
    }else{
      flag_process_mode = false;
      return false;
    }
  }else{
    return false;
  }
}


void BeaconMode(byte* data){                                       //Beacon模式
  Serial.println(timer_time); 
  if(mParent!=255) BeaconPacket(data,mParent);                     //有parent，把parent當下一個目的地
  else BeaconPacket(data,BROADCAST_ID);                            //沒的話，做成beacon傳出去

  Mirf_Send(data);
}

void BroadcastMode(byte* data){
    if(flag_broadcast ){
      flag_broadcast = false;
      ///unfinsh
    }
}

void UnicastMode(byte* data){
   if(flag_unicast){
      flag_unicast = false;
      //mes Source
      if(IS_SINK){
#if OPEN_ROUTINGPATH_RECORD == true
//        Serial.println("search path");
        for(int f = 0; f < MesCount ; f++){
          flag_unicast_up[f] = false;
          boolean finish = false;
          for(int i = 0; i < PathCount ; i++){
            for(int j = 0;j < PathLen[i]; j++){
              if(PathData[i][j] == MesPath[f][0]){
                for(int k = 0; k < PathLen[i]-j; k++){
                  MesPath[f][k] = PathData[i][PathLen[i] -k -1];
                }
                finish = true;
              };
              if(finish)break;
            }
            if(finish)break;
          }
          if(finish){
            UniCastPacket(data,flag_unicast_up[f],MesPath[f][1],&MesPath[f][0],&MesData[f][0]);
            Mirf_Send(data);
          }else{
//            Serial.println("not path");          
          }
        }
 #endif       
      }else{
        for(int i = 0 ; i < MesCount ; i++){
          if(flag_unicast_up[i]){
            UniCastPacket(data,flag_unicast_up[i],mParent,&MesPath[i][0],&MesData[i][0]);
          }else{
            UniCastPacket(data,flag_unicast_up[i],MesPath[i][mHop+1],&MesPath[i][0],&MesData[i][0]);
          }
//          for(uint8_t i = 0; i < PACKET_LEN_MAX; i++){
//            Serial.println(data[i]);
//          }
          Mirf_Send(data);
//          Serial.println("-send-");
        }
      }
      MesCount = 0;
    }
    
}
#if OPEN_ROUTINGPATH_RECORD == true
void PathMode(byte* data){            //通道data進
  if(flag_path){
    flag_path = false;
    byte data1[Mirf.payload];
    if(IS_ENDDEVICE){
      PathLen[0] = 1;
      PathData[0][0] = NODE_ID;
      PathPacket(data1,mParent,PathLen[0],&PathData[0][0]);
      Mirf_Send(data1);
    }else if(IS_ROUTER){
      //　要改用MEMCPY
      ///　要檢查PathCount+1
//      Serial.print("PathCount:");  
//      Serial.println(PathCount); 
      for(uint8_t i = 0;i < PathCount ;i++){
        PathPacket(data1,mParent,PathLen[i],&PathData[i][0]);
        Mirf_Send(data1);
      }
      PathCount = 0;
    }
  }
}
#endif


void onPacketReceive(byte* rx , uint8_t* count){                //判斷封包類型
  
  switch(rx[LOCATION_TYPE]){                                    //LOCATION_TYPE=1
    case PACKET_TYPE_BEACON:
      Serial.println("g");
//      Serial.println(millis());
      onPacketReceiveBeacon(rx);
      *count = *count - 1;
      break;
    case PACKET_TYPE_PATH:                                      //不是終點-->封包數-1
      if(NODE_ID != rx[LOCATION_DEST]) *count = *count - 1;
      break;
    case PACKET_TYPE_UNICAST_UP:
      if(NODE_ID != rx[LOCATION_DEST]) *count = *count - 1;
      break;
    case PACKET_TYPE_UNICAST_DOWN:
      if(NODE_ID != rx[LOCATION_DEST]) *count = *count - 1;
      break;
    case PACKET_TYPE_BROADCAST_UP:
      if(NODE_ID != rx[LOCATION_DEST]) *count = *count - 1;
      break;      
    case PACKET_TYPE_BROADCAST_DOWN:
      if(NODE_ID != rx[LOCATION_DEST]) *count = *count - 1;
      break;      
    default:
      break;
  }
}
void onPacketProcess(byte* rx){                            //處理模式---->封包處理
  
  switch(rx[LOCATION_TYPE]){
    case PACKET_TYPE_BEACON:
//      Serial.println(millis());
      break;
#if OPEN_ROUTINGPATH_RECORD == true
    case PACKET_TYPE_PATH:
      if(NODE_ID == rx[LOCATION_DEST]){
        onPacketProcessPath(rx);
      }
      break;
#endif
    case PACKET_TYPE_UNICAST_UP:
      if(NODE_ID == rx[LOCATION_DEST])
        onPacketProcessUnicastUp(rx);
      break;
    case PACKET_TYPE_UNICAST_DOWN:
      if(NODE_ID == rx[LOCATION_DEST])
        onPacketProcessUnicastDown(rx);
      break;
    case PACKET_TYPE_BROADCAST_UP:
      if(NODE_ID == rx[LOCATION_DEST])
//        onPacketProcessUnicastUp(rx);
      break;      
    case PACKET_TYPE_BROADCAST_DOWN:
      if(NODE_ID == rx[LOCATION_DEST])
//        onPacketProcessUnicastDown(rx);
      break;      
    default:
      break;
  }
}

//void onPacketReceiveBroadcastUp(byte* rx){
//  for(uint8_t i = 0 ; i < NETWORK_MAX_MES ; i++){
//    BroadMesData[i] = rx[LOCATION_DATA + i];
//  }
//  flag_broadcast = true;
//  if(!IS_SINK){
//    flag_broadcast_up = true;
//  }else{
//    flag_broadcast_up = false;
//  }
//}

//void onPacketReceiveBroadcastDown(byte* rx){
//  flag_broadcast = true;
//  flag_broadcast_up = false;
//
//  for(uint8_t i = 0 ; i < NETWORK_MAX_MES ; i++){
//    BroadMesData[i] = rx[LOCATION_DATA + i];
//  }
//  LCD_PrintChar((char*)BroadMesData);
//  if(IS_ENDDEVICE)flag_broadcast = false;
//}

void onPacketProcessUnicastUp(byte* rx){
  if(NODE_ID == rx[LOCATION_UNICAST_DATA]){
//    for(uint8_t i = 0; i < PACKET_LEN_MAX; i++){
//      Serial.print(rx[i]);
//      Serial.print("\t");
//    }
//    Serial.println("");
    UnicastDataReceive(rx);
  }else{
//    Serial.println("Transfer Data get!!"); 
    memcpy(&MesPath[MesCount][0],&rx[LOCATION_DATA],NETWORK_MAX_HOP);
    memcpy(&MesData[MesCount][0],&rx[LOCATION_DATA + NETWORK_MAX_HOP],NETWORK_MAX_MES);
    
    flag_unicast = true;
    if(!IS_SINK){
      MesPath[MesCount][mHop-1] = NODE_ID;
      flag_unicast_up[MesCount] = true;
    }else{
      flag_unicast_up[MesCount] = false;
    }
    MesCount++;
//    Serial.print("MesCount :");
//    Serial.println(MesCount);
  }
}
void onPacketProcessUnicastDown(byte* rx){
  flag_unicast = true;
  flag_unicast_up[MesCount] = false;
  boolean destination = false;
  for(uint8_t i = 0 ; i < NETWORK_MAX_HOP ; i++){
    MesPath[MesCount][i] = rx[LOCATION_DATA + i];
    if(rx[LOCATION_DATA + i] ==NODE_ID && (mHop == 5 || rx[LOCATION_DATA + i + 1] ==0xFF)){
      destination = true;
    }
  }
  
  for(uint8_t i = 0 ; i < NETWORK_MAX_MES ; i++){
    MesData[MesCount][i] = rx[LOCATION_DATA + NETWORK_MAX_HOP + i];
  }
  if(destination){
    UnicastDataReceive(rx);
    LCD_PrintChar((char*)&MesData[MesCount][0],true);
    flag_unicast = false;
  }
  MesCount++;
}

#if OPEN_ROUTINGPATH_RECORD == true
void onPacketProcessPath(byte* rx){
  if(IS_SINK){
//    LCD_PrintString("S:P"+String(rx[LOCATION_SOU]));
    if(!hasPreviousPath(rx)){
      PathLen[PathCount] = rx[LOCATION_LEN] - PACKET_LEN_PATH + 1;
      for(uint8_t i = 0; i < (rx[LOCATION_LEN] - PACKET_LEN_PATH) ; i++){
//          Serial.print(rx[LOCATION_DATA+i]);
//          Serial.print(" ");
          PathData[PathCount][i] = rx[LOCATION_DATA+i];
      }
//      Serial.println("");
      PathCount = PathCount + 1;
    }
  }else if(IS_ROUTER){
//    Serial.println("             R:P"+String(rx[LOCATION_SOU]));
    PathLen[PathCount] = rx[LOCATION_LEN] - PACKET_LEN_PATH + 1;
    for(uint8_t i = 0; i < (rx[LOCATION_LEN] - PACKET_LEN_PATH) ; i++){
        PathData[PathCount][i] = rx[LOCATION_DATA+i];
    }
    PathData[PathCount][(rx[LOCATION_LEN] - PACKET_LEN_PATH)] = NODE_ID;
    PathCount = PathCount + 0x01;
  }else{
//    Serial.println("big error");
  }
}

boolean hasPreviousPath(byte* rx){
    boolean PreviousPath = false;
    for(uint8_t j = 0;j < PathCount; j++){
      if(PathData[j][0] == rx[LOCATION_DATA]){
        PreviousPath = true;
        PathLen[j] = rx[LOCATION_LEN] - PACKET_LEN_PATH + 1;
        for(uint8_t k = 0; k < (rx[LOCATION_LEN] - PACKET_LEN_PATH) ; k++){
//            Serial.print(rx[LOCATION_DATA+k]);
            PathData[j][k] = rx[LOCATION_DATA+k];
        }
//        Serial.print(" renew");
//        Serial.println("");
        break;
      }
    }
    return PreviousPath;
}
#endif

boolean flag_first_sync = true;
uint8_t PWCounter[NETWORK_MAX_NODE];
void onPacketReceiveBeacon(byte* rx){                                      //Beacon封包處理
      if(mHop > rx[LOCATION_HOP] && (mParent == 255)){
        //New Parent
        if(((timer_time - last_sync_time )  > TIME_DETERMINE_PARENT_PERIOD * NETWORK_CYCLE) && !flag_first_sync){  
          if(!PWagain && PWLocation == 0){
            timer_time = 0;
            last_sync_time = 0;
            flag_first_sync = true;
          }else{
            reset_timer();
            uint8_t maxCount = 0;
            uint8_t ID = 0;
			uint8_t samenumber = 0;
			uint8_t randnumber = 0;
			for(uint8_t i = 0; i < PW_MAX_COUNT;i++){
              if(!PWagain && i == PWLocation)break;
              
			  PWCounter[PWnode[i]] = PWCounter[PWnode[i]] + 1;
			  if(PWCounter[PWnode[i]] > maxCount){         //得知最大值
				maxCount = PWCounter[PWnode[i]];
			  }
            }
            for(uint8_t i = 0; i < PW_MAX_COUNT; i++){
			  if(PWCounter[i] == maxCount){
				samenumber++;                             //多少node重複次數最高
			  }
			}
			randnumber = random(samenumber) +1;           //從最多的node中隨機選出一個
			for(uint8_t i = 0; i < PW_MAX_COUNT; i++){
			  if(PWCounter[i] == maxCount){
				ID++;
			  }
			  if(ID == randnumber){
				ID = i;                                  //得到最後的parent
				break;
			  }
			}
            /*for(uint8_t i = 0; i < PW_MAX_COUNT;i++){
              if(!PWagain && i == PWLocation)break;
              PWCounter[PWnode[i]] = PWCounter[PWnode[i]] + 1;
            }
            for(uint8_t i = 0; i < PW_MAX_COUNT; i++){
              if(PWCounter[i] > maxCount){
                maxCount = PWCounter[i];
                ID = i;
              }
            }*/
            memset(PWCounter,0x00,NETWORK_MAX_NODE);
            type_flag = type_flag | 0x01;
            mParent = ID;
//            Serial.println("My New Parent:"+String(mParent));
            Serial.print("MNP:");
            Serial.println(mParent,DEC);
            syncTimer(rx);
          }
        }else{                                                //沒超過timeout
          if(flag_first_sync){
            last_sync_time = timer_time;
            flag_first_sync = false;
          }
          if(rx[LOCATION_HOP] < NETWORK_MAX_HOP){             //hop小於5
            PWnode[PWLocation++] = rx[LOCATION_SOU];
            if(PWLocation == PW_MAX_COUNT){
              PWLocation = 0;
              PWagain = true;
            }
          }
//          Serial.println((unsigned int)rx[LOCATION_SOU]);
//          Serial.print("T :");
//          unsigned int time123  = timer_time;
//          Serial.println(timer_time);
//          Serial.print("L :");
//          time123 = last_sync_time;
//          Serial.println(last_sync_time);
        }
      }else if(mHop > rx[LOCATION_HOP] && mParent == rx[LOCATION_SOU]){
        //Parent
//        Serial.println("My Parent:   "+String(mParent));
        boolean flag = true;
        if(mHop != 255)flag = false;
        reset_timer();
        syncTimer(rx);
        if(flag){
          mHop = rx[LOCATION_HOP] + 1;
          #if ISSINK == false
          printNodeParameter();
          #endif
        }
      }else if(mHop > rx[LOCATION_HOP] && mParent != rx[LOCATION_SOU]){
//        Serial.println("Not My Parent: "+String(rx[LOCATION_SOU]));
//        HLnode[rx[LOCATION_SOU]] = true;
      }else if(mHop == rx[LOCATION_HOP]){
//        Serial.println("Same :        "+String(rx[LOCATION_SOU]));
//        SLnode[rx[LOCATION_SOU]] = true;
      }else if(mHop < rx[LOCATION_HOP] && NODE_ID == rx[LOCATION_DEST]){
//        Serial.println("Child :       "+String(rx[LOCATION_SOU]));
//        CDnode[rx[LOCATION_SOU]] = true;
//        LLnode[rx[LOCATION_SOU]] = true;
        if(!IS_SINK&&!IS_ROUTER)setRouterType();
        if(IS_ROUTER)last_time_received_child_info = timer_time;
      }else if(mHop < rx[LOCATION_HOP]){
//        Serial.println("Not child:     "+String(rx[LOCATION_SOU]));
//        CDnode[rx[LOCATION_SOU]] = false;
//        LLnode[rx[LOCATION_SOU]] = true;
      }
}

void syncTimer(byte* rx){
    timer_time = ((unsigned long)(rx[LOCATION_SYNC])<<24)
              |  ((unsigned long)(rx[LOCATION_SYNC+1])<<16)
              |  ((unsigned long)(rx[LOCATION_SYNC+2])<<8)
              |  ((unsigned long)(rx[LOCATION_SYNC+3]));
    last_sync_time = timer_time;
}

#if ISSINK == false
/** 
*  Function Name : printNodeParameter() for Child
*  Author : Lin, H.-H.
*  Param :  
*      FSM: Finite-state machine of Node
*      Part: Progress
*  Return :  void
*  Description: 
*        This function is used for debug.
**/

void printNodeParameter(){
    int location = 3;
    char temp[4];
    char typeState2[17] = "ID:";
    itoa(NODE_ID,temp);
    location = charcopy(typeState2,location,temp);
    location = charcopy(typeState2,location," P:");
    itoa(mParent,temp);
    location = charcopy(typeState2,location,temp);
    location = charcopy(typeState2,location," H:");
    itoa(mHop,temp);
    location = charcopy(typeState2,location,temp);
	//2014/8/25新增
/*	location = charcopy(typeState2,location," NUM:");
	itoa(DataCounter,temp);
	location = charcopy(typeState2,location,temp);*/
	//印出
    LCD_PrintChar(typeState2,true);
}
#endif

//#if SHOW_SINK_RECEIVE_TOTAL == true
///** 
//*  Function Name : printNodeParameter() for Sink
//*  Author : Lin, H.-H.
//*  Param :  
//*      FSM: Finite-state machine of Node
//*      Part: Progress
//*  Return :  void
//*  Description: 
//*        This function is used for debug.
//**/
//void printNodeParameter(){
//    char temp[3];
//    char typeState2[32];
//    int location = 0;
//    //line = 0
//    itoa_unsigned(ReceiveRecord,temp);
//    location = charcopy(typeState2,location,temp);
//    typeState2[location++] = ' ';
//
//    itoa_unsigned(ReceiveCounter[0],temp);
//    location = charcopy(typeState2,location,temp);
//    typeState2[location++] = ' ';
//    
//    itoa_unsigned(ReceiveCounter[1],temp);
//    location = charcopy(typeState2,location,temp);
//    typeState2[location++] = ' ';
//    
//    itoa_unsigned(ReceiveCounter[2],temp);
//    location = charcopy(typeState2,location,temp);
//    typeState2[location++] = ' '; 
//    LCD_PrintChar(typeState2,false);
//}
//#endif

void setDeviceType(){                             //初始化參數設定
  if(NODE_ID == 0){                                //節點 0 -->sink做設定
    char typeState[] = "Initializing...";
    LCD_PrintChar(typeState,true);                //LCD顯示
    type_flag = type_flag | 0x01;                  //01
    IS_SINK = true;
    IS_ROUTER = false;
    IS_ENDDEVICE = false;
    TypeStartTime = millis();                      //記錄開始時間
  }else{                                           //其他節點
    IS_SINK = false;
    IS_ROUTER = false;
    IS_ENDDEVICE = true;
    char typeState2[9] = "9/02ED:";                           //未建立網路前皆為end device
    typeState2[7] = (NODE_ID + 0x30);
    typeState2[8] = '\0';
    LCD_PrintChar(typeState2,true);
    type_flag = type_flag & 0xFE ;
    TypeStartTime = millis();
  }
}

void setInitDeviceType(){                          //將裝置變Init
    Mirf.powerUpRx();
    delay(2);
    NODE_ID == 0x00 ? flag_init_mode = false : flag_init_mode = true;
    IS_SINK = false;
    IS_ROUTER = false;
    IS_ENDDEVICE = true;
    mHop = 255;
    mParent = 255;
    type_flag = 0x00;
    flag_path = false;
    flag_unicast = false;
    flag_broadcast = false;
    flag_broadcast_up = false;
    flag_sample_period = false;
    IS_FIRST_PATH = true;
    MesCount = 0;
    PWagain = false;
    PWLocation = 0;
    TypeStartTime = millis();
    timer_time = 0;
    last_sync_time = 0;
    last_time_received_child_info = 0;
    flag_receive_mode = false;
    flag_transmit_mode = false;
    flag_process_mode = false;
    WorkTime = 0 ;
    flag_first_sync = true;
    flag_path = true;
    
    LCD_PrintChar("Inti",true);
}

void setEndDeviceType(){                            //標記EndDevice
    unsigned int id = NODE_ID;
    IS_SINK = false;
    IS_ROUTER = false;
    IS_ENDDEVICE = true;
//    LCD_PrintString("ENDDEVICE:" + String(id));
    TypeStartTime = millis();
}

void setRouterType(){                               //標記Router
    unsigned int id = NODE_ID;
    IS_SINK = false;
    IS_ROUTER = true;
    IS_ENDDEVICE = false;
//    LCD_PrintString("ROUTER:" + String(id));
    TypeStartTime = millis();
}

uint8_t getTypeLen (byte *rx){
  switch(rx[LOCATION_TYPE]){
    case PACKET_TYPE_BEACON:
      return PACKET_LEN_BEACON;
      break;
   case PACKET_TYPE_PATH:
      return rx[LOCATION_LEN];
      break;
   case PACKET_TYPE_UNICAST_UP:
      return PACKET_LEN_UNICAST;
      break;
   case PACKET_TYPE_UNICAST_DOWN:
      return PACKET_LEN_UNICAST;
      break;
   case PACKET_TYPE_BROADCAST_UP:
      return PACKET_LEN_BROADCAST;
      break;
   case PACKET_TYPE_BROADCAST_DOWN:
      return PACKET_LEN_BROADCAST;
      break;      
    default:
      return 0;
      break;
  }
}
void BeaconPacket(byte *packet,uint8_t parent){                //建立beacon封包
  unsigned long time = timer_time;
  packet[0]=type_flag;
  packet[LOCATION_TYPE]=PACKET_TYPE_BEACON;
  packet[LOCATION_SOU]=NODE_ID;
  packet[LOCATION_DEST]=parent;//Destination
  packet[LOCATION_HOP]=mHop;
  packet[LOCATION_SYNC+3]=time;
  time=time>>8;
  packet[LOCATION_SYNC+2]=time;
  time=time>>8;
  packet[LOCATION_SYNC+1]=time;
  time=time>>8;
  packet[LOCATION_SYNC]=time;
  packet[LOCATION_LEN]=PACKET_LEN_BEACON;
  packet[PACKET_LEN_BEACON-1]=Checksum(packet,PACKET_LEN_BEACON);
}

void PathPacket(byte *packet,uint8_t parent,uint8_t path_len,byte *path_data){   //封包資料修改
  unsigned long time = timer_time;
  packet[0]=type_flag;                     //0x00
  packet[LOCATION_TYPE]=PACKET_TYPE_PATH;
  packet[LOCATION_SOU]=NODE_ID;         //來源
  packet[LOCATION_DEST]=parent;        //Destination
  packet[LOCATION_HOP]=mHop;               //幾個hop數
  packet[LOCATION_SYNC+3]=time;
  time=time>>8;
  packet[LOCATION_SYNC+2]=time;
  time=time>>8;
  packet[LOCATION_SYNC+1]=time;
  time=time>>8;
  packet[LOCATION_SYNC]=time;
  packet[LOCATION_LEN]=PACKET_LEN_PATH + path_len;
  memcpy(&packet[LOCATION_DATA],&path_data[0],path_len);
  packet[PACKET_LEN_PATH + path_len-1]=Checksum(packet,PACKET_LEN_PATH+path_len);
}

void UniCastPacket(byte *packet,boolean up_down,byte dest,byte *path_data,byte *data){
  unsigned long time = timer_time;
  packet[0]=type_flag;
  if(up_down){
    packet[LOCATION_TYPE]=PACKET_TYPE_UNICAST_UP;
//    Serial.print(PACKET_TYPE_UNICAST_UP);
  }else{
    packet[LOCATION_TYPE]=PACKET_TYPE_UNICAST_DOWN;
//    Serial.print(PACKET_TYPE_UNICAST_DOWN);
  }
  packet[LOCATION_SOU]=NODE_ID;
  packet[LOCATION_DEST]=dest;//Destination
  packet[LOCATION_HOP]=mHop;
  packet[LOCATION_SYNC+3]=time;
  time=time>>8;
  packet[LOCATION_SYNC+2]=time;
  time=time>>8;
  packet[LOCATION_SYNC+1]=time;
  time=time>>8;
  packet[LOCATION_SYNC]=time;
  packet[LOCATION_LEN]=PACKET_LEN_UNICAST;
  memcpy(&packet[LOCATION_DATA],&path_data[0],NETWORK_MAX_HOP);
  memcpy(&packet[LOCATION_DATA + NETWORK_MAX_HOP],&data[0],NETWORK_MAX_MES);
  packet[PACKET_LEN_UNICAST-1]=Checksum(packet,PACKET_LEN_UNICAST);
}

void BroadCastPacket(byte *packet,boolean up_down,byte dest,byte *path_data,byte *data){
  unsigned long time = timer_time;
  packet[0]=type_flag;
  if(up_down){
    packet[LOCATION_TYPE]=PACKET_TYPE_BROADCAST_UP;
  }else{
    packet[LOCATION_TYPE]=PACKET_TYPE_BROADCAST_DOWN;
  }
  packet[LOCATION_SOU]=NODE_ID;
  packet[LOCATION_DEST]=dest;//Destination
  packet[LOCATION_HOP]=mHop;
  packet[LOCATION_SYNC+3]=time;
  time=time>>8;
  packet[LOCATION_SYNC+2]=time;
  time=time>>8;
  packet[LOCATION_SYNC+1]=time;
  time=time>>8;
  packet[LOCATION_SYNC]=time;
  packet[LOCATION_LEN]=PACKET_LEN_BROADCAST;

  memcpy(&packet[LOCATION_DATA],&path_data[0],NETWORK_MAX_MES);
  packet[PACKET_LEN_BROADCAST-1]=Checksum(packet,PACKET_LEN_BROADCAST);
}


uint8_t Checksum(byte *packet,uint8_t len){
  uint8_t result2=0x00;
  for(int i = 0 ; i < len-1; i++){
    result2=result2+packet[i];
  }
  result2=0xFF-result2;
  return result2;
}

void Mirf_Send(byte *packet){
  Mirf.send(packet);
  while(Mirf.isSending()){
    delay(1);
  }
}

void Mirf_Initialize(){
  Mirf.spi = &MirfHardwareSpi;  
  Mirf.init();
  Mirf.setRADDR((byte *)Myaddr);
  Mirf.setTADDR((byte *)DesAddress);
//  Mirf.payload = sizeof(char);//Size in bytes, default 16, max 32.
  Mirf.payload = PACKET_LEN_MAX;
  Mirf.config();
}

//enable timer1
void enable_timer(){
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  TCCR1B |= (1 << WGM12);   // CTC mode
  ICR1 = timer1_counter;             // 62500 = 1 second , 31250 = 0.5 sec
  TCCR1B |= (1 << WGM13);
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

//reset timer
void reset_timer(){
  noInterrupts();
  TCNT1  = Ncounter_earlier;//(62500/1000)*(10) 提早10 ms醒
  //TCNT1 = 3750;
  interrupts();
}

//disable timer
void disable_timer(){
  noInterrupts();           // disable all interrupts
  TIMSK1 |= (0 << OCIE1A);  // disable timer compare interrupt
  interrupts();             // enable all interrupts
}

int charcopy(char* dest , int loca , char* data){
  memcpy(&dest[loca], data, strlen(data));
  return loca + strlen(data);
}

size_t strlen(const char * str) {
   const char *cp =  str;
   while (*cp++ );
   return (cp - str - 1 );
}


/* Function Name : LCD_PrintString
*  Param :
*        s: s is used for print on LCD.
*        setPrintParam : use Serial.println?
*  Result :  
*      true : Display success.
*      false: The string is too long.
*  Coding by Lin.
*/
boolean LCD_PrintChar(char* s,boolean setPrintParam){//LCD印出
    if(setPrintParam)Serial.println(s);
    #if OPEN_LCD == true
    lcd.clear();
    lcd.setCursor(0,0);                              //第一列
    for(int i = 0;i < 16; i++){
      if(s[i] == '\0')return true;
      lcd.print(s[i]);
    }
    lcd.setCursor(0,1);                              //col=0 row=1
    for(int i = 16;i < 32; i++){
      if(s[i] == '\0')return true;
      lcd.print(s[i]);
    }
    #endif
    return true;
}
/* itoa:  convert n to characters in s */
static void itoa_unsigned(uint16_t n,char s[])
{
  uint16_t i, sign;
 
  if ((sign = n) < 0)  /* record sign */
     n = -n;          /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
     s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);     /* delete it */
   if (sign < 0)
     s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}

/* itoa:  convert n to characters in s */
static void itoa_unsigned(uint8_t n,char s[])
{
  uint8_t i, sign;
 
  if ((sign = n) < 0)  /* record sign */
     n = -n;          /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
     s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);     /* delete it */
   if (sign < 0)
     s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}

/* itoa:  convert n to characters in s */
static void itoa(int n,char s[])
{
  int i, sign;
 
  if ((sign = n) < 0)  /* record sign */
     n = -n;          /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
     s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);     /* delete it */
   if (sign < 0)
     s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}

static void reverse(char s[])
{
   int i, j;
   char c;
 
   for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
     c = s[i];
     s[i] = s[j];
     s[j] = c;
   }
}
//-----MCU set sleep mode----------
/*void Mcusleep_set()
{
	cbi( SMCR,SE );      // sleep enable, power down mode
	cbi( SMCR,SM0 );     // power down mode
	sbi( SMCR,SM1 );     // power down mode
	cbi( SMCR,SM2 );     // power down mode
}
//-----set watchdog time------------------------
void setup_watchdog(int ii) {
 byte bb;
 int ww;
 if (ii > 9 ) ii=9;
 bb=ii & 7;
 if (ii > 7) bb|= (1<<5);
 bb|= (1<<WDCE);
 ww=bb;
 MCUSR &= ~(1<<WDRF);
 // start timed sequence
 WDTCSR |= (1<<WDCE) | (1<<WDE);
 // set new watchdog timeout value
 WDTCSR = bb;
 WDTCSR |= _BV(WDIE);
}
//-----MCU sleep mode---------------------------------
void system_sleep() {
 cbi(ADCSRA,ADEN);  // switch Analog to Digitalconverter OFF
 set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
 sleep_enable();
 sleep_cpu();                        // System sleeps here
 sleep_disable();              // System continues execution here when watchdog timed out 
 sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}*/
