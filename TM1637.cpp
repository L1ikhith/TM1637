extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include "TM1637.h"
#include <TM4C123.h>                   


#define COMMAND1    0x40
#define COMMAND2    0xC0
#define COMMAND3    0x80
static const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
  };

static uint8_t bright;
void Display::TM1637Init(uint16_t bitDelay)
{
	Sysctl->RCGCGPIO= 0x01;
  GpioA->LOCK = 0x4C4F434B;
	GpioA->CR = 0x1F;
	GpioA->DEN = 0x0E;
	GpioA->DIR = 0x0E;
/* Configure 4Mhz(in this using CLK_SRC bit as 0) Systick Timer and Enabiling Interrupt for Systick */
	SysTick -> CTRL = 0x03;
/* Load value for 1 second delay generation (LOAD= Time Delay(uS)/((1/Clock freq(Hz))*1000000uS)*/
/* Time for one tick of systick Timer = 0.25uS*/
	SysTick->LOAD = 4000-1 ;  /* 1 milliS */
	
 GpioA->DATA|= 0x0C; // pulls both Data and Clock pins high 
 _delay_ms(bitDelay);
}

void Display::start()
{
	GpioA->DATA&=~(0x04u); // Data pin is low
	
}
void Display::stop()
{
	GpioA->DATA&=~(0x04u);                     // Data pin is low
	GpioA->DATA|=0x08;                        //  Clk pin is high
	GpioA->DATA|=(0x04u);                    //   Data pin is high
}
	
	void Display::SendData( uint8_t segments[], uint8_t length, uint8_t pos)
	{
		start();                                
		WriteReg(COMMAND1);                     
		stop();                                 
		//*******************************//
		start();
	  WriteReg(COMMAND2 + (pos & 0x03));
	  for (uint8_t k=0; k < length; k++){
	   WriteReg(segments[k]);
		}
		stop();
		//*****************************//
		start();
		WriteReg(COMMAND3+(bright & 0x0f));
		stop();
	  //*****************************//
	}

	void Display::setBrightness(uint8_t brightness, bool on)
	{
		  bright = ((brightness & 0x7) | (on? 0x08 : 0x00));
	}
	
bool Display::WriteReg(uint8_t b)
{
	unsigned int ack;
	uint8_t data =b;

for(uint8_t i = 0; i < 8; i++) {
		GpioA->DATA&=~(0x08u);                           //  Clk pin is low
		_delay_ms(DEFAULT_DELAY);
	
	// Sending data to TM1637 through Data Pin
    if (data & 0x01)
     GpioA->DATA|=(0x04u); 
    else
     GpioA->DATA&=~(0x04u);
		
   _delay_ms(DEFAULT_DELAY);
	  GpioA->DATA|=0x08;                          //  Clk pin is high
	  data >>= 1;
	}
	 // This is to check Ack
	//************************falling edge of 8th cycle ***************//
		GpioA->DATA&=~(0x08u);                     //  Clk pin is low
    GpioA->DATA|=(0x04u);
	  _delay_ms(DEFAULT_DELAY);
 //************************raising edge of 9th cycle ***************//
		GpioA->DATA|=0x08;                        //  Clk pin is high
		_delay_ms(DEFAULT_DELAY);
		
		if (GpioA->DATA==0x08){
		 ack =0;
		}
		else {
			ack=1;
 //************************falling edge of 9th cycle ***************//
		GpioA->DATA&=~(0x08u);                    //  Clk pin is low
		}	
	return ack; 
}

void Display::clear()
{
	uint8_t data[] = { 0, 0, 0, 0 };
	SendData(data);
}

void Display::showNumberDec(int num, bool leading_zero, uint8_t length, uint8_t pos)
{
  showNumberDecEx(num, 0, leading_zero, length, pos);
}

void Display::showNumberDecEx(int num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
	uint16_t numb = static_cast<uint16_t>(num);
  showNumberBaseEx(num < 0? -10 : 10, num < 0? -numb : numb, dots, leading_zero, length, pos);
}

void Display::showNumberHexEx( uint16_t num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
  showNumberBaseEx(16, num, dots, leading_zero, length, pos);
}

void Display::showNumberBaseEx(int8_t base, uint16_t num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
    bool negative = false;
	if (base < 0) {
	    base = -base;
		negative = true;
	}

    uint8_t digits[4];

	if (num == 0 && !leading_zero) {
		
		for(uint8_t i = 0; i < (length-1); i++)
			digits[i] = 0;
		digits[length-1] = encodeDigit(0);
	}
	else {
		

		for(int i = length-1; i >= 0; --i)
		{
		    uint8_t digit = num % base;
			
			if (digit == 0 && num == 0 && leading_zero == false)
			   
				digits[i] = 0;
			else
			    digits[i] = encodeDigit(digit);
				
			if (digit == 0 && num == 0 && negative) {
			    digits[i] = minusSegments;
				negative = false;
			}

			num /= base;
		}
    }
	
	if(dots != 0)
	{
		showDots(dots, digits);
	}
    
    SendData(digits, length, pos);
}
uint8_t Display::encodeDigit(uint8_t digit)
{
	return digitToSegment[digit & 0x0f];
}
	void Display::showDots(uint8_t dots, uint8_t* digits)
{
    for(int i = 0; i < 4; ++i)
    {
        digits[i] |= (dots & 0x80);
        dots <<= 1;
    }
}
// look indepth for Dealy use sysclk for creating a Delay 
void Display:: _delay_ms(uint16_t delay)
{
	ms=0;
	while(delay>ms);
	}
void SysTick_Handler(void)
{
 ms++;
}

