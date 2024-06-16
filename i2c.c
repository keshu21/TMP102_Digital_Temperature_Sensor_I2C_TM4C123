 #include "TM4C123.h"  // Device header
#include <stdint.h>
#include <stdlib.h>

#include "i2c.h"

void I2c_Int(void){
    RCGCI2C|=0x01;//enable i2c0
	  RCGCGPIO|=0x02;//enable port b
    PORTB_AFSEL_R |= 0x0C;
    PORTB_ODR_R |= 0x08; // Open drain for SDA
    PORTB_DEN_R |= 0x0C;
    PORTB_PCTL_R |= 0x3300;
    PORTB_AMSEL_R &= ~0x0C;

    I2C_MCR_R = 0x10; // Initialize master function
    I2C_MTPR_R = 7; // Set clock speed to 100kbps
}

void Portf_Init(void){
   RCGCGPIO |= 0x20; // Enable GPIO Port F clock
    PORTF_DIR_R = 0x0E; // PF1, PF2, and PF3 as output (LEDs)
    PORTF_DEN_R |= 0x0E; // Digital enable PF1, PF2, and PF3
    PORTF_AFSEL_R &= ~0x0E; // Disable alternate function
    PORTF_PCTL_R &= ~0xFFF0; // Configure as GPIO
    PORTF_AMSEL_R &= ~0x0E; // Disable analog function
}

uint8_t Write_Data(uint8_t data1,uint8_t data2,uint8_t pointer_r,uint8_t slave){
    I2C_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
   I2C_MSA_R &= ~0x01;             // MSA[0] is 0 for send
   I2C_MDR_R = pointer_r;
	I2C_MCS_R = (0
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                     //  & ~I2C_MCS_STOP    // no stop
                       | I2C_MCS_START    // generate start/restart
                       | I2C_MCS_RUN);    // master enable
	while(I2C_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
		if((I2C_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C_MCS_R = (0                // send stop if nonzero
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // stop
                     //  & ~I2C_MCS_START   // no start/restart
                     //  & ~I2C_MCS_RUN   // master disable
                       );   
                                          // return error bits if nonzero
    return (1);
  }
	I2C_MDR_R = data1&0xFF;         // prepare second byte
  I2C_MCS_R = (0
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                     //  & ~I2C_MCS_STOP    // no stop
                     //  & ~I2C_MCS_START   // no start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C_MCS_R = (0                // send stop if nonzero
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // stop
                     //  & ~I2C_MCS_START   // no start/restart
                     //  & ~I2C_MCS_RUN   // master disable
                        );
                                          // return error bits if nonzero
    return (1);
  }
  I2C_MDR_R = data2&0xFF;         // prepare third byte
  I2C_MCS_R = (0
                    //   & ~I2C_MCS_ACK     // no data ack (no data on send)
                       | I2C_MCS_STOP     // generate stop
                    //   & ~I2C_MCS_START   // no start/restart
                       | I2C_MCS_RUN);    // master enable
  while(I2C_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // return error bits
	if((I2C_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0)
  return (1);	
	else
		return 0;

}

uint16_t Read_Data(uint8_t slave,uint8_t pointer_reg){
//FIRST SEND VALUE OF POINTER REGISTER
	 uint8_t data1,data2;
    I2C_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
   I2C_MSA_R &= ~0x01;             // MSA[0] is 0 for send
   I2C_MDR_R = pointer_reg;
	I2C_MCS_R = (0
                     //  & ~I2C_MCS_ACK     // no data ack (no data on send)
                       |I2C_MCS_STOP    //  stop
                       | I2C_MCS_START    // generate start/restart
                       | I2C_MCS_RUN);    // master enable
	while(I2C_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
		if((I2C_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0)
    return (1);
			
		//RECIEVE DATA
		I2C_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
   I2C_MSA_R |= 0x01;
		I2C_MCS_R = (0
                         | I2C_MCS_ACK      // positive data ack
                       //  & ~I2C_MCS_STOP    // no stop
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
			if((I2C_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){//CHECK FOR ERROR
				I2C_MCR_R=0|I2C_MCS_STOP;//SEND STOP BIT;
    return (1);//RETURN ERROR;
			}
    data1 = (I2C_MDR_R&0xFF);       // MSB data sent first
    I2C_MCS_R = (0
                       //  & ~I2C_MCS_ACK     // negative data ack (last byte)
                         | I2C_MCS_STOP     // generate stop
                       //  & ~I2C_MCS_START   // no start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
			if((I2C_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){//CHECK FOR ERROR
				I2C_MCR_R=0|I2C_MCS_STOP;//SEND STOP BIT;
    return (1);//RETURN ERROR;
			}
    data2 = (I2C_MDR_R&0xFF);
		return (data1<<8)|data2;
		
 }

 void delay(unsigned long t){
 unsigned long i=t;
	 while(i>0){i--;};
 
 }

 void EnableInterrupts(void){
    __enable_irq();
}
 
void Portb_init(){
      RCGCGPIO|=0x02;//enable port b
	    PORTB_DIR_R&=~0x01;//make it as input
	    PORTB_DEN_R|=0x01;
	    //PORTB_PCTL_R&=~0xF;
	    PORTB_PUR_R|=0x01;
	    
	PORTB_IS_R&=~0x01;//set a edge/level(0/1) sensitive
	PORTB_IBE_R&=~0x01;//not both edge 
	PORTB_IEV_R&=~0x01;//set as a falling/rising (0/1) 
	PORTB_IM_R|=0x01;//MAKE INTERRUPT AT PIN 0
	PORTB_ICR_R=0x01;//CLEAR PREVOIUS INTERRUPT 
	NVIC_PRI0=(NVIC_PRI0&0xFFFF0FFF)|0x00002000;//set priority as 2
	NVIC_EN0|=0x02;//ENABLE GPIO INTERRUPT FOR PORT B
	EnableInterrupts();//ENABLE GLOBLE INTERRUPT 
}




 





	
	
	
	
	
