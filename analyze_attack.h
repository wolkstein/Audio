/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef analyze_attackdetect_h_
#define analyze_attackdetect_h_

#include "Arduino.h"
#include "AudioStream.h"

class AudioAnalyzeAttack : public AudioStream
{
public:
	AudioAnalyzeAttack(void) : AudioStream(1, inputQueueArray) {
		min_sample = 32767;
		max_sample = -32768;
                old_calculated_peak = 0;
                calculated_peak = 0;
                current_peak = 0;
                falldown = 1000;
                treshold = 2000;
                bounce_time = 50;
                lastbounce = 0 ;
                callback_active = false;
                //(*func)();
                
	}
       
        void set(int32_t tres, int32_t falld, uint16_t bt, void (*f)(bool)) {
		falldown = falld;
                treshold = tres;
                bounce_time = bt;
                func = f;
        }
        
        void setCallbackActive(bool set){
            callback_active = set;
        }
        
        void setTresAndFall(int32_t tres, int32_t falld){
            treshold = tres;
            falldown = falld;
        }
        
        void setBounceTime(uint16_t time){
            bounce_time = time;
        }        
        
	bool available(void) {
		__disable_irq();
		bool flag = new_output;
		if (flag) new_output = false;
		__enable_irq();
		return flag;
	}
	float peakInfo(void) {
		return calculated_peak;
	}

	virtual void update(void);
        
private:
	audio_block_t *inputQueueArray[1];
	volatile bool new_output;
	int16_t min_sample;
	int16_t max_sample;
        void (*func)(bool);
        bool callback_active;
        
        //
        uint16_t bounce_time;
        int32_t treshold;
        int32_t falldown;
        int32_t current_peak;
        int32_t calculated_peak;
        int32_t old_calculated_peak;
        int32_t old_new_diff;
        
        uint32_t lastbounce;
        
        void calculate(void) {
                // get the peaks
                __disable_irq();
                int min = min_sample;
                int max = max_sample;
                min_sample = 32767;
                max_sample = -32768;
                __enable_irq();
                min = abs(min);
                max = abs(max);
                if (min > max) max = min;
                current_peak = max;
                // get peaks end
                
                
                // lower calculated_peak by falldown value
                calculated_peak -= falldown;
                
                
                // rise calculated peak to max peak
                if(calculated_peak < current_peak) calculated_peak = current_peak;
                
                old_new_diff = calculated_peak - old_calculated_peak;
                
                // hit calculation
                uint32_t currenttime = millis();
                bool anhit = false;
                if(old_new_diff > treshold && currenttime > lastbounce + bounce_time){ // an hit
                    anhit = true;
                    lastbounce = millis();
                }
                
                // debug out
                //if(max > 2) Serial.printf("%d | %d | %d | %d\n",currenttime,max, calculated_peak,old_new_diff);
                //if (calculated_peak > 1){
                    //if(old_new_diff > treshold && anhit) Serial.print("++++++++++++++++++++++++++----------treffer");
                    //Serial.printf("Time: %d | CP: %d | O-N-Diff: %d | AnHit?: %d\n",currenttime,calculated_peak,old_new_diff, anhit );
                //}
                // end debug out 
                
                // set old peak
                old_calculated_peak = calculated_peak;
                
                
                // fire callback Function
                if(callback_active){
                    if (anhit && *func) (*func)(true);
                }
                return;
        }

        
        
};

#endif
