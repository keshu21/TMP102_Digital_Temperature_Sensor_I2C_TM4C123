#include "TM4C123.h"                    // Device header
#include "i2c.h"
#include <stdint.h>
#include <stdio.h>


#define slave_address     0x48
#define high_temp_r  0x03
#define low_temp_r    0x02
#define confi_r       0x01
#define temp_r        0x00

/*
configure registure in sensor
4->em extended mode
5->AL ALERT BIT READ ONLY
6-7->CR0 CR1 SET CONVERSION RATE OF SENSOR
8->SD->SHUTDOWN MODE
9>TM->THERMOSTAT 1 FOR INERRUPT
10->POL->POLARITY 
11 12 F0-F1->FALTE QUEUE 
13 14 R0 R1-> RESOLUTION READ ONLY
15 OS->ONE SHORT OUTPUT*/


void GPIOB_Handler(void);

volatile uint16_t data[50]={0};
volatile uint16_t i=0;
volatile unsigned long  FallingEdges=0;
volatile uint16_t low_t=0,high_t=0,confi_regis=0;
int main(){
   I2c_Int();
	 Portf_Init();
   uint8_t reply=Write_Data(0x6A,0x80,confi_r,slave_address);//configure sensor
	 delay(1000);
	 if(reply==1){//check for error
		 PORTF_DATA_R=0x02;//red led
		 delay(16000000);
	 }
	 confi_regis=Read_Data(slave_address,confi_r);
	 reply=Write_Data(0x20,0x00,low_temp_r,slave_address);//low at 32
	 if(reply==1){//check for error
		 PORTF_DATA_R=0x02;//red led
	 }
	 reply=Write_Data(0x28,0x00,high_temp_r,slave_address);//high at 40
	 high_t=Read_Data(slave_address,high_temp_r);
	 if(high_t==1)//check for error
		 PORTF_DATA_R=0x02;
	 low_t=Read_Data(slave_address,low_temp_r);
	 if(low_t==1)//check for error
		 PORTF_DATA_R=0x02;
   while(1){
	 if(i<50){
		 uint16_t c_data=Read_Data(slave_address,temp_r);
		 if(c_data==1){//check for error
			 PORTF_DATA_R=0x02;
			 break;
		 } 
		 data[i]=c_data;
		 data[i]=(c_data>>4);
		 data[i]=data[i]*(625)/10000;
		 i++;
		 delay(4500000);//delay of t>0.25s
	 }
	 if(i<=50&&(data[i-1]>=(high_t>>4)*(625)/10000||data[i-1]<=(low_t>>4)*625/10000)){
	     PORTF_DATA_R=0x04;//on blue led
	 }
	 }

}
void GPIOB_Handler(void){
    PORTB_ICR_R = 0x01;        // Acknowledge flag1
    FallingEdges = FallingEdges + 1;
    PORTF_DATA_R = 0x04;      // Toggle PF2 (blue LED)
}
