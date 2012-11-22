TBR = 0;

// waehle ACLK
BIT_SET(TBCTL, TBSSEL0);
BIT_CLR(TBCTL, TBSSEL1);

// wahle count up mode
BIT_SET(TBCTL, MC0);
BIT_CLR(TBCTL, MC1);

// setze taktanzahl fuer eine sekunde
TBCCR0 = 32769;

// loesche interrupt flag fuer timer
BIT_CLR(TBCCTL0, CCIFG);

BIT_CLR(P1IES, TASTE_LINKS | TASTE_RECHTS); // LH an linker taste loest interrupt aus
BIT_CLR(P1IFG, TASTE_LINKS | TASTE_RECHTS); // interrupt flag loeschen
BIT_SET(P1IE, TASTE_LINKS | TASTE_RECHTS); // interrupts fuer linke taste erlauben

// interrupt fuer timer freigeben
BIT_SET(TBCCTL0, CCIE);

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
	LED_TOGGLE(LED_ROT);
	
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	unsigned long new_value = 0;
	
	if (P1IFG & TASTE_LINKS) {
		BIT_CLR(P1IFG, TASTE_LINKS);
		new_value = (long)TBCCR0 * (long)2;
		if (new_value > 0xffff) {
			new_value = 0xffff;	
		}
	}
	
	if (P1IFG & TASTE_RECHTS) {
		BIT_CLR(P1IFG, TASTE_RECHTS);
		new_value = TBCCR0 / 2;
		if (0 == new_value) {
			new_value = 1;
		}
	}
	TBR = 0;
	TBCCR0 = new_value;
}
