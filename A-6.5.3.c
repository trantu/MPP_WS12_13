BIT_SET(P4SEL, BIT1);

// waehle ACLK
BIT_SET(TBCTL, TBSSEL0);
BIT_CLR(TBCTL, TBSSEL1);

// setze divider auf 8
//BIT_SET(TBCTL, ID0 | ID1);

// setze out-mode 011, set/reset mode
BIT_SET(TBCCTL1, OUTMOD0);
BIT_SET(TBCCTL1, OUTMOD1);
BIT_CLR(TBCCTL1, OUTMOD2);

// wahle count up mode
BIT_SET(TBCTL, MC0);
BIT_CLR(TBCTL, MC1);

TBCCR0 = 512;
TBCCR1 = 20;

LPM3;
//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
	}	// Ende der Endlosschleife
  