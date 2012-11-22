while(1){
		int  pressed = P1IN & (TASTE_LINKS | TASTE_RECHTS);
		if (pressed) {
			fast = !fast;
			while (P1IN & (TASTE_LINKS | TASTE_RECHTS)) {
			}	
		}

		if (fast) {
			DCO();
			BIT_CLR(BCSCTL2, DIVM0 | DIVM1);			
		} else {
 			//LFXT1CLK
			BIT_SET(BCSCTL2, SELM0 | SELM1);
			BIT_SET(BCSCTL2, DIVM0 | DIVM1);
		}
	}	// Ende der Endlosschleife
