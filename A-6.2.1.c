static unsigned int cycles = 0;

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
TBCCR0 = 0xffff;

// loesche interrupt flag fuer timer
BIT_CLR(TBCCTL0, CCIFG);

BIT_CLR(P1IES, TASTE_LINKS | TASTE_RECHTS); // LH an linker taste loest interrupt aus
BIT_CLR(P1IFG, TASTE_LINKS | TASTE_RECHTS); // interrupt flag loeschen
BIT_SET(P1IE, TASTE_LINKS | TASTE_RECHTS); // interrupts fuer linke taste erlauben

LED_OFF(LED_ALL);

_bis_SR_register(GIE);

//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
		
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0(void)
{
	cycles++;
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	unsigned int tbr = TBR;
	unsigned int secs = 0;
	unsigned int centi_secs = 0;
	char buffer[16]; 
	
	if (P1IFG & TASTE_LINKS) {
		// starte zeitmessung
		BIT_CLR(P1IFG, TASTE_LINKS);
		
		cycles = 0;
		TBR = 1;	// interrupt wird ausgeloest bei 0
		
		// interrupt fuer timer freigeben
		BIT_SET(TBCCTL0, CCIE);
	}
	
	if (P1IFG & TASTE_RECHTS) {
		// stoppe zeitmessung
		BIT_CLR(P1IFG, TASTE_RECHTS);
		
		// interrupt fuer timer stoppen
		BIT_CLR(TBCCTL0, CCIE);
		
		secs = cycles * 16 + tbr / 4096;
		centi_secs = (tbr % 4096) % 10;
		sprintf(buffer, "Zeit: %02d,%02d", secs, centi_secs);
		lcd_clear(WHITE);
		lcd_string(BLACK, 15, 25, buffer);	// Textausgabe
		lcd_paint();
	}
}
