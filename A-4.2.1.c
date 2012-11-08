BIT_CLR(P1IES, TASTE_LINKS); // LH
BIT_SET(P1IE, TASTE_LINKS);
BIT_CLR(P1IFG, TASTE_LINKS);

LED_OFF(LED_ALL);

BIT_SET(BCSCTL1, DIVA1);
WDTCTL = WDTPW + WDTCNTCL + WDTSSEL + WDTTMSEL;
BIT_SET(IE1, WDTIE);

LED_ON(LED_GELB);

_bis_SR_register(GIE);

//===Hier die Endlosschleife quasi das Betriebssystem=====================
	while(1){
		LED_TOGGLE(LED_GRUEN);
		wait(50000);
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	if (P1IFG & TASTE_LINKS) {
			BIT_CLR(P1IFG, TASTE_LINKS);
			WDTCTL = WDTPW + WDTCNTCL + WDTSSEL + WDTTMSEL;
			LED_OFF(LED_GELB);	
	}
}

#pragma vector = WDT_VECTOR
__interrupt void WDT(void)
{
	BIT_CLR(IFG1, WDTIFG);

	if (P4OUT & LED_GELB) {
		LED_ON(LED_GELB);
	} else {
		WDTCTL = WDTPW + WDTHOLD;
		while (true) {
			LED_OFF(LED_GELB | LED_GRUEN | LED_ROT);
			LED_ON(LED_ROT);
			SLEEP(1);
			LED_ON(LED_GELB);
			SLEEP(1);
			LED_OFF(LED_GELB | LED_ROT);
			LED_ON(LED_GRUEN);
			SLEEP(1);
			LED_OFF(LED_GRUEN);
			LED_ON(LED_GELB);
			SLEEP(1);
		}
	}
}
