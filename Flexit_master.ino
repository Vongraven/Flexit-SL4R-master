/*
* Flexit master for SL4R ventilation system (CS60 cntrol board)
* Tested on Arduino Mega. 
* Important! increase SERIAL_RX_BUFFER in HardwareSerial.h 
*/

#define RXen    4
#define TXen    5
#define COM_VCC 6

#define rawFanLevel         rawData[5]
#define rawPreheatOnOff     rawData[6]
#define rawHeatExchTemp     rawData[9]
#define rawPreheatActive1   rawData[10]
#define rawPreheatActive2   rawData[11]

#define fanLevel            processedData[0]
#define heatExchTemp        processedData[1]
#define preheatOnOff        processedData[2]
#define preheatActive       processedData[3]


// Eavsdrop command sent from ci50 controller and match values in commandBuffer[] 
uint8_t commandBuffer[18] = {195, 4, 0, 199, 81, 193, 4, 8, 32, 15, 0, 'F', 'P', 4, 0, 'T' };    
uint8_t processedData [4]   = {}; 
uint8_t rawData [25]   = {};                           


void setup() {          
  Serial1.begin(19200, SERIAL_8N1); 

  pinMode(RXen, OUTPUT);
  pinMode(TXen, OUTPUT);
  pinMode(COM_VCC, OUTPUT);
  
  digitalWrite(RXen, LOW);
  digitalWrite(TXen, LOW);
  digitalWrite(COM_VCC, LOW);
 
}


void loop() {       
}

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* CS60 control board repeats 16 lines of data over and over. For this purpose only line number 15 is needed. 
* Look for a specific combination of bytes to identify the correct line.
* When combination is matched, read the line into buffer rawData[]
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateFlexitData() {
  digitalWrite(COM_VCC, HIGH);      // activate MAX485 
  digitalWrite(RXen, LOW);      // RX enable
  uint8_t Buffer[1000];
  
  for (int i=0; i<1000; ++i) {
    while (!Serial1.available()); 
    Buffer[i] = Serial1.read();
    if (Buffer[i]==22 && Buffer[i-2]==193 && Buffer[i-8]==195) {
      for (int i=0; i<25; ++i) {
        while (!Serial1.available()); 
        rawData[i] = Serial1.read();
      }
    break;
    }
    if (i == 999) Serial.println("StBuffer not updated"); 
  }

  digitalWrite(RXen, HIGH);     // RX disable
  digitalWrite(COM_VCC, LOW);     // deactivate MAX485      
  while (Serial1.available()) Serial1.read();     // empty serial RX buffer
}  

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
* - Fan level has three steps. They are received as 17, 34 or 51. Divide by 17 to get 1, 2 or 3.
* - Preheat state is received as 0 (off) or 128 (on), and is translated to 0/1
* - Heat exchanger temperature is unchanged (15 - 25 degrees) 
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void processFlexitData(){
  if (rawFanLevel == 17 ||rawFanLevel == 34 ||rawFanLevel == 51) {
    fanLevel = rawFanLevel / 17;
    commandBuffer[11] = rawFanLevel;      
  }
      
  if (rawPreheatOnOff == 128) {
    preheatOnOff = 1;
    commandBuffer[12] = 128;  
  }
  else if (rawPreheatOnOff == 0) {
    preheatOnOff = commandBuffer[12] = preheatActive = 0; 
  }
      
  if (14< rawHeatExchTemp <26) {
    heatExchTemp = commandBuffer[15] = rawHeatExchTemp;  
  }

  if (rawPreheatActive1 >10 && preheatActive == 0 && preheatOnOff == 1) { 
    preheatActive = 1;
  }
      
  if (rawPreheatActive2 <100 && preheatActive == 1 && preheatOnOff == 1) {
    preheatActive = 0;
  }
}

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* createCommand() takes two arguments, command and level
* cmd: which value in commandbuffer to change
*     fan level           - cmd=11
*     preheat on/off      - cmd=12
*     heat exchanger temp - cmd=15
* 
* lvl: which value to send    
*     fanlevel    1 > 2   - lvl=18    (17 +1)
*                   > 3   - lvl=35    (34 +1)
'                 3 > 2   - lvl=50    (51 -1)
'                   > 1   - lvl=33    (34 -1)

*     preheat         on  - lvl=128   
*                     off - lvl=0
*                     
*     heat exchanger temp - lvl=15-25
'
* Calculate and append check sum
*
' Important! execute updateFlexitData() and processFlexitData() before each command.   
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void createCommand(uint8_t cmd, uint8_t lvl) {                                    
  commandBuffer[cmd] = lvl;
  int sum1=0, sum2=0;
  for ( int i=5; i<(sizeof(commandBuffer)-2) ; ++i) {
    sum1 = sum1 + commandBuffer[i];
    sum2 = (sum2 + sum1);
  }
  commandBuffer[sizeof(commandBuffer)-2] = sum1%256;  
  commandBuffer[sizeof(commandBuffer)-1] = sum2%256;

  sendCommand();
}

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
* Transmission of the commands must be timed so they dont collide with data from the CS60 control board. 
* Find the length of the incomming line (value number 8 in each line), count the bytes until end of line is reached, jump in and send command   
*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
void sendCommand () {  
  digitalWrite(COM_VCC, HIGH);      // activate MAX485
  digitalWrite(RXen, LOW);      // RX enable
  uint8_t data;  
  int Length, repeats=0;
  do {
    while (!Serial1.available());
    data=Serial1.read();
    if (data == 195){
      while (!Serial1.available());
      data=Serial1.read();
      if (data == 1){
        for (int i=0; i<6; ++i) {
          while (!Serial1.available());
          Serial1.read();
        }
    
        while (!Serial1.available());    
        Length = Serial1.read()+2;
        if (3 < Length <33) {
          for (int i=0; i<Length; ++i) {
            while (!Serial1.available());
            Serial1.read();
          }
          digitalWrite(TXen, LOW);      // TX enable
          delay(10);
          Serial1.write(commandBuffer, 18);     // transmit command
          Serial1.flush();
          digitalWrite(TXen, HIGH);     // TX disable
          Serial.println("command sent");
          ++repeats;
        }
      }
    }   
  } while (repeats<5);
  digitalWrite(RXen, HIGH);     // RX disable
  digitalWrite(COM_VCC, LOW);     // deactivate MAX485  
}


