// P1.0 Input
BIT_CLR(P1DIR, TASTE_LINKS);

BIT_SET(P1IES, TASTE_LINKS); // HL an linker taste loest interrupt aus
BIT_SET(P1IE, TASTE_LINKS);  // interrupts fuer linke taste erlauben
BIT_CLR(P1IFG, TASTE_LINKS); // interrupt flag loeschen

LED_OFF(LED_ALL);

// takt ausgeben, zum debuggen
BIT_SET(P5SEL, 1 << 4);
BIT_CLR(P5DIR, 1 << 4);

_bis_SR_register(GIE);

//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
		
		// LPM mode
		if (0 == status) {
			LED_OFF(LED_ALL);
			LPM4;
		}
		
		// aktiver mode	
		if (1 == status) {
			
			// ampelsequenz
			LED_ON(LED_ROT);
			SLEEP(2);
			LED_ON(LED_GELB);
			SLEEP(1);
			LED_OFF(LED_ROT);
			LED_OFF(LED_GELB);
			LED_ON(LED_GRUEN);
			SLEEP(2);
			LED_OFF(LED_GRUEN);
			LED_ON(LED_GELB);
			SLEEP(1);
			LED_OFF(LED_GELB);
		}
		
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	if (P1IFG & TASTE_LINKS) {

		if (P1IES & TASTE_LINKS) {
			// high - low flanke aktiviert		
			status = 1;	// aktiver mode
			LPM4_EXIT;
			// setze interrupt fuer low - high
			BIT_CLR(P1IES, TASTE_LINKS);
		} else {
			// low - high flanke aktiviert
			status = 0;	// low power mode
			// setze interrupt fuer high - low
			BIT_SET(P1IES, TASTE_LINKS);
		}
		
		BIT_CLR(P1IFG, TASTE_LINKS);
	}
}
