// FILE UPDATED TO WORK WITH BMS SETTIGNS AS OF SEPTEMBER 10, 2023

#include <FlexCAN_T4.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "EasyNextionLibrary.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
// FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
CAN_message_t msg;
EasyNex myNex(Serial1);
int pageNum = 0;

void canSniff(const CAN_message_t &msg);
int hexToDec(char hex[]);

void nextionSend();


//0x680
int pack_current_1 = 0;
int pack_current_2 = 0;
int pack_inst_vol_1 = 0;
int pack_inst_vol_2 = 0;
int pack_SOC = 0;
int relay_state_1 = 0;
int relay_state_2 = 0;

//0x681
int high_temp = 0;
int high_ID = 0;
int avg_temp = 0;
int input_supply_vol_1 = 0;
int input_supply_vol_2 = 0;
int low_cell_vol_1 = 0;
int low_cell_vol_2 = 0;

//0xAFF
int low_cell_ID = 0;
// int avg_temp;
int avg_cell_vol_1 = 0;
int avg_cell_vol_2 = 0;

//0x051
int ERPM_1 = 0;
int ERPM_2 = 0;
int ERPM_3 = 0;
int ERPM_4 = 0;
int duty_cycle_1 = 0;
int duty_cycle_2 = 0;
int MC_input_voltage_1 = 0;
int MC_input_voltage_2 = 0;

//0x151
int AC_current_1 = 0;
int AC_current_2 = 0;
int DC_current_1 = 0;
int DC_current_2 = 0;

//0x251
int controller_temp_1 = 0;
int controller_temp_2 = 0;
int motor_temp_1 = 0;
int motor_temp_2 = 0;
int MC_fault_code = 0;

bool inCanSniff = true;

int delay_counter = 0;

void setup(void) {

  myNex.begin(9600);
  myNex.writeStr("speed.txt", "HI");
  // myNex.writeNum("soc.val", 0);
  // myNex.writeNum("dcCurrent.val", 0);
  // myNex.writeNum("packVoltage.val", 0);
  // myNex.writeNum("acTemp.val", 0);
  // myNex.writeNum("rpm.val", 0);
  // myNex.writeNum("temp.val", 0);
  // myNex.writeNum("acCurrent.val", 0);
  // myNex.writeStr("page page0");

  can1.begin();
  can1.setBaudRate(500000);
  // can2.begin();
  // can2.setBaudRate(500000);

  can1.setMB(MB0,RX,EXT); //Configure the mailbox
  can1.setMB(MB1,RX,EXT); //Configure the mailbox
  can1.setMB(MB2,RX,EXT); //Configure the mailbox
  can1.setMB(MB3,RX,EXT); //Configure the mailbox
  can1.setMB(MB4,RX,EXT); //Configure the mailbox
  can1.setMB(MB5,RX,EXT); //Configure the mailbox

  can1.setMBFilter(REJECT_ALL); //reject all inputs prior to filtering
  can1.enableMBInterrupts();
  can1.setMBFilter(MB0, 0x680); 
  can1.setMBFilter(MB1, 0x681); 
  can1.setMBFilter(MB2, 0xAFF); 
  can1.setMBFilter(MB3, 0x051); 
  can1.setMBFilter(MB4, 0x151); 
  can1.setMBFilter(MB5, 0x251); 
  
  can1.onReceive(MB0,canSniff);
  can1.onReceive(MB1,canSniff);
  can1.onReceive(MB2,canSniff);
  can1.onReceive(MB3,canSniff);
  can1.onReceive(MB4,canSniff);
  can1.onReceive(MB5,canSniff);
}

void loop() {
  
  can1.events();
  delay_counter++;
  if(delay_counter == 1000) {
    delay_counter = 0;
    nextionSend();
  }
}

