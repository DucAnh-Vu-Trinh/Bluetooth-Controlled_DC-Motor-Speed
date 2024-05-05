#include <msp430.h> 

#include <stdio.h> 

#include <string.h> 

#include "LiquidCrystal_I2C.h" 

  

unsigned int TXBUF; 

unsigned int value; 

  

int main(void) 

{ 

    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer 

  

   // Configure UART 

   UCA1CTLW0 |= UCSWRST;  // Put eUSCI in reset 

   UCA1CTLW0 |= UCSSEL__ACLK;  // Clock source ACLK 

  

   // Baud Rate calculation for  9600 baud rate with ACLK = 32.768 kHz 

   UCA1BR0 = 3; 

   UCA1BR1 = 0; 

   UCA1MCTLW = 0x9200;  // from slides 

  

   // Configure UART pins 

   P4SEL1 &= ~BIT2; 

   P4SEL0 |= BIT2;  // Set P4.2 as UART RX 

  

   UCA1CTLW0 &= ~UCSWRST;  // Initialize eUSCI 

   UCA1IE |= UCRXIE;  // Enable RX interrupt 

  

   // Configure PWM on P6.1 

   P6DIR |= BIT1;  // P6.1 output 

   P6SEL0 |= BIT1;  // P6.1 options select (TB3.2) 

  

   // Setup Timer3_B CCR 

   TB3CCR0 = 100;  // PWM Period 

   TB3CCTL2 = OUTMOD_7;  // CCR2  reset  and set 

   TB3CCR2 = 0;  // Initial duty cycle 

  

  

   // Setup Timer3_B 

   TB3CTL = TBSSEL_1;  // ACLK 

   TB3CTL |= MC_1;     // up mode 

   TB3CTL |= TBCLR;    // clear TBR 

  

   I2C_Init(0x27); 

   LCD_Setup(); 

  

  

  

    // Enable interrupts 

    __enable_interrupt(); 

  

    // Main loop 

    while (1) 

    { 

        LCD_SetCursor(0,0); 

        LCD_Write("Speed: "); 

        LCD_SetCursor(0,1); 

        LCD_WriteNum(value); 

    } 

  

    return 0; 

} 

  

  

  

void I2C_Init(int addr){ 

    UCB1CTLW0 |= UCSWRST; 

    UCB1CTLW0 |= UCSSEL_3; 

    UCB1BRW = 10; 

    UCB1CTLW0 |= UCMODE_3; 

    UCB1CTLW0 |= UCMST; 

    UCB1CTLW0 |= UCTR; 

    UCB1I2CSA = addr; 

    UCB1CTLW1 |= UCASTP_2; 

    UCB1TBCNT = 0x01; 

    P4SEL1 &= ~BIT7; 

    P4SEL0 |= BIT7; 

    P4SEL1 &= ~BIT6; 

    P4SEL0 |= BIT6; 

    PM5CTL0 &= ~LOCKLPM5; 

    UCB1CTLW0 &= ~UCSWRST; 

    UCB1IE |= UCTXIE0; 

    __enable_interrupt(); 

  

} 

  

void I2C_Send (int value){ 

    UCB1CTLW0 |= UCTXSTT; 

    TXBUF = value; 

} 

  

void pulseEnable(int value){ 

    I2C_Send (value | En); 

    __delay_cycles(150); 

    I2C_Send(value & ~En); 

    __delay_cycles(1500); 

} 

  

void write4bits(int value){ 

    I2C_Send (value); 

    __delay_cycles(50); 

    pulseEnable(value); 

} 

  

void LCD_Send(int value, int mode){ 

    int high_b = value & 0xF0; 

    int low_b = (value << 4) & 0xF0; 

    write4bits(high_b | mode); 

    write4bits(low_b | mode); 

} 

  

void LCD_Write (char *text){ 

    unsigned int i; 

    for (i=0; i<strlen(text); i++){ 

        LCD_Send((int)text[i], Rs | LCD_BACKLIGHT); 

    } 

} 

  

void LCD_WriteNum(unsigned int num){ 

    if (num == 0) { 

        LCD_Send('0', Rs | LCD_BACKLIGHT); 

        return; 

    } 

  

    unsigned int reverseNum = 0; 

    int numDigits = 0; 

  

    // Process the number and count how many digits it has 

    while (num > 0) { 

        reverseNum = reverseNum * 10 + num % 10; 

        num /= 10; 

        numDigits++; 

    } 

  

    // Display the number on the LCD 

    while(numDigits > 0){ 

        LCD_Send((reverseNum % 10) + '0', Rs | LCD_BACKLIGHT); 

        reverseNum /= 10; 

        numDigits--; 

    } 

} 

  

void LCD_SetCursor(int col, int row){ 

    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54}; 

    LCD_Send(LCD_SETDDRAMADDR | (col +row_offsets[row]),LCD_BACKLIGHT); 

} 

  

void LCD_Setup(void){ 

  

    int _init[] = {LCD_init, LCD_init, LCD_init, LCD_4_BIT}; 

    int _setup[5]; 

    int mode = LCD_BACKLIGHT; 

  

    _setup[0] = LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS; 

    _setup[1] = LCD_CLEARDISPLAY; 

    _setup[2] = LCD_RETURNHOME; 

    _setup[3] = LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT; 

    _setup[4] = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF; 

  

    write4bits(_init[0]); 

    __delay_cycles(108000); 

    write4bits(_init[1]); 

    __delay_cycles(108000); 

    write4bits(_init[2]); 

    __delay_cycles(3600); 

    write4bits(_init[3]); 

  

    LCD_Send(_setup[0],mode); 

    LCD_Send(_setup[1],mode); 

    __delay_cycles(50); 

    LCD_Send(_setup[2],mode); 

    LCD_Send(_setup[3],mode); 

    LCD_Send(_setup[4],mode); 

  

} 

  

void LCD_ClearDisplay(void){ 

    LCD_Send(LCD_CLEARDISPLAY, LCD_BACKLIGHT); 

    __delay_cycles(2000); // A delay to ensure the clear command has been processed 

} 

  

  

#pragma vector = EUSCI_B1_VECTOR 

__interrupt void EUSCI_B1_I2C_ISR(void) 

{ 

    UCB1TXBUF = TXBUF; 

} 

  

  

  

#pragma vector=USCI_A1_VECTOR 

__interrupt void USCI_A1_ISR(void) 

{ 

    char receivedChar = UCA1RXBUF; 

  

    if (receivedChar >= '0' && receivedChar <= '9') 

    { 

        // Convert received character to numeric value 

        int numericValue = receivedChar - '0'; 

        // Map numeric value to PWM duty cycle (0 to 100% duty cycle) 

  

        TB3CCR2 = (numericValue * TB3CCR0) / 9; 

        value = numericValue; 

    } 

  

} 
