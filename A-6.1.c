TBR = 0;

// waehle ACLK
BIT_SET(TBCTL, TBSSEL0);
BIT_CLR(TBCTL, TBSSEL1);

// wahle count up mode
BIT_SET(TBCTL, MC0);
BIT_CLR(TBCTL, MC1);

// setze taktanzahl fuer eine sekunde
TBCCR0 = 32769;

// loesche interrupt flag
BIT_CLR(TBCCTL0, CCIFG);

// interrupt freigeben
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
	
	BIT_CLR(TBCCTL0, CCIFG);
}