void nextionSend() {
  int pack_voltage = concat(pack_inst_vol_1, pack_inst_vol_2);
  int pack_current = concat(pack_current_1, pack_current_2);
  
  Serial.println(); 
  Serial.println(); 
  Serial.println(); 
  Serial.println(); 
  Serial.println(); 
  Serial.println(); 
  Serial.println(); 
  Serial.println(); 
  Serial.println(); 

  Serial.print("Pack Current: "); Serial.print(pack_current);
  myNex.writeStr("maxTorque.txt", pack_current);
  Serial.print(" Pack Voltage: "); Serial.print(pack_voltage);
  myNex.writeStr("charge.txt", (pack_voltage/10));
  myNex.writeNum("battery.val", (pack_voltage/10 - 250));
  Serial.print(" Pack SOC: "); Serial.print(pack_SOC/2);
  myNex.writeStr("soc.txt", pack_SOC/2);
//  Serial.println(); 

  Serial.print(" High temp: "); Serial.print(high_temp);
  Serial.print(" High ID: "); Serial.print(high_ID);
  myNex.writeStr("highCellTemp.txt", high_temp);
  Serial.print(" Avg. temp "); Serial.print(avg_temp);
  myNex.writeStr("cellTemp.txt", avg_temp);

  int supply_voltage = concat(input_supply_vol_1, input_supply_vol_2);
  int low_cell_voltage = concat(low_cell_vol_1, low_cell_vol_2);

  Serial.print(" Supply voltage: "); Serial.print(supply_voltage);
  //myNex.writeNum("dcCurrent.val", Pack_Current/100);
  Serial.print(" Low cell voltage: "); Serial.print(low_cell_voltage);
  //myNex.writeNum("packVoltage.val", Pack_Volt/100.0);
  //Serial.println(); 

  int average_cell_voltage = concat(avg_cell_vol_1, avg_cell_vol_2);

  Serial.print(" Low cell ID: "); Serial.print(low_cell_ID);
  Serial.print(" Average cell voltage: "); Serial.print(average_cell_voltage);
  //myNex.writeNum("rpm.val", ERPM/10.0);
  //Serial.println(); 

  int ERPM = concat(ERPM_3, ERPM_4);

  int duty_cycle = concat(duty_cycle_1, duty_cycle_2);
  int MC_input_voltage = concat(MC_input_voltage_1, MC_input_voltage_2);

  Serial.print(" ERPM: "); Serial.print(ERPM);
  if (ERPM == 0) {
    myNex.writeStr("speed.txt", 0);
  }
  else {
    myNex.writeStr("speed.txt", (65535 - ERPM)/420);
  }
  Serial.print(" Duty cycle: "); Serial.print(duty_cycle);
  Serial.print(" MC input voltage: "); Serial.print(MC_input_voltage);

  //Serial.println(); 

  int AC_current = concat(AC_current_1, AC_current_2);
  int DC_current = concat(DC_current_1, DC_current_2);

  Serial.print(" AC current: "); Serial.print(AC_current);
  //myNex.writeNum("cellTemp.val", cellTemp/10.0);
  Serial.print(" DC Current: "); Serial.print(DC_current);
  //Serial.println(); 

  int controller_temp = concat(controller_temp_1, controller_temp_2);
  int motor_temp = concat(motor_temp_1, motor_temp_2);

  Serial.print(" MC temperature: "); Serial.print(controller_temp);
  myNex.writeStr("MCTemp.txt", controller_temp/10);
  Serial.print(" Motor temperature: "); Serial.print(motor_temp);
  myNex.writeStr("motorTemp.txt", motor_temp/10);
  //Serial.println(); 


}


