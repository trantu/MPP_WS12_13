
//=========================================================================
//=== Startprojekt zum Mikroprozessorpraktikum ============================
//=========================================================================
#include "msp430x16x.h"		// Systemdefinitionen für den MSP430F1612
#include "init.h"			// Initialisierung des Mikrocontrollers
#include "system.h"			// Systemfunktionen MSB430H
#include "interrupts.h"		// ISR - Interrupt Service Routinen

#include "HW_CC1100.h"		// CC1100 868MHz Funk Transceiver
#include "HW_SHT11.h"		// SHT11 Feuchte- und Temperatursensor
#include "HW_MMA7260Q.h"	// MMA7260Q 3D-Beschleunigungssensor
#include "HW_LCD_TS.h"		// DOGM128 LCD Display Touchscreen
#include "HW_LCD.h"			// DOGM128 LCD Display 

#include "stdio.h"			// Standard Libary 
#include "string.h"			// Standard Libary 
//#include "math.h"			// Standard Libary

//#include "aufgabe_x.h"	// Header Datei für aufgabe_x.c 
							// möglichst eigene xxx.h und xxx.c Dateien
							// erstellen und verwenden.
main(void);					//Haupprogramm
void print_value(void);

//====Hauptprogramm=======================================================

main(void)
	{
//===Hier sollten Variablen deklariert werden ============================
	//unsigned char i = 0;
	//char text[60];
	//int x,y;
	
//===Hier die notwendigen Initialisierungsschritte =======================
//=(1)== Port-Initialisierung ============================================
	init_Port();				// Initialisierung der Port Register
		
//(2)=== Clock-System-Initialisierung ====================================
	//== XT2() oder Dco() als Taktquelle einstellen
	//== durch Ein- oder Auskommentieren
	//== DCO ist bei LPM Einsatz bevorzugt muß zyklisch kalibriert werden
	//== XT2 ist quarzstabil muß nicht zyklisch kalibriert werden
	//  			
	//XT2 ();					// XT2 Taktquelle aktivieren mit 7.3728MHz
	DCO ();					// Dco Taktquelle aktivieren mit 7.3728MHz
	//						   beachte DELTA
		
//=(3)== Timer-Initialisierung= ==========================================
	init_Timer_A();			// Init Timer für Sekundeninterrupt
							// !! noch leere Funktion

//=(4)== USART-Initialisierung ===========================================
	init_UART1();			// UART-RS232 mit 115.2kBit/s initialisieren
							// !! noch leere Funktion

//=(5)== CC1100-Transceiver-Initialisierung ==============================
	init_UART0_SPI();		// CC1100 SPI UART initalisieren
	init_CC1100_POWERDOWN();			// CC1100 init und in RX Mode setzen
							// !!!Interrupte sind ab jetzt freigegeben!!
	//== Adresse und Funkkanal des Transceivers setzen
	//== für die Arbeitsplaetze HWPx (x=1...10) sollten
	//== ID=x und channnel=x gesetzt werden
	ID = 1;							// Adresse 
	setUid(ID);						// Adresse im Transceiver setzen 
	channel = 1; 					// Funkkanal  
	switchFreq(channel);			// Funkkanal im Transceiver setzen 
	//== Soll der Transceiver genutzt werden müssen folgende zwei Zeilen  
	//== auskommentiert werden: 
	init_CC1100_IDLE();		// CC1100 in den IDLE Mode setzen
	init_CC1100_POWERDOWN();// CC1100 in den PowerDown Mode setzen

//=(6)== LCD-Display-Initialisierung =====================================
	dogm_reset();		// Hardware Reset des LCD Controllers
	dogm_init();		// Initialisierung der LCD Controller Register
	lcd_clear(WHITE);	// Grafikspeicher auf dem MSP430 löschen
	//lcd_string(BLACK, 15, 25, "MSP430-GESTARTET!");	// Textausgabe
	lcd_paint();		// Grafikspeicher auf das LCD Display ausgeben

#define BIT_SET(a,b) ((a) |= (b))
#define BIT_CLR(a,b) ((a) &= ~(b))
#define BIT_TOGGLE(a,b) ((a) ^= (b))

#define LED_ROT (0x01)   // 0 0 1 P4.0
#define LED_GELB (0x02)  // 0 1 0 P4.1
#define LED_GRUEN (0x04) // 1 0 0 P4.2
#define LED_ALL (LED_ROT | LED_GELB | LED_GRUEN)

#define LED_ON(led) (BIT_CLR(P4OUT, led)) 
#define LED_OFF(led) (BIT_SET(P4OUT, led)) 
#define LED_TOGGLE(led) (BIT_TOGGLE(P4OUT, led))

#define IS_LED_ON(led) (!(P4OUT & led))

#define TASTE_LINKS (0x1)
#define TASTE_RECHTS (0x2)

#define SLEEP_QUANTUM 10000
#define SLEEP(n) do { 		/* sleep for n seconds */ \
	long time = n * 100000; /* wait() sleeps 10*n microseconds */ \
	while(time > SLEEP_QUANTUM) { \
		wait(SLEEP_QUANTUM); \
		time -= SLEEP_QUANTUM; \
	} \
	wait(time); \
} while(0)

