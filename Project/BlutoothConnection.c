#include "MKL25Z4.h"                    // Device header

#define RED_LED   18  // PortB Pin 18
#define GREEN_LED 19  // PortB Pin 19
#define BLUE_LED  1   // PortD Pin 1
#define MASK(x) (1 << (x))

#define LED_RED 2 
#define LED_MASK(x) (x& 0x06)
#define BIT0_MASK(x) (x & 0x01)
#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

typedef enum  led_colors {
red_led = RED_LED, green_led = GREEN_LED, blue_led = BLUE_LED} led_colors_t;

char led_mapping[3][2] = {{0,red_led}, {1,green_led},{2,blue_led}};


typedef enum states 
{ led_off,
	led_on,
}my_state_t;

/* Delay Function */

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}



void initUART2(uint32_t baud_rate) {
  uint32_t divisor, bus_clock;
  
  // Enable Clocks
  SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  
  // Configure Mode 4 for UART pin operation
  // Alternative 4
  PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);
  
  PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
  
  // Clear TE and RE bits - disable trasmitter and receiver
  UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
  
  bus_clock = DEFAULT_SYSTEM_CLOCK / 2;
  // Divide by 16 due to oversampling
  divisor = bus_clock / (baud_rate * 16);
  // Set Baud Rate Register High and Low
  UART2->BDH = UART_BDH_SBR(divisor >> 8);
  UART2->BDL = UART_BDL_SBR(divisor);
  
  // Set Control and Status Registers to 0
  UART2->C1 = 0;
  UART2->S2 = 0;
  UART2->C3 = 0;
  
  // Enable TE and RE bits
  UART2->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;
}

void UART2_Transmit_Poll(uint8_t data){
  while(!(UART2->S1 & UART_S1_TDRE_MASK));
  UART2->D = data;
}

uint8_t UART2_Receive_Poll() {
  while (!(UART2->S1 & UART_S1_RDRF_MASK));
  return UART2->D;
}

void initLED(void)
{
  // Enable Clock to PORTB and PORTD
  SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
  // Configure MUX settings to make all 3 pins GPIO
  PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
  PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
  PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
  // Set Data Direction Registers for PortB and PortD
  PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
  PTD->PDDR |= MASK(BLUE_LED);
}

void offRGB() {
  // Set all (active low)
  PTB->PSOR |= MASK(RED_LED) | MASK(GREEN_LED);
  PTD->PSOR |= MASK(BLUE_LED);
}

void led_control(led_colors_t color, my_state_t state) {
  offRGB();
  // Turn on specific colour
  switch (color) {
  case RED_LED:
		if(state)
			PTB->PCOR = MASK(RED_LED);
		else
			PTB->PSOR = MASK(RED_LED);
    break;
  case GREEN_LED:
		if(state)
			PTB->PCOR = MASK(GREEN_LED);
		else
			PTB->PSOR = MASK(GREEN_LED);
    break;
  case BLUE_LED:
		if(state)
			PTD->PCOR = MASK(BLUE_LED);
		else 
			PTD->PSOR = MASK(BLUE_LED);
    break;
  }
}



int main(void) {
 uint8_t rx_data = 0x69;
	SystemCoreClockUpdate();
  initLED();
  offRGB();
  initUART2(BAUD_RATE);
  
  while (1) {
		rx_data = UART2_Receive_Poll();
		
		if(LED_MASK(rx_data) ==  LED_RED) {
			if (BIT0_MASK(rx_data)) {
				led_control(led_mapping[0][1],led_on);
			}
				else {
					led_control(led_mapping[0][1],led_off);
				}
			}
		}
			
  }
 
