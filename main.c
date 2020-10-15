
 

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define RED_LED   18  // PortB Pin 18
#define GREEN_LED 19  // PortB Pin 19
#define BLUE_LED  1   // PortD Pin 1
#define MASK(x) (1 << (x))
#define LED_MASK(x) (x& 0x06)
#define BIT0_MASK(x) (x & 0x01)
#define LED_RED 2 
#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

//The green 8 segment LED
#define PTC7_Pin 7
#define PTC0_Pin 0
#define PTC3_Pin 3
#define PTC4_Pin 4
#define PTC5_Pin 5
#define PTC6_Pin 6
#define PTC10_Pin 10
#define PTC11_Pin 11

//The green 8 segment LED
#define PTE30_Pin 30
#define PTE29_Pin 29
#define PTE23_Pin 23
#define PTE22_Pin 22
#define PTE21_Pin 21
#define PTE20_Pin 20
#define PTE5_Pin 5
#define PTE4_Pin 4

//configuration of green led segment
#define FRONT_LED_0 PTC7_Pin
#define FRONT_LED_1 PTC0_Pin
#define FRONT_LED_2 PTC3_Pin
#define FRONT_LED_3 PTC4_Pin 
#define FRONT_LED_4 PTC5_Pin 
#define FRONT_LED_5 PTC6_Pin 
#define FRONT_LED_6 PTC10_Pin 
#define FRONT_LED_7 PTC11_Pin 


typedef enum  green_LED_segment {
	fled0 = FRONT_LED_0, 
	fled1 = FRONT_LED_1,  
	fled2 = FRONT_LED_2, 
	fled3 = FRONT_LED_3, 
	fled4 = FRONT_LED_4,  
	fled5 = FRONT_LED_5, 
	fled6 = FRONT_LED_6, 
	fled7 = FRONT_LED_7
} green_LED_segment_t;


typedef enum states 
{ led_off,
	led_on,
}my_state_t;
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

void turnOffGreen() {
	PTC->PCOR |= (MASK(FRONT_LED_0) | MASK(FRONT_LED_1) | MASK(FRONT_LED_2) | MASK(FRONT_LED_3) |	MASK(FRONT_LED_4) |
					MASK(FRONT_LED_5) | MASK(FRONT_LED_6) | MASK(FRONT_LED_7));
}

void turnOnGreen(void) {
	PTC->PDOR |= (MASK(FRONT_LED_0) | MASK(FRONT_LED_1) | MASK(FRONT_LED_2) | MASK(FRONT_LED_3) |	MASK(FRONT_LED_4) |
					MASK(FRONT_LED_5) | MASK(FRONT_LED_6) | MASK(FRONT_LED_7));
}



void initLED(void)
{
  // Enable Clock to PORTB and PORTD
  SIM->SCGC5 |= ((SIM_SCGC5_PORTC_MASK) | (SIM_SCGC5_PORTE_MASK));
	
  // Configure MUX settings to make all 16 pins GPIO
		PORTC->PCR[FRONT_LED_0] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_0] |= PORT_PCR_MUX(1);
		PORTC->PCR[FRONT_LED_1] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_1] |= PORT_PCR_MUX(1);
		PORTC->PCR[FRONT_LED_2] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_2] |= PORT_PCR_MUX(1);
		PORTC->PCR[FRONT_LED_3] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_3] |= PORT_PCR_MUX(1);
		PORTC->PCR[FRONT_LED_4] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_4] |= PORT_PCR_MUX(1);
		PORTC->PCR[FRONT_LED_5] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_5] |= PORT_PCR_MUX(1);
		PORTC->PCR[FRONT_LED_6] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_6] |= PORT_PCR_MUX(1);
		PORTC->PCR[FRONT_LED_7] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[FRONT_LED_7] |= PORT_PCR_MUX(1);
	
	/*
  	PORTE->PCR[GREEN_LED1] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED1] |= PORT_PCR_MUX(1);
		PORTE->PCR[GREEN_LED2] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED2] |= PORT_PCR_MUX(1);
		PORTE->PCR[GREEN_LED3] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED3] |= PORT_PCR_MUX(1);
		PORTE->PCR[GREEN_LED4] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED4] |= PORT_PCR_MUX(1);
		PORTE->PCR[GREEN_LED5] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED5] |= PORT_PCR_MUX(1);
		PORTE->PCR[GREEN_LED6] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED6] |= PORT_PCR_MUX(1);
		PORTE->PCR[GREEN_LED7] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED7] |= PORT_PCR_MUX(1);
		PORTE->PCR[GREEN_LED8] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[GREEN_LED8] |= PORT_PCR_MUX(1);
		*/
		
  // Set Data Direction Registers for PortC and PortE
		PTC->PDDR |= (MASK(FRONT_LED_0) | MASK(FRONT_LED_1) | MASK(FRONT_LED_2) | MASK(FRONT_LED_3) |	MASK(FRONT_LED_4) |
					MASK(FRONT_LED_5) | MASK(FRONT_LED_6) | MASK(FRONT_LED_7));
		//PTE->PDDR |= (MASK(FRONT_LED_) | MASK(GREEN_LED2) | MASK(GREEN_LED3) |	MASK(GREEN_LED4) |
					//MASK(GREEN_LED5) | MASK(GREEN_LED6) | MASK(GREEN_LED7) | MASK(GREEN_LED8) );
		//PTC->PDDR |= MASK(RED_LED8);
		turnOffGreen();
	//turnOffRed();
}


/** Turns on the green led at the specified index */
void sequenceGreen(green_LED_segment_t led_count) {
	PTC->PDOR = MASK(led_count);
}

void moving_led_display(void) {
	static green_LED_segment_t count = fled0;
	//toggleRed();
	sequenceGreen(count);
	osDelay(500);
	if(count == fled7){
		count = fled0;
	} else {
		count++;
	}
	//count %= 8;
}

/** Thread to light up green LED's sequentially when the robot is moving */
void tRunningLed(void *argument) { //tRunning priority > tStatic
	for (;;) {
		moving_led_display();
	}
}



/*void led_red_thread(void *argument) {
 
  // ...
  for (;;) {
	  led_control(RED_LED,led_on);
		osDelay(1000);
		led_control(RED_LED,led_off);
		osDelay(1000);
	}
}

void led_green_thread(void *argument) {
 
  // ...
  for (;;) {
	  led_control(GREEN_LED,led_on);
		osDelay(1000);
		led_control(GREEN_LED,led_off);
		osDelay(1000);
	}
}
*/

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initLED();
	//offRGB();
  // ...
 
  osKernelInitialize();
  //osThreadNew(tStaticLED, NULL, NULL);	// Initialize CMSIS-RTOS
  osThreadNew(tRunningLed, NULL, NULL);    // Create application main thread
  osKernelStart();                      // Start thread execution
  for (;;) {}
}


 

