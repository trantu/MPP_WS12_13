
//=========================================================================
//=== Startprojekt zum Mikroprozessorpraktikum ============================
//=========================================================================
#include "msp430x16x.h"  	// Systemdefinitionen für den MSP430F1612
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
#include "math.h"			// Standard Libary

//#include "aufgabe_x.h"	// Header Datei für aufgabe_x.c 
							// möglichst eigene xxx.h und xxx.c Dateien
							// erstellen und verwenden.
main(void);					//Haupprogramm
void print_value(void);

#define BIT_SET(a,b) ((a) |= (b))
#define BIT_CLR(a,b) ((a) &= ~(b))
#define BIT_TOGGLE(a,b) ((a) ^= (b))

//====Hauptprogramm=======================================================


int values[100];

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

	memset(values, 0, 100*sizeof(int));

	// waehle ACLK (32.7 kHz ohne divider)
	BIT_CLR(TBCTL, TBSSEL0);
	
	// wahle count up mode
	BIT_SET(TBCTL, MC0);
	BIT_CLR(TBCTL, MC1);
	
	// setze anzahl fuer 100Hz Trigger
	TBCCR0 = 327; // 32.7kHz: takt=32_768 / 100 = count to 32
	//TBR = 1;
	
	// loesche interrupt flag fuer timer
	BIT_CLR(TBCCTL0, CCIFG);

	P6SEL |= BIT7; // P6.7 als input
	ADC12CTL0 = ADC12ON + MSC;
	// Trigger Timer_B.OUT0, SMCLK
	ADC12CTL1 = CONSEQ1 + SHP;
	ADC12MCTL0 = INCH_7;
	ADC12IFG = 0;
	
	// erlaube konversion
	ADC12CTL0 |= ENC;
	
	// DMA konfigurieren
	DMA2SZ = 100;
	DMA2SA = (unsigned int)&ADC12MEM0; // speichere die Addresse des ADC Register
	DMA2DA = (unsigned int)values; // und die unserer Werte
	DMACTL0 = DMA2TSEL2 + DMA2TSEL1; // Trigger kommt vom ADC
	// kopiere 1 Word aus dem ADC Register und erhoehe dst um eins
	DMA2CTL = DMAEN + DMADSTINCR0 + DMADSTINCR1;
	
	// Bescheid sagen, wenn der Transfer fertig ist
	BIT_CLR(DMA2CTL, DMAIFG);
	DMA2CTL += DMAIE;

	ADC12CTL0 |= ADC12SC;
	_bis_SR_register(GIE);

//===Hier die Endlosschleife quasi das Betriebssystem=====================

while(1){
}	// Ende der Endlosschleife
}	// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = DACDMA_VECTOR
__interrupt void DMA(void)
{
	double val = 0.0;
	int i;
	char buffer[16];

	LED_ON(LED_ROT);
	
	for (i = 0; i < 100; i++) {
		double this = values[i];
		
		this /= 4096.0f; // Wertbreite
		this *= 3.3f; // Referenzspannung
		
		val += this/100.0f;
	}

	sprintf(buffer, "U=%.2f V", val);
	lcd_clear(WHITE);
	lcd_string(BLACK, 15, 25, buffer);
	lcd_paint();
	BIT_CLR(DMA2CTL, DMAIFG);
	LED_OFF(LED_ROT);
	//Problem: Es wird nur einmal aufgerufen
	DMA2CTL |= DMAEN;
	BIT_CLR(ADC12IFG, 1);
	//ADC12CTL0 |= ADC12SC;
}
