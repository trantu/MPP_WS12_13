static unsigned long t1 = 60*60*8 + 60*30 + 22;
static unsigned long t2 = 60*60*9 + 60*1 + 01;
static unsigned char hours = 0;
static unsigned char mins = 0;
static unsigned char secs = 0;

.
.
.

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
TBCCR0 = 4096; // eine sekunde: takt=32000 / divider=8

// loesche interrupt flag fuer timer
BIT_CLR(TBCCTL0, CCIFG);

BIT_CLR(P1IES, TASTE_LINKS | TASTE_RECHTS); // LH an linker taste loest interrupt aus
BIT_CLR(P1IFG, TASTE_LINKS | TASTE_RECHTS); // interrupt flag loeschen
BIT_SET(P1IE, TASTE_LINKS | TASTE_RECHTS); // interrupts fuer linke taste erlauben

LED_OFF(LED_ALL);

BIT_SET(TBCCTL0, CCIE);

_bis_SR_register(GIE);

//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
		
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0(void)
{
	char buffer[16];
	secs++;
	if (secs > 59) {
		mins++;
		secs %= 60;
		if (mins > 59) {
			mins %= 60;
			hours++;
			if (hours > 23) {
				hours %= 24;	
			}
		}
	}
	
	if (t1 == 60*60*(long)hours + 60*(long)mins + (long)secs) {
		LED_ON(LED_GELB);		
	}
	
	if (t2 == 60*60*(long)hours + 60*(long)mins + (long)secs) {
		LED_OFF(LED_GELB);
	}
	
	sprintf(buffer, "%02d:%02d:%02d", hours, mins, secs);
	lcd_clear(WHITE);
	lcd_string(BLACK, 15, 25, buffer);	// Textausgabe
	lcd_paint();
	LED_TOGGLE(LED_ROT);
}
