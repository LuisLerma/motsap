//Prototipo bytes0//MOTSAP-BETA
//Ingeniero Luis Eduardo Lerma J.
//Agosto 2019

#include <16F628A.h>
#fuses XT, BROWNOUT, NOLVP, NOWDT, PUT, NOMCLR
#use delay(internal = 4MHz)
#use rs232(baud = 9600, parity = N, XMIT = PIN_B2, RCV = PIN_B1, bits = 8)
fast_io(A);
#define MFRC522_CS PIN_B4
#define MFRC522_SCK PIN_B5
#define MFRC522_SI PIN_B6
#define MFRC522_SO PIN_B7
#define MFRC522_RST PIN_A6
//!
#include <Lib_rc522.h>
char llavero1[4] = {0xA6, 0x19, 0x01, 0x2B};
//!
char UID[4], i;
unsigned int TagType;
MFRC522_Init();

int1 C_uid = false, onehit = 0;

//Chronos mitho dios del tiempo.
int8 tiempo_alarma = 0, tiempo_panic = 0;
int8 cont = 0, segundos = 0;
int8 c1 = 0,  c2 = 0,  c3 = 0;
int1 sound = 0;

//Variables - Sistema de seguridad
char clave[5];
char edu[5];
char pass6[5];
char pass[5];
char control;
int8 num_faults = 0;
int8 SecurityLVL = 0;

/*TENER EN CUENTA

BLUETOOTH   RB3
ALARMA      RA1
START       RA7
ON/OFF      RA0

*/

int1 get_UID(char Data[], char UID[])
{
   for (int i = 0; i < 4; i++)
   {
      if (UID[i] == Data[i])
      {
         C_uid = true;
      }
      else
      {
         C_uid = false;
         break;
      }
   }
   return C_uid;
}

#int_TIMER1
void timer_1()
{
   securityLVL = READ_EEPROM(3);
   cont++;
   if (cont == 2)
   {
      if (securityLVL == 0 && sound != 1){
         output_toggle(PIN_A1);
      }
      
      if(securityLVL == 1 && segundos >= 120){
            output_low(PIN_B3);
      }

      if(segundos >= 250){segundos = 0;}
      printf("%d", segundos);
      segundos++;
      cont = 0;
   }

   if(segundos >= tiempo_panic && sound == 1){
      sound = 0;
      output_LOW(PIN_A0);
      output_HIGH(PIN_B3);
      output_low(PIN_A1);
   }
   set_timer1(3036);
}
  


#int_EXT // Interrupcion btn panico
Void PanicRoom()
{
   //Se apaga motocicleta, se enciende bluetooth, se graba lvl seguridad.
   securityLVL = 0;
   segundos = 0;
   WRITE_EEPROM(3, securityLVL);
   sound = 1;

   tiempo_panic = READ_EEPROM(41); 
   enable_interrupts(int_timer1);

   char pass0[4] = {'1','9','9','6'};
   for(int8 v=30 ;v<34;v++){WRITE_EEPROM(v,pass0[v-30]);}

}
  

void tiempoala()
{
//!   gets(clave);
//!   strcpy(edu, clave);
//!   onehit = 1;  

   c3 = edu[0]-'0';
   c2 = edu[1]-'0';
   c1 = edu[2]-'0';

   tiempo_alarma = c1 + (c2*10) + (c3*100);

   if(control == 't' ){
      WRITE_EEPROM(40, tiempo_alarma);
      printf("t");
      printf("%d", tiempo_alarma);
   }

   if(control == 'd' ){
      tiempo_panic = tiempo_alarma;
      WRITE_EEPROM(41, tiempo_panic);
      printf("d");
   }
}

void backdoor()
{

   gets(clave);
   strcpy(edu, clave);
   onehit = 1;

   for(int8 v=30;v<34;v++){pass6[v-30] = READ_EEPROM(v);}   
   if (edu[0] == pass6[0] && edu[1] == pass6[1] && edu[2] == pass6[2] && edu[3] == pass6[3])
   {
      num_faults = 0;
      securityLVL = 1;
      WRITE_EEPROM(3, securityLVL);
      disable_interrupts(int_timer1);
   }
}

