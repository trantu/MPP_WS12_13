
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


mode_t mode;

unsigned char secs = 0;

#define BIT_SET(a,b) ((a) |= (b))
#define BIT_CLR(a,b) ((a) &= ~(b))
#define BIT_TOGGLE(a,b) ((a) ^= (b))

//====Hauptprogramm=======================================================


main(void)
	{
		
	mode = RECEIVER;
		
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
	init_CC1100();			// CC1100 init und in RX Mode setzen
							// !!!Interrupte sind ab jetzt freigegeben!!
	//== Adresse und Funkkanal des Transceivers setzen
	//== für die Arbeitsplaetze HWPx (x=1...10) sollten
	//== ID=x und channnel=x gesetzt werden
	if (SENDER == mode) {
		ID = 18;						// Adresse 
		channel = 8; 					// Funkkanal  
	} else {
		ID = 8;							// Adresse 
		channel = 8; 					// Funkkanal  
	} 
	setUid(ID);						// Adresse im Transceiver setzen 
	switchFreq(channel);			// Funkkanal im Transceiver setzen
	//== Soll der Transceiver genutzt werden müssen folgende zwei Zeilen  
	//== auskommentiert werden: 
	/*init_CC1100_IDLE();		// CC1100 in den IDLE Mode setzen
	init_CC1100_POWERDOWN();*/// CC1100 in den PowerDown Mode setzen

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


	if (SENDER == mode) {
		// interrupt wird bei 0 ausgeloest
		TBR = 1;
		
		// waehle ACLK
		BIT_SET(TBCTL, TBSSEL0);
		BIT_CLR(TBCTL, TBSSEL1);
		
		// setze divider auf 8
		BIT_SET(TBCTL, ID0 | ID1);
		
		// wahle count up mode
		BIT_SET(TBCTL, MC0);
		BIT_CLR(TBCTL, MC1);
		
		// setze taktanzahl fuer eine sekunde
		TBCCR0 = 0x1000;
		
		// loesche interrupt flag fuer timer
		BIT_CLR(TBCCTL0, CCIFG);
		
		// AD Wandler Konfiguration
		
		// setze Pin 5.4, 5.5, 5.6 als output
		BIT_SET(P5DIR, BIT4 + BIT5 + BIT6);
		// Beschleunigungssensor g-Range 1.5g, Sensitivity 800mV/g
		BIT_CLR(P5OUT, BIT4);
		BIT_CLR(P5OUT, BIT5);
		// sleep mode off
		BIT_SET(P5OUT, BIT6);
		
		// waehle P6.0, P6.1, P6.2 als input
		P6SEL = BIT0 + BIT1 + BIT2;
		// schalte wandler an, waehle "multiple sample and conversion", SHT = 0 -> 4 cycles
		ADC12CTL0 = ADC12ON + MSC;
		// ADC12 clock, sequence of channels
		ADC12CTL1 = CONSEQ0 + SHP;
		// waehle channel A0, A1, A2
		ADC12MCTL0 = INCH_0;
		ADC12MCTL1 = INCH_1;
		ADC12MCTL2 = INCH_2 + EOS;
		
		// loesche interrupt flag
		BIT_CLR(ADC12IFG, BIT2);
		
		// erlaube interrupt
		BIT_SET(ADC12IE, BIT2);
		
		// erlaube timer interrupt
		BIT_SET(TBCCTL0, CCIE);
		
		// erlaube conversion
		ADC12CTL0 |= ENC;
	}
	
	LED_OFF(LED_ALL);

	_bis_SR_register(GIE);

//===Hier die Endlosschleife quasi das Betriebssystem=====================

while(1){
}	// Ende der Endlosschleife
}	// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0(void)
{
	secs++;
	if (5 == secs) {
		// starte conversion
		ADC12CTL0 |= ADC12SC;
	}
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12(void)
{
	char buffer[64];
	int tasten_status = 0;
	int x, y, z;
	if (ADC12IFG & BIT2)
	{
		// Beschleunigungsdaten
		x = ADC12MEM0;
		y = ADC12MEM1;
		z = ADC12MEM2;
		
		BIT_CLR(ADC12IFG, BIT2 + BIT1 + BIT0);
		
		// tasten status
		if (P1IN & TASTE_RECHTS) {
			tasten_status += 1;
		}
		if (P1IN & TASTE_LINKS) {
			tasten_status += 2;
		}
		
		// sensor lesen
		SHT11_Read_Sensor();
		
		sprintf(buffer, "%s\t%s\t%d\t%d\t%d\t%d\r\n", temp_char, humi_char, x, y, z, tasten_status);
		
		// rote LED an
		LEDON;

		// sende string terminierung mit
		sendPacket(8, 18, buffer, strlen(buffer));
		
		secs = 0;
	}	
}

#pragma vector = PORT2_VECTOR	//ISR für den CC1100 Transceiver
__interrupt void PORT2 (void) {
	char res;						// CRC Check 
	int tasten_status = 0;
	int x, y, z;
	static int last_x = 0, last_y = 0, last_z = 0;
	char temp[16];
	char humi[16];
	char data[64];
	int i = 0;
	
	if (P2IFG & 0x01)// Check P2IFG Bit P2.0 - CC1100 Rx Packet
		{ 
		CLEAR(P2IFG, 0x01);
		if (RECEIVER == mode) {
			LED_TOGGLE(LED_GELB);
		}
		//LEDTOGGLE;
		res = receivePacket(); 	// CRC Rückgabe 
		if (res)				// wenn Paket OK ...
			{
				// kopiere daten
				for (i = 2; i < RxCC1100.length; i++)
				{
					data[i-2] = RxCC1100.data[i];
				}
				data[i-2] = 0x00;
				
				if (RECEIVER == mode) {
					// receiver
					printPacket(1); // 0 - Paket auf LCD ausgeben
									// 1 - Paket auf UART1 ausgeben
									
					//TODO wird nicht empfangen? sende bestaetigung
					sendPacket(18, 8, "ACK", 3);
					
					// parse paket
					sscanf(data, "T= %s\t%s\t%d\t%d\t%d\t%d\r\n", temp, humi, &x, &y, &z, &tasten_status);
					
					lcd_clear(WHITE);	// Grafikspeicher auf dem MSP430 löschen
					lcd_string(BLACK, 2, 2, data);	// Textausgabe
					lcd_paint();		// Grafikspeicher auf das LCD Display ausgeben
					
					// ueberpruefe tasten status
					if (2 == tasten_status) {
						LED_ON(LED_GRUEN);
					} else if (1 == tasten_status) {
						LED_OFF(LED_GRUEN);
					}
					
					//TODO fabs ? wtf?! beschleunigungsaenderung groeßer 20% ?
					if (((fabs(((float)x/(float)last_x) - 1)) > 0.2) ||
						((fabs(((float)y/(float)last_y) - 1)) > 0.2) || 
						((fabs(((float)z/(float)last_z) - 1)) > 0.2)) {
						LED_ON(LED_ROT);
					} else {
						LED_OFF(LED_ROT);
					}
					
					last_x = x;
					last_y = y;
					last_z = z;
					
				} else {
					// sender
					if (3 == RxCC1100.length) {
						if (!memcmp(data, "ACK", 3)) {
							// rote LED an
							LEDOFF;
						}
					}
				}
			}
		else
			{	
				spiStrobe(CC1100_SIDLE); 	// Switch to IDLE
				spiStrobe(CC1100_SFRX);	 	// Flush the RX FIFO
			}
		}	
	else
		{
		CLEAR(P2IFG, 0xFF);			 	// Clear all flags
		}
	spiStrobe(CC1100_SRX);			 	// Switch to RX Mode
}
