/*
 * blink.c
 *
 * Created: 2015-02-22 16:54:11
 *  Author: petter
 */ 


#include <avr/io.h>
#define F_CPU 1000000

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

//declarations
void blink();
void enable_WDT();
void disable_WDT();

#define red PB0
#define green PB1
#define blue PB2

uint8_t blink_counter = 0;
const uint8_t no_blink_states = 7;
const uint8_t blink_states[] = {
				(1 << red),
				(1 << green),
				(1 << blue),
				(1 << red) | (1 << green),
				(1 << red) | (1 << blue),
				(1 << green) | (1 << blue),
				(1 << red) | (1 << green) | (1 << blue),
				};

//variables for wd period
const int	WD_periods = 60;
volatile int WD_counter = 0;



//Isr for wdt timeout, enable timer
ISR(SIG_WATCHDOG_TIMEOUT)	
{
	blink();
	WD_counter++;
	
	
	if(WD_counter > WD_periods)	//Disable WD
	{
		disable_WDT();
		
	}else{
		WDTCR |= 1 << WDIE;		//Set interrupt enable again
	}
}

ISR(SIG_PIN_CHANGE)
{
	//blink();	
	enable_WDT();	//Start period of wdt interrupts
}

//Set up wdt for blink period
void enable_WDT()
{
	//Reset counter for wd
	WD_counter = 0;
	//Setup and start wd
	
	WDTCR |= /*(1 << WDP1) |*/ (1 << WDP2) | (1 << WDP0);	//Set to 1/2 sec timeout
	WDTCR |= (1 << WDIE) | (1 << WDE);	//start wd
	//wdt_enable(WDTO_1S);
	
	
}
void disable_WDT()
{
	MCUSR &= !(1 << WDRF);
	//wdt_disable();
	WDTCR |= (1 << WDCE) | (1 << WDE);	//Prepare for stop
	WDTCR &= !((1 << WDIE) | (1 << WDE));	//stop
}

//Blink leds, set registers and so on
void blink()
{
	
	if(blink_counter >= no_blink_states)
	{
		blink_counter = 0;
	}
	uint8_t curr_blink_state = blink_states[blink_counter];
	//curr_blink_state = 1 << red;
	
	DDRB |= curr_blink_state;	//Enable output
	PORTB |= curr_blink_state;	//power on
	_delay_ms(5);
	
	PORTB &= !(curr_blink_state);	//power off
	DDRB = 0;	//Disable
	blink_counter++;
}

int main(void)
{
	
	// setup
	ADCSRA = 0;	//Disable adc
	PRR = 0xf;	//Enable all power reduction modes
	
	
	PCMSK |= 1 << PCINT3;	//Enable interrupt on pin 2, pcint 3
	GIMSK |= 1 << PCIE;		//Enable pin change interrupt
	//Blink once for startup
	disable_WDT();
	blink();
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
		//blink();
		
    }
}