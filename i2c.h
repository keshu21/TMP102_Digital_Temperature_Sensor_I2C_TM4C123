#include "TM4C123.h"                    // Device header

#define RCGCI2C (*((volatile unsigned long*)(0x400FE000+0x620)))
#define RCGCGPIO (*((volatile unsigned long*)(0x400FE000+0x608)))
	
#define PORTB_BASE  0x40005000
#define PORTB_AFSEL_R  (*((volatile unsigned long*) (PORTB_BASE + 0x420)))
#define PORTB_PCTL_R   (*((volatile unsigned long*) (PORTB_BASE + 0x52C)))
#define PORTB_DIR_R    (*((volatile unsigned long*) (PORTB_BASE + 0x400)))
#define PORTB_DEN_R    (*((volatile unsigned long*) (PORTB_BASE + 0x51C)))
#define PORTB_DATA_R   (*((volatile unsigned long*) (PORTB_BASE + 0x3FC)))
#define PORTB_AMSEL_R  (*((volatile unsigned long*) (PORTB_BASE + 0x528)))
#define PORTB_ODR_R    (*((volatile unsigned long*) (PORTB_BASE + 0x50C)))
	
#define PORTB_IS_R          (*((volatile unsigned long*)(PORTB_BASE + 0x404)))
#define PORTB_IBE_R         (*((volatile unsigned long*)(PORTB_BASE + 0x408)))
#define PORTB_IEV_R         (*((volatile unsigned long*)(PORTB_BASE + 0x40C)))
#define PORTB_IM_R         (*((volatile unsigned long*)(PORTB_BASE + 0x410)))
#define PORTB_RIS_R         (*((volatile unsigned long*)(PORTB_BASE + 0x414)))
#define PORTB_ICR_R        (*((volatile unsigned long*)(PORTB_BASE + 0x41C)))
#define PORTB_MIS_R        (*((volatile unsigned long*)(PORTB_BASE + 0x418)))
#define PORTB_PUR_R        (*((volatile unsigned long*) (PORTB_BASE + 0x510)))


#define I2C0_BASE      (0x40020000)
#define I2C_MSA_R      (*((volatile unsigned long*) (I2C0_BASE + 0x00)))
#define I2C_MCS_R      (*((volatile unsigned long*) (I2C0_BASE + 0x004)))
#define I2C_MDR_R      (*((volatile unsigned long*) (I2C0_BASE + 0x008)))
#define I2C_MTPR_R     (*((volatile unsigned long*) (I2C0_BASE + 0x00C)))
#define I2C_MCR_R      (*((volatile unsigned long*) (I2C0_BASE + 0x020)))
	
#define PORTF_BASE    0x40025000
#define PORTF_DIR_R   (*((volatile unsigned long*) (PORTF_BASE + 0x400)))
#define PORTF_DEN_R   (*((volatile unsigned long*) (PORTF_BASE + 0x51C)))
#define PORTF_DATA_R  (*((volatile unsigned long*) (PORTF_BASE + 0x3FC)))
#define PORTF_AFSEL_R (*((volatile unsigned long*) (PORTF_BASE + 0x420)))
#define PORTF_PCTL_R  (*((volatile unsigned long*) (PORTF_BASE + 0x52C)))
#define PORTF_AMSEL_R (*((volatile unsigned long*) (PORTF_BASE + 0x528)))

	
#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable

void I2c_Int(void);
void Portf_Init(void);
uint8_t Write_Data(uint8_t data1,uint8_t data2,uint8_t pointer_r,uint8_t slave);
uint16_t Read_Data(uint8_t slave,uint8_t pointer_reg);
void delay(unsigned long t);
//uint8_t Set_Temp(uint16_t low_temp,uint16_t high_temp,uint8_t slave_address);
//uint16_t Find_High_Temp(uint8_t slave_address);
//uint16_t Find_Low_Temp(uint8_t slave_address);



// for gpiob interrupt irq number is 1

#define NVIC_EN0  (*((volatile unsigned long*) ( 0xE000E000 + 0x528)))  //bit 1 for gpiob
#define NVIC_PRI0  (*((volatile unsigned long*) ( 0xE000E000 + 0x400))) //BIT 13-14-15
	
void EnableInterrupts(void);
void Portb_init(void);