// waehle ACLK
BIT_SET(TBCTL, TBSSEL0);
BIT_CLR(TBCTL, TBSSEL1);

// waehle Set/Reset
BIT_SET(TBCCTL1, OUTMOD1 + OUTMOD0);

// setze divider auf 8
//BIT_SET(TBCTL, ID0 | ID1);

// wahle count up mode
BIT_SET(TBCTL, MC0);
BIT_CLR(TBCTL, MC1);

// setze taktanzahl fuer eine halbe sekunde
TBCCR0 = 1024; // eine sekunde: takt=32000 / divider=8
TBCCR1 = 512;

TBR = 1;

// setze Pin 6.4, 6.5, 6.6 als output
BIT_CLR(P5DIR, BIT4 + BIT5 + BIT6);
// Beschleunigungssensor g-Range 1.5g, Sensitivity 800mV/g
BIT_CLR(P5OUT, BIT4);
BIT_CLR(P5OUT, BIT5);
// sleep mode off
BIT_SET(P5OUT, BIT6);

// waehle P6.0, P6.1, P6.2 als input
P6SEL = BIT0 + BIT1 + BIT2;
// schalte wandler an, waehle "multiple sample and conversion", SHT = 0 -> 4 cycles
ADC12CTL0 = ADC12ON + MSC;
// ADC12 clock, sequence of channels, timer B1
ADC12CTL1 = CONSEQ0 + SHS1 + SHS0 + SHP;
// waehle channel A0, A1, A2
ADC12MCTL0 = INCH_0;
ADC12MCTL1 = INCH_1;
ADC12MCTL2 = INCH_2 + EOS;
// loesche interrupt flag
BIT_CLR(ADC12IFG, BIT2);
// erlaube interrupt
BIT_SET(ADC12IE, BIT2);

// erlaube conversion
ADC12CTL0 |= ENC;

_bis_SR_register(GIE);
//===Hier die Endlosschleife quasi das Betriebssystem=====================
	while(1){
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

void print_buf(const char *str)
{
	do {
		U1TXBUF = *str;
		/*
		 * (115200 / 8)**-1 = 70us pro Zeichen
		 */
		wait(7);
	} while(*str++);
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12(void)
{
	char buffer[32];
	int x, y, z;
	LED_TOGGLE(LED_GRUEN);
	if (ADC12IFG & BIT2)
	{
		LED_TOGGLE(LED_ROT);
		x = ADC12MEM0;
		y = ADC12MEM1;
		z = ADC12MEM2;
		
		lcd_clear(WHITE);
		sprintf(buffer, "ax=%d", x);
		lcd_string(BLACK, 16, 16, buffer);
		sprintf(buffer, "ay=%d", y);
		lcd_string(BLACK, 16, 25, buffer);
		sprintf(buffer, "az=%d", z);
		lcd_string(BLACK, 16, 33, buffer);
		
		sprintf(buffer, "%d\t%d\t%d\r\n", x, y, z);
		
		print_buf(buffer);
		
		lcd_paint();
		BIT_CLR(ADC12IFG, BIT2 + BIT1 + BIT0);
	}	
}
