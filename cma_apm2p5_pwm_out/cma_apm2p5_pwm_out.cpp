/*
Copyright(c) 2016, Carlos Ma
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met :
*Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and / or other materials provided with the distribution.
* Neither the name of the <organization> nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.IN NO EVENT SHALL <Carlos Ma> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Arduino.h"
#include <cma_apm2p5_pwm_out.h>

apm2p5_pwm_out::apm2p5_pwm_out()
{
}

void apm2p5_pwm_out::setup(uint16_t pwm_freq)
{
	// initialization sequence
	pwm_freq = constrain(pwm_freq, MIN_PWM_FREQ, MAX_PWM_FREQ); // Min/Max PWM out frequency
		
	// enabling pwm out pins as output pins
	// channel, output compare register
	// pulling high to avoid momentary drop of the pin to low in case ESC has a pull-down resistor (REF: APM_RC_APM2.cpp)
	digitalWrite(12,HIGH); 
    digitalWrite(11,HIGH);
	digitalWrite(8,HIGH); 
    digitalWrite(7,HIGH);
	digitalWrite(6,HIGH); 
    digitalWrite(3,HIGH);
	digitalWrite(2,HIGH); 
    digitalWrite(5,HIGH);

	pinMode(12, OUTPUT); // ch1, OC1B
	pinMode(11, OUTPUT); // ch2, OC1A
	pinMode(8, OUTPUT); // ch3, OC4C
	pinMode(7, OUTPUT); // ch4, OC4B
	pinMode(6, OUTPUT); // ch5, OC4A
	pinMode(3, OUTPUT); // ch6, OC3C
	pinMode(2, OUTPUT); // ch7, OC3B
	pinMode(5, OUTPUT); // ch8, OC3A

	digitalWrite(12,HIGH); // pull high again to avoid memory drop, not sure if necessary
    digitalWrite(11,HIGH);
	digitalWrite(8,HIGH); 
    digitalWrite(7,HIGH);
	digitalWrite(6,HIGH); 
    digitalWrite(3,HIGH);
	digitalWrite(2,HIGH); 
    digitalWrite(5,HIGH);

		
	// disable all interrupts during initialization sequence
	cli();
	
	// timer/counter settings
	TCCR1A = 0; // clearing, not necessary as by default they are initialized to 0
    TCCR1B = 0; // left here just in case some how they are not
    TCCR1C = 0;
	TCCR3A = 0;
    TCCR3B = 0;
    TCCR3C = 0;
	TCCR4A = 0;
    TCCR4B = 0;
    TCCR4C = 0;
	
	// Phase, freq correct PWM; TOP value defined by ICRn
	TCCR1B |= (1 << WGM13); 
	TCCR3B |= (1 << WGM33);
	TCCR4B |= (1 << WGM43);
		
	TCCR1B |= 2; // applying prescaler
	TCCR3B |= 2; // CSn B010 for prescaler = 8
	TCCR4B |= 2;
	
	TCNT1 = 0; // initialize timer count to 0 
	TCNT3 = 0; 
	TCNT4 = 0;
	
	// PWM settings
	// using input capture register to define TOP value of counter
	// setting TOP value to define PWM out freq (phase freq correct)
	ICR1 = 16000000 / (2 * 8 * pwm_freq); // closest freq
	ICR3 = ICR1; 
	ICR4 = ICR1; 
	
	// initialize output width
	OCR1B = MIN_PULSE_WIDTH; // output compare register; set output nx PWM width in (us)
	OCR1A = MIN_PULSE_WIDTH;
	OCR4C = MIN_PULSE_WIDTH;
	OCR4B = MIN_PULSE_WIDTH;
	OCR4A = MIN_PULSE_WIDTH;
	OCR3C = MIN_PULSE_WIDTH;
	OCR3B = MIN_PULSE_WIDTH;
	OCR3A = MIN_PULSE_WIDTH;
	
	sei(); 
}

void apm2p5_pwm_out::write_ch(uint8_t ch, uint16_t pulse_width)
{
	pulse_width = constrain(pulse_width, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
	
	switch (ch)
	{
		case 1:	OCR1B = pulse_width; break;// 16 * pulse_width / (2 * prescaler)
		case 2:	OCR1A = pulse_width; break;
		case 3: OCR4C = pulse_width; break;
		case 4: OCR4B = pulse_width; break;
		case 5:	OCR4A = pulse_width; break;
		case 6:	OCR3C = pulse_width; break;
		case 7:	OCR3B = pulse_width; break;
		case 8:	OCR3A = pulse_width; break;
	}
}

uint16_t apm2p5_pwm_out::read_out_ch(uint8_t ch)
{
	switch (ch)
	{
		case 1:	return OCR1B;
		case 2:	return OCR1A;
		case 3: return OCR4C;
		case 4: return OCR4B;
		case 5:	return OCR4A;
		case 6:	return OCR3C;
		case 7:	return OCR3B;
		case 8:	return OCR3A;
		default: return 0;
	}
}

void apm2p5_pwm_out::enable_ch(uint8_t ch)
{
	// enable PWM
	// Clear OCnA/OCnB/OCnC on compare match, set OCnA/OCnB/OCnC at BOTTOM 
	switch (ch)
	{
		case 1:	TCCR1A |= (1 << COM1B1); break; 
		case 2:	TCCR1A |= (1 << COM1A1); break;
		case 3: TCCR4A |= (1 << COM4C1); break;
		case 4: TCCR4A |= (1 << COM4B1); break;
		case 5:	TCCR4A |= (1 << COM4A1); break;
		case 6:	TCCR3A |= (1 << COM3C1); break;
		case 7:	TCCR3A |= (1 << COM3B1); break;
		case 8:	TCCR3A |= (1 << COM3A1); break;
	}
	
}

void apm2p5_pwm_out::disable_ch(uint8_t ch)
{
	// diable PWM
	// disconnect OCnA/OCnB/OCnC
	switch (ch)
	{
		case 1:	TCCR1A &= ~(1 << COM1B1); break; 
		case 2:	TCCR1A &= ~(1 << COM1A1); break;
		case 3: TCCR4A &= ~(1 << COM4C1); break;
		case 4: TCCR4A &= ~(1 << COM4B1); break;
		case 5:	TCCR4A &= ~(1 << COM4A1); break;
		case 6:	TCCR3A &= ~(1 << COM3C1); break;
		case 7:	TCCR3A &= ~(1 << COM3B1); break;
		case 8:	TCCR3A &= ~(1 << COM3A1); break;
	}
}
