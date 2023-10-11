void WDT_interrupt_enable(uint8_t timeout_v)
{
    unsigned char bakSREG;
    uint8_t prescaler;

    prescaler = timeout_v & 0x07;
    prescaler |= (1 << WDIE);
    if (timeout_v > 7)
        prescaler |= (1 << WDP3);

    bakSREG = SREG;
    cli();
    wdt_reset();
    WDTCSR |= ((1 << WDCE) | (1 << WDE));
    WDTCSR = prescaler;
    SREG = bakSREG;
}

void WDT__enable(uint8_t timeout_v)
{
    unsigned char bakSREG;
    uint8_t prescaler;

    prescaler = timeout_v & 0x07;
    prescaler |= (1 << WDE);
    if (timeout_v > 7)
        prescaler |= (1 << WDP3);

    bakSREG = SREG;
    cli();
    wdt_reset();
    WDTCSR |= ((1 << WDCE) | (1 << WDE));
    WDTCSR = prescaler;

    SREG = bakSREG;
}

void WDT__disable(void)
{
    uint8_t bakSREG;
    bakSREG = SREG;
    cli();
    wdt_reset();
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= ((1 << WDCE) | (1 << WDE));
    WDTCSR = (0 << WDE) | (0 << WDIE);
    SREG = bakSREG;
}

