BIT_CLR(P1IES, TASTE_LINKS); // LH
BIT_SET(P1IES, TASTE_RECHTS); // HL
BIT_SET(P1IE, TASTE_LINKS | TASTE_RECHTS);
BIT_CLR(P1IFG, TASTE_LINKS | TASTE_RECHTS);

_bis_SR_register(GIE);

LED_OFF(LED_ALL);
//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	if (P1IFG & TASTE_LINKS) {
		BIT_CLR(P1IFG, TASTE_LINKS);
		LED_TOGGLE(LED_GRUEN);
	}

	if (P1IFG & TASTE_RECHTS) {
		BIT_CLR(P1IFG, TASTE_RECHTS);
		LED_TOGGLE(LED_ROT);
	}
}
