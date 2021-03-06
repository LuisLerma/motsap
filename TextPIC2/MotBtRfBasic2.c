//Prototipo bytes0//MOTSAP-BETA
//Ingeniero Luis Eduardo Lerma J.
//Agosto 2019

#include <16F628A.h>   
#fuses   XT, BROWNOUT, NOLVP, NOWDT, PUT, NOMCLR
#use     delay(internal=4MHz)  
#use rs232(baud=9600,parity=N,XMIT=PIN_B2, RCV=PIN_B1,bits=8)
fast_io(A);
#define MFRC522_CS  PIN_B3               
#define MFRC522_SCK PIN_B4
#define MFRC522_SI  PIN_B5
#define MFRC522_SO  PIN_B6            
#define MFRC522_RST PIN_B7 
//!
#include<Lib_rc522.h>

char Tarjeta1[4] ={0xA6,0x19,0x01,0x2B};
char Tarjeta2[4] ={0x6D,0xFE,0xAA,0x59};  
char llavero1[4] ={0x69,0x48,0xC8,0x73};
//!
char UID[4], i;
unsigned int TagType; 
MFRC522_Init ();

int1 C_uid=false, onehit = 0;

//Chronos mitho dios del tiempo.
int16 tiempo_alarma = 0;
int8  cont = 0, segundos = 0;

//Variables - Sistema de seguridad
char a, b, c, d; char e, f, g, h;
char clave[10];
char edu[10];
char control;
int8 SecurityLVL = 0;

int1 get_UID(char Data[],char UID[] ){
         for(int i = 0; i < 4; i++){   
           if(UID[i]== Data[i]){C_uid=true;}else{C_uid=false;break;}
           } 
   return C_uid;
}

#int_EXT // Interrupcion btn panico 
Void PanicRoom(){
        //Se apaga motocicleta, se enciende bluetooth, se graba lvl seguridad.
            delay_ms(5000);
            output_LOW(PIN_A0);
            output_HIGH(PIN_A3);
            output_low(PIN_A1);
            securityLVL = 0;
            WRITE_EEPROM(3, securityLVL);
            onehit = 1;
            enable_interrupts(int_timer1);
}
#int_TIMER1
void timer_1(){
         securityLVL = READ_EEPROM(3);
         cont ++;
         if(cont == 2){
            if(securityLVL == 0){
            output_toggle(PIN_A1);
               }
            segundos++;
            cont = 0;
          } 
         set_timer1(3036);
}

void PassChange(){
      disable_interrupts(int_timer1);
      gets(clave);
      strcpy(edu, clave);
      onehit = 1;
      
      WRITE_EEPROM(10, edu[1]);
      WRITE_EEPROM(11, edu[2]);
      WRITE_EEPROM(12, edu[3]);
      WRITE_EEPROM(13, edu[4]);     
      }
   
void PassVerify(){
      disable_interrupts(int_timer1);
      gets(clave);
      strcpy(edu, clave);
      onehit = 1;
      
      a = edu[1] ;
      b = edu[2] ;
      c = edu[3] ;
      d = edu[4] ;
      
      e = READ_EEPROM(10);
      f = READ_EEPROM(11);
      g = READ_EEPROM(12);
      h = READ_EEPROM(13);
       
      if(a == e && b == f && c == g && d == h){
         output_high(PIN_A0);
         output_LOW(PIN_A1);
         output_LOW(PIN_A3);
         securityLVL = 1; 
         WRITE_EEPROM(3, securityLVL);
            }else {
               output_low(PIN_A0); 
               output_HIGH(PIN_A3);
               securityLVL = 0;
               WRITE_EEPROM(3, securityLVL);
               enable_interrupts(int_timer1);
               }
}

#INT_RDA // Interrupcion por serial. para recibir datos del movil
void serial_isr(){
     
      control = getc();
      if(control == 'h'){PassChange();}
      if(control == 'O'){PassVerify();}

      printf("%s",edu);
}

//Detecccion de tarjecta inicio
void deteccion(){
   enable_interrupts(global);
   enable_interrupts(int_timer1);
   tiempo_alarma = 10;
   
      securityLVL = READ_EEPROM(3);
      
      if (securityLVL == 1 && segundos == tiempo_alarma){  
            securityLVL = 0;
            WRITE_EEPROM(3, securityLVL);
            output_LOW(PIN_A0);
            output_HIGH(PIN_A3);
            onehit = 1;
            enable_interrupts(int_timer1);
           
           }
            
//!&&  READ_EEPROM(5) == 10
      if (securityLVL == 1 && MFRC522_isCard (&TagType) ){ 
            if (MFRC522_ReadCardSerial (&UID)){
               if(get_UID(Tarjeta1,UID) || get_UID(Tarjeta2,UID)|| get_UID(llavero1,UID)){
                  disable_interrupts(int_timer1); 
                  output_high(PIN_A0);
                  output_LOW(PIN_A1);
                  securityLVL = 1;
                  WRITE_EEPROM(3, securityLVL);
                  onehit = 1;
                        }
                  }

         MFRC522_Halt () ;
         disable_interrupts(int_timer1);
      }
      
}
    
void main(){

   set_timer1(3036);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
   ENABLE_INTERRUPTS(GLOBAL); // Se habilitan las interrupciones globales 
   ENABLE_INTERRUPTS(INT_RDA); // Se habilitan la interrupcion por evento serial.
   ENABLE_INTERRUPTS(INT_EXT);// Se habilitan la interrupcion por evento Externo.
   
   cont = 0;
   onehit = 0; //Variable para desechar deteccion() una vez usada y evitar su activasion.
   
   securityLVL = READ_EEPROM(3);
   set_tris_A(0x00); //Puerto D como salidas digitales.

   MFRC522_Init ();
   output_low(PIN_A0);
   output_low(PIN_A1);
   output_low(PIN_A3);
   

   
   if(securityLVL == 0){
      output_LOW(PIN_A0);
      output_HIGH(PIN_A3);
      onehit = 1;
      enable_interrupts(int_timer1);
         }
         
   delay_ms(50);
   while (true){
   if(securityLVL == 1 && onehit == 0){
      deteccion(); 
      }
   }
}

