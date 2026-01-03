#ifndef TIM_HPP
#define TIM_HPP

#include <types.h>

uint8_t timer_8bit_OCR_cal(uint16_t frequency, uint16_t prescaler);
void startTimer();
void stopTimer();
uint8_t setTimer();
void resetTimer();

#endif // MY_FILENAME_HPP