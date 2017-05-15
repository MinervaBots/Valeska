#include <24F16KM202.h>
#device ICSP=1
#use delay(crystal=20000000)

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES NOWINDIS                 //Watch Dog Timer in Window mode
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOPUT                    //No Power Up Timer
#FUSES BORV_LOW                 //Brown-out Reset set to lowest voltage
#FUSES NOMCLR                   //Master Clear pin used for I/O

#use FIXED_IO( B_outputs=PIN_B11,PIN_B10 )

#define PIN_BORDA_ESQUERDA   PIN_A6
#define PIN_MOTOR_ESQUERDO   PIN_B10
#define PIN_MOTOR_DIREITO   PIN_B11
#define PIN_BORDA_DIREITA   PIN_B14


