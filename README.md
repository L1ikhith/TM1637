# TM1637-Tiva-C
This library is to communicate between Tiva C series(TM4C123GH6PM) board and TM1637 4 digit display  using Keil uprocessor 

# Connections 
The TM1637 has 4 pins (VCC, GND, CLK, DIO)  
   TM1637 | TM4C123GH6PM
--------- | -------------
VCC       | VBus
GND       | GND
CLK       | PA3
DIO       | PA2

The code is created based on above pins if you want to use different pins in port A goto Markup: [PinConfig](#PinConfig "Goto PinConfig")

# Install/Usage
Create a new project in uKeil and add header and cpp files in Source Group which is under Target folder

# Functions
* TM1637Init  - to initialize the port and pins
* setBrightness - to change brightness
* clear - to clear the display
* showNumberDec - display decimal number 
* _delay_ms - to create delay (millisecs)

# PinConfig
To use different pins in Port A please change the hex values of DEN and DIR to the respective values, these are located in TM1637.cpp --> TM1637Init method  
GpioA->DATA (0x04) to specific pin hex value  ---> Data (DIO)
GpioA->DATA (0x08) to specific pin hex value  ---> Clock (CLK)
_________________________________________
Hex values for different pins
  Pin     | Hex
--------- | -------------
0         | 0x01
1         | 0x02
2         | 0x04
3         | 0x08
4         | 0x10
5         | 0x20
6         | 0x40
7         | 0x80

# Reference

Some of the code was inspired by avishorp **TM1637 for arduino** code