void start(){
   output_HIGH(PIN_A7);
   delay_ms(1000);
   output_low(PIN_A7);
}

void PassChange(){
   gets(clave);
   strcpy(edu, clave);
   onehit = 1;

   for(int8 v=10;v<14;v++){ WRITE_EEPROM(v, edu[v-10]);}
//!   printf("%s", edu);
}

void PassVerify()
{
   gets(clave);
   strcpy(edu, clave);
   onehit = 1;

   for(int8 v=10;v<14;v++){pass[v-10] = READ_EEPROM(v);}  
   if (edu[0] == pass[0] && edu[1] == pass[1] && edu[2] == pass[2] && edu[3] == pass[3])    
   {
      output_high(PIN_A0);
      output_LOW(PIN_A1);
      securityLVL = 1;
      WRITE_EEPROM(3, securityLVL);
      start();
      num_faults = 0;      
   }
   else
   {
      output_low(PIN_A0);
      output_HIGH(PIN_B3);
      securityLVL = 0;
      WRITE_EEPROM(3, securityLVL);
      num_faults = num_faults + 1;
      printf("%d", num_faults);  

      if(num_faults >= 9){enable_interrupts(int_timer1);}
   }
  
}

#INT_RDA // Interrupcion por serial. para recibir datos del movil
void serial_isr()
{  
      disable_interrupts(int_timer1);
      control = getc();
      if (control == 'h' && securityLVL == 1 ){PassChange();}
      if (control == 'X' && num_faults < 9 )  {PassVerify();printf("#");}
      if (control == 'X' && num_faults >= 9 ) {backdoor();printf("@");}
      if (control == 't' && securityLVL == 1) {tiempoala();}
      if (control == 'd' && securityLVL == 1) {tiempoala();}
      if (control == '_') {start();}
   
}

//Detecccion de tarjecta inicio
void deteccion()
{
   enable_interrupts(global);
   enable_interrupts(int_timer1);

   if (securityLVL == 1 && segundos >= tiempo_alarma && onehit != 1)
   {
      securityLVL = 0;
      WRITE_EEPROM(3, securityLVL);
      output_LOW(PIN_A0);
      output_HIGH(PIN_B3);
      onehit = 1;
   }

   //!&&  READ_EEPROM(5) == 10
   if (securityLVL == 1 && MFRC522_isCard(&TagType))
   {
      if (MFRC522_ReadCardSerial(&UID))
      {
         if (get_UID(llavero1, UID))
         {
            output_high(PIN_A0);
            output_LOW(PIN_A1);
            securityLVL = 1;
            WRITE_EEPROM(3, securityLVL);
            onehit = 1;
            start();
            enable_interrupts(int_timer1);
            segundos = 0;
         }
      }

      MFRC522_Halt();
   }
}

void main()
{

   set_timer1(3036);
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
   ENABLE_INTERRUPTS(GLOBAL);  // Se habilitan las interrupciones globales
   ENABLE_INTERRUPTS(INT_RDA); // Se habilitan la interrupcion por evento serial.
   ENABLE_INTERRUPTS(INT_EXT); // Se habilitan la interrupcion por evento Externo.

   cont = 0;
   onehit = 0; //Variable para desechar deteccion() una vez usada y evitar su activasion.

   securityLVL = READ_EEPROM(3);
   set_tris_A(0x00); //Puerto D como salidas digitales.

   MFRC522_Init();
   output_low(PIN_A0);
   output_low(PIN_A1);
   output_high(PIN_B3);
   output_low(PIN_A7);
   tiempo_alarma = READ_EEPROM(40);
   delay_ms(300);
   segundos = 0;

   if (securityLVL == 0)
   {
      output_LOW(PIN_A0);
      output_HIGH(PIN_B3);
      onehit = 1;
      enable_interrupts(int_timer1);
   }
   segundos = 0;
   delay_ms(300);
   while (true)
   {
      if (securityLVL == 1 && onehit == 0)
      {
         deteccion();
      }
   }
}
