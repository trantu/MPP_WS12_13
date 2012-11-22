#define BIT_SET(a,b) ((a) |= (b))
#define BIT_CLR(a,b) ((a) &= ~(b))
#define BIT_TOGGLE(a,b) ((a) ^= (b))

#define LED_ROT (0x01)   // 0 0 1
#define LED_GELB (0x02)  // 0 1 0
#define LED_GRUEN (0x04) // 1 0 0
#define LED_ALL (LED_ROT | LED_GELB | LED_GRUEN)

#define LED_ON(led) (BIT_CLR(P4OUT, led)) 
#define LED_OFF(led) (BIT_SET(P4OUT, led)) 
#define LED_TOGGLE(led) (BIT_TOGGLE(P4OUT, led))

#define TASTE_LINKS (0x1)
#define TASTE_RECHTS (0x2)

#define SLEEP_QUANTUM 10000
#define SLEEP(n) do { \
	long time = n * 100000; /* wait() sleeps 10*n microseconds */ \
	while(time > SLEEP_QUANTUM) { \
		wait(SLEEP_QUANTUM); \
		time -= SLEEP_QUANTUM; \
	} \
	wait(time); \
} while(0)

print_value();

// P5.4 Input
BIT_SET(P5SEL, 1 << 4);

//===Hier die Endlosschleife quasi das Betriebssystem=====================
	while(1){
		LED_OFF(LED_ALL);
		SLEEP(1);

		LED_ON(LED_ROT);
		SLEEP(1);
		LED_OFF(LED_ROT);

		LED_ON(LED_GELB);
		SLEEP(1);
		LED_OFF(LED_GELB);

		LED_ON(LED_GRUEN);
		SLEEP(1);
		LED_OFF(LED_GRUEN);

		LED_ON(LED_ALL);
		SLEEP(1);
	}
