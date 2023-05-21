#include <Arduino.h>
#include <SPI.h>

#define latch_pin 4
#define blank_pin 22
#define data_pin 23
#define clock_pin 18

int shift_out;
byte anode[8];

byte red0[64], red1[64], red2[64], red3[64];
byte blue0[64], blue1[64], blue2[64], blue3[64];
byte green0[64], green1[64], green2[64], green3[64];

int level = 0;
int anodelevel = 0;
int BAM_Bit, BAM_Counter = 0;

unsigned long start;

hw_timer_t *My_timer = NULL;

void LED(int level, int row, int column, byte red, byte green, byte blue){

  level = constrain(level, 0, 7);
  row = constrain(row, 0, 7);
  column = constrain(column, 0, 7);
  red = constrain(red, 0, 15);
  green = constrain(green, 0, 15);
  blue = constrain(blue, 0, 15);

  int whichbyte = int(((level*64)+(row*8)+column)/8);

  int wholebyte = (level*64)+(row*8)+column;

  bitWrite(red0[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 0));
  bitWrite(red1[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 1));
  bitWrite(red2[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 2)); 
  bitWrite(red3[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 3)); 

  bitWrite(green0[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 0));
  bitWrite(green1[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 1));
  bitWrite(green2[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 2)); 
  bitWrite(green3[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 3));

  bitWrite(blue0[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 0));
  bitWrite(blue1[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 1));
  bitWrite(blue2[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 2)); 
  bitWrite(blue3[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 3));
}

void IRAM_ATTR onTimer(){

  digitalWrite(blank_pin, HIGH);

  if (BAM_Counter == 8){
  BAM_Bit++;
  }

  else if (BAM_Counter == 24){
  BAM_Bit++;
  }

  else if (BAM_Counter == 56){
  BAM_Bit++;
  }

  BAM_Counter++;

  switch (BAM_Bit){
    case 0:
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(red0[shift_out]);
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(green0[shift_out]); 
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(blue0[shift_out]);
        break;
    case 1:
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(red1[shift_out]);
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(green1[shift_out]); 
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(blue1[shift_out]);
        break;
    case 2:
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(red2[shift_out]);
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(green2[shift_out]); 
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(blue2[shift_out]);
        break;
    case 3:
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(red3[shift_out]);
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(green3[shift_out]); 
        for(shift_out=level; shift_out<level+8; shift_out++)
        SPI.transfer(blue3[shift_out]);

        if (BAM_Counter == 120){
            BAM_Counter = 0;
            BAM_Bit = 0;
        }
        break;
  }
  SPI.transfer(anode[anodelevel]);

  digitalWrite(latch_pin, HIGH);
  digitalWrite(latch_pin,LOW);
  digitalWrite(blank_pin,LOW);

  anodelevel++;
  level = level + 8;

  if (anodelevel == 8){
    anodelevel = 0;
  }
  if (level == 64){
    level = 0;
  }
  digitalWrite(blank_pin, LOW);

  // debug
  // Serial.println(".");
  // Serial.println(level);
  // Serial.println(anodelevel);
  // Serial.println("--------");
}

void checkCube() {
  //turn on red
  // Serial.println("check Start");
  digitalWrite(BUILTIN_LED, HIGH);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 8; k++) {
        LED(i, j, k, 15, 0, 0);
        // Serial.println(".");
      }
      // Serial.println("*");
    }
    // Serial.println("^");
  }
  // green
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 8; k++) {
        LED(i, j, k, 0, 15, 0);
      }
    }
  }
  //blue
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 8; k++) {
        LED(i, j, k, 0, 0, 15);
      }
    }
  }
  delay(500);
  digitalWrite(BUILTIN_LED, LOW);
  delay(500);
  // Serial.println("check end");
}

void setup() {
  // put your setup code here, to run once:
  noInterrupts();
  // Serial.begin(115200);
  // Serial.println("Serial Start");
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  // SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setFrequency(40000000);

  anode[7]=B00000001;
  anode[6]=B00000010;
  anode[5]=B00000100;
  anode[4]=B00001000;
  anode[3]=B00010000;
  anode[2]=B00100000;
  anode[1]=B01000000;
  anode[0]=B10000000;

  pinMode(latch_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(blank_pin, OUTPUT);

  pinMode(BUILTIN_LED, OUTPUT);

  SPI.begin();
  interrupts();
  //hardware timer settings
  //8khz / 320(prescaler) * 30(timerHit) * 8(level) => ~100Hz
  //175 runs well
  //155 max
  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 175, true);
  timerAlarmEnable(My_timer);


  // unsigned long startTime;
  // unsigned long endTime;
  // unsigned long executionTime;
  // startTime = micros();
  // onTimer();
  // endTime = micros();
  // executionTime = endTime - startTime;
  // Serial.print(executionTime);

}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("before serial");
  checkCube();
  // Serial.println("after serial");
  // LED(0, 0, 0, 15, 0, 0);
}

