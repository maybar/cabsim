//#define CLKSPEED 20000000
#include <C:\Aplicaciones\Ingeniería\PICC\Devices\16F886.h>
#device *=16
#device ICD=TRUE
#device adc=8

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES XT                       //Crystal osc <= 4mhz
#FUSES NOPUT                    //No Power Up Timer
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOCPD                    //No EE protection
#FUSES NOWRT                    //Program memory not write protected
#FUSES NODEBUG                  //No Debug mode for ICD

#use delay(clock=4000000,RESTART_WDT)


void WriteFrecToBuffer(int16 Frec1, int16 Frec2, int16 Frec3, int16 Frec4);
signed char Check_Encoder(void);
