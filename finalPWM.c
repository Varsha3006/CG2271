#include "MKL25Z4.h" 


#define PTB0_Pin 0
#define PTB1_Pin 1
#define MUSICAL_NOTE_CNT 7
#define FREQ_2_MOD(x) (375000 / x)

//int musical_notes[] = {262, 294, 330, 349, 392, 440, 494};
int musical_notes[] = {100, 400, 600, 800, 1000, 1200, 1400};


void initPWM() {
  // Enable Clock Gating for PORTB
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
  
  // Configure Mode 3 for PWM pin operation
  // Alternative 3
  PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
  
  PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
  
  // Enable Clock Gating for Timer1
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  
  // Select clock for TPM module
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // MCGFLLCLK or MCGPLLCLK/2
  
  // Set Modulo Value 20971520 / 128 = 163840 / 327 = 50 Hz
  //TPM1->MOD = 3276;
  
  // Set Modulo Value 48000000 / (128*50) = 50 Hz (50% duty cycle)
  TPM1->MOD = 7500; 
  
  // Edge-Aligned PWM
  // Update SnC register: CMOD = 01, PS=111 (128)
  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
  
  // Enable PWM on TPM1 Channel 0 -> PTB0
  TPM1_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
  TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  
  // Enable PWM on TPM1 Channel 1 -> PTB1
  TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
  TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

/* Delay Function */

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}


int main() {
  char i = 0;
  SystemCoreClockUpdate();
  initPWM();
  
  //TPM1_C0V = 0x0666; // 0x666 = 1638 (half of 3276) -> 50% Duty Cycle
  TPM1_C0V = 0x0EA6; // 0xEA6 = 3750 (half of 7500) -> 50% Duty Cycle
  TPM1_C1V = 0x753; // 0x0EA6;
  
  while (1) {
    for (i = 0; i < MUSICAL_NOTE_CNT; i++) {
      TPM1->MOD = FREQ_2_MOD(musical_notes[i]);
      TPM1_C0V = FREQ_2_MOD(musical_notes[i]) / 2;
      delay(0xFFFFFF);
    }
  }
}