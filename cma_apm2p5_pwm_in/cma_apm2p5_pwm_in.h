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

	rev02
	27 Feb 2015
	
	cma_apm2p5_pwm_in.h 
	PPM decoder uses ICP5. (assume max channel = 8)
		
	Usage: 
	setup > PPM available
	
	Created by Carlos Ma, 13 Nov 2014.
*/

#ifndef cma_apm2p5_pwm_in_h
#define cma_apm2p5_pwm_in_h
#define MIN_PULSE_WIDTH 900
#define MAX_PULSE_WIDTH 2100
#define NUM_PWM_IN_CH 8 // number of pwm channels in ppm stream

#include "Arduino.h"

class apm2p5_pwm_in
{
	public:
		apm2p5_pwm_in();
		void setup(void);
		uint16_t read_in_ch(uint8_t ch);
		static void _timer5_ppm_interrupt(void);
		boolean ppm_in_status(void);
		
	private:
		static volatile uint16_t _pwm_in[NUM_PWM_IN_CH];
		static volatile boolean _ppm_in_status;
};

#endif