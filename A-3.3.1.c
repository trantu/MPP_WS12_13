int main()
{
#define MCU_RESET (WDTCTL = 0)
	BIT_SET(P1SEL, TASTE_LINKS | TASTE_RECHTS);
	LED_OFF(LED_ALL);
	while (1) {
		if (P1IN & (TASTE_LINKS | TASTE_RECHTS)) {
			LED_ON(LED_ALL);
			wait(30000);
			MCU_RESET;
		}
	}
}
