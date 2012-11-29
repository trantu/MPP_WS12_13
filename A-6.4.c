#define IS_LED_ON(led) (!(P4OUT & led))
#define TIMER_INTERRUPT_MS 3000
#define LED_ON_MS 1000

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

// setze anzahl fuer interrupt
TBCCR0 = (4 * TIMER_INTERRUPT_MS); // eine sekunde: takt=32000 / divider=8

// loesche interrupt flag fuer timer
BIT_CLR(TBCCTL0, CCIFG);

LED_OFF(LED_ALL);

BIT_SET(TBCCTL0, CCIE);

_bis_SR_register(GIE);

//===Hier die Endlosschleife quasi das Betriebssystem=====================
	while(1){
		LPM3;
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0(void)
{
	if (IS_LED_ON(LED_ROT)) {
		TBCCR0 = (4 * TIMER_INTERRUPT_MS);
		LED_OFF(LED_ROT);
	} else {
		TBCCR0 = (4 * LED_ON_MS);
		LED_ON(LED_ROT);
	}
}