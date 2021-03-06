//Prototipo bytes0//MOTSAP-BETA
//Ingeniero Luis Eduardo Lerma J.
//Agosto 2019

#include <16F628A.h>
#fuses XT, BROWNOUT, NOLVP, NOWDT, PUT, NOMCLR
#use delay(internal = 4MHz)
#use rs232(baud = 9600, parity = N, XMIT = PIN_B2, RCV = PIN_B1, bits = 8)
fast_io(A);

//Variables - Sistema de seguridad
char a, b, c, d;
char e, f, g, h;
char clave[10];
char edu[10];
char control;
int8 num_faults = 0;
int8 SecurityLVL = 0;
/*TENER EN CUENTA

BLUETOOTH   RB3
ALARMA      RA1
START       RA7
ON/OFF      RA0

*/

void PassVerify()
{

   gets(clave);
   strcpy(edu, clave);


   a = edu;
   strcpy( d, READ_EEPROM(10));

   printf("%s",a);
   printf("%s",d);

   if (a == d){
     printf("bien");
   }else{
      printf("mal");
   }

}

#INT_RDA // Interrupcion por serial. para recibir datos del movil
void serial_isr()
{
   control = getc();
   if (control == 'X' ){PassVerify();printf("z");}
}



void main()
{

   ENABLE_INTERRUPTS(GLOBAL);  // Se habilitan las interrupciones globales
   ENABLE_INTERRUPTS(INT_RDA); // Se habilitan la interrupcion por evento serial.


   set_tris_A(0x00); //Puerto D como salidas digitales.
   WRITE_EEPROM(10,"1996");

   output_low(PIN_A0);
   output_low(PIN_A1);
   output_high(PIN_B3);
   output_low(PIN_A7);




   delay_ms(50);
   while (true)
   {

   }
}
