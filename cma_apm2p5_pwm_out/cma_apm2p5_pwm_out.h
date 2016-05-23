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


	rev01
	2 Feb 2015
	
	cma_apm2p5_pwm_io.h - for direct adaptation in an APM2.5.
	Phase, frequency-correct (OR FAST PWM) hardware PWM out. 
	
	Ch. 1-8, use timer OC1A/B, OC3A/B/C, OC4A/B/C. PFC PWM.
	Ch. 9, 10, use timer OC5B/C (DISABLED)
	
	PPM decoder uses ICP5. (assume max channel = 8)
	
	Min-Max PWM out frequency: 20-450 Hz.
	
	Usage: 
	setup > PPM available
	setup > enable > PWM out available
	
	Created by Carlos Ma, 13 Nov 2014.
*/



#ifndef cma_apm2p5_pwm_out_h
#define cma_apm2p5_pwm_out_h

#define MIN_PWM_FREQ 20 // Hz
#define MAX_PWM_FREQ 450
#define MIN_PULSE_WIDTH 900 // us
#define MAX_PULSE_WIDTH 2100

#include "Arduino.h"

class apm2p5_pwm_out
{
	public:
		apm2p5_pwm_out();
		void setup(uint16_t pwm_freq); // does not enable pwm out
		void enable_ch(uint8_t ch);
		void disable_ch(uint8_t ch);
		void write_ch(uint8_t ch, uint16_t pulse_width);
		uint16_t read_out_ch(uint8_t ch);
	private:
};

#endif