void canSniff(const CAN_message_t &msg) {


//   Serial.print("MB "); Serial.print(msg.mb);
//   Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
//   Serial.print("  LEN: "); Serial.print(msg.len);
//   Serial.print(" EXT: "); Serial.print(msg.flags.extended);
//   Serial.print(" TS: "); Serial.print(msg.timestamp);
//   Serial.print(" ID: "); Serial.print(msg.id, HEX);
//   Serial.print(" Buffer: ");
//   for ( uint8_t i = 0; i < msg.len; i++ ) {
//     Serial.print(msg.buf[i], HEX); Serial.print(" ");
//   } Serial.println();
// }

// INVERT COMMENTED SECTIONS TO JUST OUTPUT 


  if (msg.mb == 0) { //0x680
    for ( uint8_t i = 0; i < msg.len; i++ ) {
      switch (i) {
        case 0:
        pack_current_1 = msg.buf[i];
        break;
        case 1:
        pack_current_2 = msg.buf[i];
        break;
        case 2:
        pack_inst_vol_1 = msg.buf[i];
        break;
        case 3:
        pack_inst_vol_2 = msg.buf[i];
        break;
        case 4:
        pack_SOC = msg.buf[i];
        break;

      // DON'T CARE FOR NOW  
        // case 5:
        // relay_state_1 = msg.buf[i];
        // break;
      }
      //Serial.print(msg.buf[i], HEX); Serial.print(" ");
      inCanSniff = true;
    } 
      // int pack_voltage = concat(pack_inst_vol_1, pack_inst_vol_2);
      // int pack_current = concat(pack_current_1, pack_current_2);

      // Serial.print("Pack Current: "); Serial.println(pack_current);
      // //myNex.writeNum("dcCurrent.val", Pack_Current/100);
      // Serial.print("Pack Voltage: "); Serial.println(pack_voltage);
      // //myNex.writeNum("packVoltage.val", Pack_Volt/100.0);
      // Serial.print("Pack SOC: "); Serial.println(pack_SOC);
      // //myNex.writeNum("soc.val", Pack_SOC);
      // Serial.println(); 

      // INSERT CODE HERE TO SEND THIS INFO. TO DASH DISPLAY
  }




  if (msg.mb == 1) { //0x681
    for ( uint8_t i = 0; i < msg.len; i++ ) {
      switch (i) {
        case 0:
        high_temp = msg.buf[i];
        break;
        case 1:
        high_ID = msg.buf[i];
        break;
        case 2:
        avg_temp = msg.buf[i];
        break;
        case 3:
        input_supply_vol_1 = msg.buf[i];
        break;
        case 4:
        input_supply_vol_2 = msg.buf[i];
        break;
        case 5:
        low_cell_vol_1 = msg.buf[i];
        break;
        case 6:
        low_cell_vol_2 = msg.buf[i];
        break;
      }
      //Serial.print(msg.buf[i], HEX); Serial.print(" ");
      inCanSniff = true;
    } 
    // Serial.print("High temp: "); Serial.println(high_temp);
    // Serial.print("High ID: "); Serial.println(high_ID);
    // //myNex.writeNum("temp.val", High_Temp);
    // Serial.print("Avg. temp "); Serial.println(avg_temp);

    // int supply_voltage = concat(input_supply_vol_1, input_supply_vol_2);
    // int low_cell_voltage = concat(low_cell_vol_1, low_cell_vol_2);

    // Serial.print("Supply voltage: "); Serial.println(supply_voltage);
    // //myNex.writeNum("dcCurrent.val", Pack_Current/100);
    // Serial.print("Low cell voltage: "); Serial.println(low_cell_voltage);
    // //myNex.writeNum("packVoltage.val", Pack_Volt/100.0);
    // Serial.println(); 
  }


  if (msg.mb == 2) { //0xAFF
    for ( uint8_t i = 0; i < msg.len; i++ ) {
      switch (i) {
        case 0:
        low_cell_ID = msg.buf[i];
        break;
        // case 1:
        // avg_temp = msg.buf[i];
        // break;
        case 2:
        avg_cell_vol_1 = msg.buf[i];
        break;
        case 3:
        avg_cell_vol_2 = msg.buf[i];
        break;
      }
      //Serial.print(msg.buf[i], HEX); Serial.print(" ");
      inCanSniff = true;
    }
    // int average_cell_voltage = concat(avg_cell_vol_1, avg_cell_vol_2);

    // Serial.print("Low cell ID: "); Serial.println(low_cell_ID);
    // Serial.print("Average cell voltage: "); Serial.println(average_cell_voltage);
    // //myNex.writeNum("rpm.val", ERPM/10.0);
    // Serial.println(); 
  }


  if (msg.mb == 3) { //0x051
    for ( uint8_t i = 0; i < msg.len; i++ ) {
      switch (i) {
        case 0:
        ERPM_1 = msg.buf[i];
        break;
        case 1:
        ERPM_2 = msg.buf[i];
        break;
        case 2:
        ERPM_3 = msg.buf[i];
        break;
        case 3:
        ERPM_4 = msg.buf[i];
        break;
        case 4:
        duty_cycle_1 = msg.buf[i];
        break;
        case 5:
        duty_cycle_2 = msg.buf[i];
        break;
        case 6:
        MC_input_voltage_1 = msg.buf[i];
        break;
        case 7:
        MC_input_voltage_2 = msg.buf[i];
        break;
      }
      //Serial.print(msg.buf[i], HEX); Serial.print(" ");
      inCanSniff = true;
    }
    //ERPM is weird so will need to check how it works
    // int ERPM = concat(ERPM_3, ERPM_4);

    // int duty_cycle = concat(duty_cycle_1, duty_cycle_2);
    // int MC_input_voltage = concat(MC_input_voltage_1, MC_input_voltage_2);

    // Serial.print("ERPM: "); Serial.println(ERPM);
    // //myNex.writeNum("acCurrent.val", acCurrent/10.0);
    // Serial.print("Duty cycle: "); Serial.println(duty_cycle);
    // Serial.print("MC input voltage: "); Serial.println(MC_input_voltage);

    // Serial.println(); 
  }


  if (msg.mb == 4) { //0x151
    for ( uint8_t i = 0; i < msg.len; i++ ) {
      switch (i) {
        case 0:
        AC_current_1 = msg.buf[i];
        //Serial.println(ERPM_Pt1);
        break;
        case 1:
        AC_current_2 = msg.buf[i];
        break;
        case 2:
        DC_current_1 = msg.buf[i];
        break;
        case 3:
        DC_current_2 = msg.buf[i];
        break;
      }
      //Serial.print(msg.buf[i], HEX); Serial.print(" ");
      inCanSniff = true;
    }
    // int AC_current = concat(AC_current_1, AC_current_2);
    // int DC_current = concat(DC_current_1, DC_current_2);

    // Serial.print("AC current: "); Serial.println(AC_current);
    // //myNex.writeNum("cellTemp.val", cellTemp/10.0);
    // Serial.print("DC Current: "); Serial.println(DC_current);
    // Serial.println(); 
  }


  if (msg.mb == 5) {
    for ( uint8_t i = 0; i < msg.len; i++ ) {
      switch (i) {
        case 0:
        controller_temp_1 = msg.buf[i];
        break;
        case 1:
        controller_temp_2 = msg.buf[i];
        break;
        case 2:
        motor_temp_1 = msg.buf[i];
        break;
        case 3:
        motor_temp_2 = msg.buf[i];
        break;
        case 4:
        MC_fault_code = msg.buf[i];
        break;
      }
      //Serial.print(msg.buf[i], HEX); Serial.print(" ");
      inCanSniff = true;
    }
    // int controller_temp = concat(controller_temp_1, controller_temp_2);
    // int motor_temp = concat(motor_temp_1, motor_temp_2);

    // Serial.print("MC temperature: "); Serial.println(controller_temp);
    // Serial.print("Motor temperature: "); Serial.println(motor_temp);
    // Serial.println(); 
  }

}

