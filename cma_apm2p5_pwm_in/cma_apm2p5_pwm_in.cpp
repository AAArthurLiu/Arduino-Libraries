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
#include <cma_apm2p5_pwm_in.h>
#include "avr/interrupt.h"

volatile uint16_t apm2p5_pwm_in::_pwm_in[NUM_PWM_IN_CH]; // will be accessed by an interrupt function, hence volatile
volatile boolean apm2p5_pwm_in::_ppm_in_status; // accessed by interrupt

apm2p5_pwm_in::apm2p5_pwm_in()
{
	for (int i = 0; i < NUM_PWM_IN_CH; i++)
	{
		_pwm_in[i] = 0;
	}	
	
	uint8_t _radio_status = 0;
}

ISR(TIMER5_CAPT_vect)
{
  apm2p5_pwm_in::_timer5_ppm_interrupt();
}

void apm2p5_pwm_in::setup(void)
{
	// initialization sequence
	pinMode(48, INPUT); // PPM Input (ICP5)
	
	// disable all interrupts during initialization sequence
	cli();
	
	// timer/counter settings
	TCCR5A = 0;
    TCCR5B = 0;
    TCCR5C = 0;
	
	// FAST PWM for clock 5; TOP value defined by OCRnA
	TCCR5A |= ((1 << WGM51) | (1 << WGM50));
	TCCR5B |= ((1 << WGM53) | (1 << WGM52));
		
	TCCR5B |= 2;
	
	// Input capture edge select
	TCCR5B |= (1 << ICES5); // capture at rising edge
	TCNT5 = 0; 
	
	// OCR5A for timer5 PWM freq
	OCR5A = 16000000 / (8 * 50) - 1; // 50 loop Hz for PPM read (so that the buffer is big enough)
	
	// attach interrupt
	//isr_reg->register_signal( ISR_REGISTRY_TIMER5_CAPT, _timer5_capt_cb );
	
	// enable ppm interrupt
	TIMSK5 |= (1 << ICIE5);
	// enable interrupt
	sei(); 
}

uint16_t apm2p5_pwm_in::read_in_ch(uint8_t ch)
{
	uint16_t pulse_width;
	cli();
    pulse_width = _pwm_in[ch];
    sei();
	pulse_width >>= 1; // divide by 2
	pulse_width = constrain(pulse_width, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
	_ppm_in_status = 0;
	
	return pulse_width;
}

void apm2p5_pwm_in::_timer5_ppm_interrupt(void)
{
	uint16_t icr_new; // temporary
	uint16_t pulse_width;
	static uint16_t icr_old; // must store for the next interrupt, hence static
	static uint8_t ch;
	
	icr_new = ICR5;
	
	// calculate pulse width
    if ( icr_new < icr_old ) // if new ICR is smaller than the old record, add OCR5A size
	{
		pulse_width = ( icr_new + 16000000 / (8 * 50) - 1 ) - icr_old;
	} 
	else 
	{
		pulse_width = icr_new - icr_old;
	}
	
	// syncing
	if ( pulse_width > 8000 ) // assuming sync width = 4ms
	{ch = 0;} 
	else 
	{
        // Writing pulse
        if ( ch < NUM_PWM_IN_CH ) 
		{
			_pwm_in[ch++] = pulse_width;
			// raise radio OK flag after receiving all 8 pulses
            if (ch >= NUM_PWM_IN_CH) 
			{
                _ppm_in_status = 1;
            }
        }
    }
	icr_old = icr_new; // marking old icr
}

boolean apm2p5_pwm_in::ppm_in_status(void)
{
	return _ppm_in_status;
}