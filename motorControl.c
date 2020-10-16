#include "MKL25Z4.h"                    // Device header
#define MAX_DUTY_CYCLE 0x1D4C //7500 (50Hz)
#define LEFT_FW 0 // PTB0 TPM1_CH0
#define LEFT_BK 1 // PTB1 TPM1_CH1
#define RIGHT_FW 2 // PTB2 TPM2_CH0
#define RIGHT_BK 3 // PTB3 TPM2_CH1

void initPWM() {
	// Enable Clock Gating for PORTB
	SIM->SCGC5 = (SIM_SCGC5_PORTB_MASK);

	// Configure Mode 3 for PWM pin operation
	PORTB->PCR[LEFT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_FW] |= PORT_PCR_MUX(3);
	PORTB->PCR[LEFT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_BK] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[RIGHT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_FW] |= PORT_PCR_MUX(3);
	PORTB->PCR[RIGHT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_BK] |= PORT_PCR_MUX(3);
	
	//Enable clock gating for Timer1
	SIM->SCGC6 = (SIM_SCGC6_TPM1_MASK)|(SIM_SCGC6_TPM2_MASK);
	
	//Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK OR MCGPLLCLK/2
	
	//set modulo value 48000000/128 = 375000, 375000Hz/50Hz = 7500	
	TPM1->MOD = 7500;
	TPM2->MOD = 7500;
	
	//Edge-Aligned PWM
	//CMOD - 1 and PS - 111 (128)
	TPM1_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM1_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)
  
	TPM2_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //CMOD = 1 => LPTPM counter increments on every LPTPM counter clock
	TPM2_SC &= ~(TPM_SC_CPWMS_MASK); //count up by default (0)
	
	//enable PWM on TPM1 channel 0 - PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM1 channel 1 - PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM2 channel 0 - PTB0
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//enable PWM on TPM2 channel 1 - PTB1
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}


// stop all PWM value
void stop() {	
		TPM1_C0V = 0;
		TPM1_C1V = 0;
		TPM2_C0V = 0;
		TPM2_C1V = 0;
}

/* Motor Base Functions */
void setLeftSpeed(int percentage) {
		if (left_dir == 0) {
				TPM2_C0V = (int) (((percentage * LEFT_MOTOR_COMP) / 10000.0) * 7499.0);
				TPM2_C1V = 0;
		} else {
				TPM2_C0V = 0;
				TPM2_C1V = (int) (((percentage * LEFT_MOTOR_COMP) / 10000.0) * 7499.0);
		}
}

void setRightSpeed(int percentage) {
		if (right_dir == 0) {
				TPM1_C0V = (int) (((percentage * RIGHT_MOTOR_COMP)/ 10000.0) * 7499.0);
				TPM1_C1V = 0;
		} else {
				TPM1_C0V = 0;
				TPM1_C1V = (int) (((percentage * RIGHT_MOTOR_COMP) / 10000.0) * 7499.0);
		}
}

void switchDir(void) {
		left_dir = (left_dir == 0 ? 1 : 0);
		right_dir = (right_dir == 0 ? 1 : 0);
}



/* Advanced Motor Functions */
// basic forward / backward
void forward(int percentage) {
		left_dir = 0; // set forward direction
		right_dir = 0; 
		setLeftSpeed(percentage);
		setRightSpeed(percentage);
}

void reverse(int percentage) {
		left_dir = 1; // set backward direction
		right_dir = 1; 
		setLeftSpeed(percentage);
		setRightSpeed(percentage);
}

// keep both wheels spinning, the other at 25% power
void turnLeft(int percentage) {
		left_dir = 0;
		right_dir = 0; 
		setLeftSpeed(percentage / 4);
		setRightSpeed(percentage - 40);
}

void turnRight(int percentage) {
		left_dir = 0;
		right_dir = 0; 
		setLeftSpeed(percentage - 40);
		setRightSpeed(percentage / 4);
}

// keep the other wheel stationary
void swingLeft(int percentage) {
		left_dir = 0;
		right_dir = 0; 
		setLeftSpeed(0);
		setRightSpeed(percentage);
}

void swingRight(int percentage) {
		left_dir = 0;
		right_dir = 0; 
		setLeftSpeed(percentage);
		setRightSpeed(0);
}

// pivot on the spot, both wheels in opposite directions
void pivotLeft(int percentage) {
		left_dir = 1;
		right_dir = 0; 
		setLeftSpeed(percentage);
		setRightSpeed(percentage);
}

void pivotRight(int percentage) {
		left_dir = 0;
		right_dir = 1; 
		setLeftSpeed(percentage);
		setRightSpeed(percentage);	
}

/* Delay Function */

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}


int main(void) {

 SystemCoreClockUpdate();
 initPWM();
	
 //TPM1_C0V = 0x0ea6; //50% duty cycle -> 7500/2 = 3750 in hex:0x0ea6
 //TPM1_C1V = 0x0753; //25% duty cycle -> 7500/4 = 1875 in hex:0x0753
 
 while(1) { // Forward, stop, backward, stop
   TPM1->MOD = 7500;
	 TPM1_C0V = MAX_DUTY_CYCLE/2; // 50% duty cycle
	 TPM2->MOD = 7500;
	 TPM2_C0V = MAX_DUTY_CYCLE/2; // 50% duty cycle
	 delay(0x80000);
	 stopMotors();
	 delay(0x80000);
	 TPM1->MOD = 7500;
	 TPM1_C1V = MAX_DUTY_CYCLE/2; // 50% duty cycle
	 TPM2->MOD = 7500;
	 TPM2_C1V = MAX_DUTY_CYCLE/2; // 50% duty cycle
	 delay(0x80000);
	 stopMotors();
	 delay(0x80000);
	}
}