// this appears to work correctly
int hexToDec(char hex[]){
     long long decimal;  
    int i = 0, val = 0, len;                  // variables declaration
    decimal = 0; 
    len = strlen(hex);  
    len--;  
  
    /* 
     * Iterate over each hex digit 
     */  
    for(i=0; hex[i]!='\0'; i++) {  
   
         /* Find the decimal representation of hex[i] */
        if(hex[i]>='0' && hex[i]<='9')
        {
            val = hex[i] - 48;
        }
        else if(hex[i]>='a' && hex[i]<='f')
        {
            val = hex[i] - 97 + 10;
        }
        else if(hex[i]>='A' && hex[i]<='F')
        {
            val = hex[i] - 65 + 10;
        }

        decimal += val * pow(16, len);
        len--; 
    } 
    return decimal;
}

// also appears to work correctly, given two ints that were converted from HEX, concating the equivalent int value of their concated HEX
long long concat(int a, int b){
  //int isOneDigitA = 0;
  int isOneDigitB = 0;
  long long dec;  
  char s1[20];
  char s2[20];
//char zeroA[20] = "0";
  char zeroB[20] = "0";
//onvert both the integers to hex
  sprintf(s1, "%X", a);
  sprintf(s2, "%X", b);
//  if(a/10 <= 0){
//     Serial.println("First");
//     isOneDigitA = 1;
//   }
   if(b/10 <= 0){
     //Serial.println("Second");
     isOneDigitB = 1;
     strcat(zeroB, s2);
   }
//     if(isOneDigitA==1){
//       strcat(zeroA, s1);
//     }
//     if(isOneDigitB == 1){
//       strcat(zeroB, s2);
//     }

//     if(isOneDigitA==1 && isOneDigitB == 1){
//       strcat(zeroA, zeroB);
//       dec = hexToDec(zeroA);
//       Serial.print("Pack part final: "); Serial.print(dec);
//       return dec;
//     }else if(isOneDigitA==1 && isOneDigitB == 0){
//       strcat(zeroA, s2);
//       dec = hexToDec(zeroA);
//       Serial.print("Pack part final: "); Serial.print(dec);
//       return dec;
//     }else 
      if(isOneDigitB == 1){
        strcat(s1, zeroB);
        //Serial.println(s1);
        dec = hexToDec(s1);
        //Serial.print("Pack part final: "); Serial.print(dec);
        return dec;
    }else{
      strcat(s1, s2);
    //change hex(s1) to decimal
    dec = hexToDec(s1);
    //Serial.print("Pack part final: "); Serial.print(dec);
    return dec;
    }
    // Serial.println(zeroA);
    // Serial.println(zeroB);
    //sprintf(s1, "%d", a);
    //sprintf(s2, "%d", b);
  
    // Concatenate both hex and store in s1
    //strcat(s1, s2);
    //change hex(s1) to decimal
    // long long dec = hexToDec(s1);
     //Serial.print("Pack part final: "); Serial.print(dec);
  
    // Convert the concatenated string
    // to integer
  
    // return the formed integer
   // return dec;
}
