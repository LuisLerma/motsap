//Prototipo bytes0//MOTSAP-BETA
//Ingeniero Luis Eduardo Lerma J.
//Agosto 2019

#include <16F628A.h>
#fuses XT, BROWNOUT, NOLVP, NOWDT, PUT, NOMCLR
#use delay(internal = 4MHz)
#use rs232(baud = 9600, parity = N, XMIT = PIN_B2, RCV = PIN_B1, bits = 8)
fast_io(A);

//Variables - Sistema de seguridad
char clave[5];
char edu[5];
char control;



/*TENER EN CUENTA

BLUETOOTH   RB3
ALARMA      RA1
START       RA7
ON/OFF      RA0

*/

void tiempoala()
{
   gets(clave);
   strcpy(edu, clave);
   
   if(edu[0] == 't' ){
      output_high(PIN_A1);
      output_low(PIN_A7);
   }
      if(edu[0] == 'q' ){
      output_low(PIN_A1);
      output_high(PIN_A7);
   }
   
}



#INT_RDA // Interrupcion por serial. para recibir datos del movil
void serial_isr()
{  
      control = getc();
      if (control == 'r'){tiempoala();}
      printf("%s", edu);
   
}


void main()
{

   set_timer1(3036);
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
   ENABLE_INTERRUPTS(GLOBAL);  // Se habilitan las interrupciones globales
   ENABLE_INTERRUPTS(INT_RDA); // Se habilitan la interrupcion por evento serial.

   set_tris_A(0x00); //Puerto D como salidas digitales.

   output_low(PIN_A0);
   output_low(PIN_A1);
   output_high(PIN_B3);
   output_low(PIN_A7);
   delay_ms(300);

   delay_ms(300);
   while (true)
   {
   
   }
}
