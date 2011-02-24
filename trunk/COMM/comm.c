#include "D:\Mis Documentos\Programacion\COMM\comm.h"
// this tells the compiler that the rtos functionality will be needed, that
// timer0 will be used as the timing device, and that the minor cycle for
// all tasks will be 1 milisecond
#use rtos(timer=0,minor_cycle=1ms)		//1ms

char LEDS_DIGITS[12]= {'1','1','8','0','0','0','1','1','8','0','0','0'};        //Valor de cada digito en el LCD



//BYTE flagSendBus = FALSE;
int8 ENC, ENC_PUSH, TEST, TFR;				// 0:IDLE  1: TRANSITORIO  2:ESTABLE
int1 TestEnabled=1;							// Indica que el test de led está activo
unsigned int8 DebounceTimerEncPush = 0;
unsigned int8 DebounceTimerTest = 0;
unsigned int8 DebounceTimerTfr = 0;
 
BYTE KHZ_SELECT;                                // Seleciona KHz o MHz
signed int16 FREC_MHZ, FREC_KHZ;
signed int16 STBY_FREC_MHZ, STBY_FREC_KHZ;

// Defines the destination's RS485 ID
//#define  RS485_DEST_ID        0x00

// Defines the device's RS485 ID
//#define  RS485_ID             0x01
//#define  RS485_USE_EXT_INT    FALSE
//#define RS485_RX_ENABLE    PIN_C5   // Controls RE pin.  Should keep low.
//#include <rs485.c>
//int8 buffer[40];
//int8 next_in  = 0;
//int8 next_out = 0;
//BYTE flagLight=0;
int8 semaphore; //Este es nuestro semáforo



//#task(rate=1s)
//void Task_SaveEEPROM();

#task(rate=1ms)
void Task_TestLeds();

#task(rate=1ms)
void Task_Select();

#task(rate=10ms)
void Task_Encoder();

#task(rate=1ms)
void Task_TFR();

#task(rate=1ms)
void Task_Debounce();

void EnableLight(int8 State)
{
if(!State)
   output_high(PIN_A5);
  else
   output_low(PIN_A5);
}




#int_EXT
void  STANDBY_isr(void) 
{
static int first=0;
	if(first==0){			// descarta la primera interrupción
		first=1;
		return;
	}
 	DebounceTimerTfr = 50;
	TFR=1;
}




#int_RB
void  RB_isr(void) 
{
 int current;
 static int last=0xFF;

   	current=input_b();
	if(TestEnabled == 0){
   		if (bit_test(current,6)!=bit_test(last,6)) {
			if(input(PIN_B6)  ) {		// Flanco de subida
				DebounceTimerEncPush = 100;
				ENC_PUSH = 1;
			}  
		}
	}
   	if (bit_test(current,7)!= bit_test(last,7)) {		//Cambio B7
		DebounceTimerTest=100;
		TEST = 1;	
	}       

   last=current;
}

// Funcion encarga de desabilitar todas las tareas
/*void Task_Disabler()
{ 
	//rtos_disable(LED_V); 
	//rtos_disable(LED_A); 
	//rtos_disable(LEDS); 
	rtos_disable(Task_Disabler); 
} */



#task(rate=1ms)
//Tarea encargada de refrescar el display cada ms.
// Como son 12 display, tardará en resfrescar todos, en 12 ms.
void LCD_rtos_task ( )
{
BYTE portc,porta;
BYTE Value;
static BYTE Index=0;
   
   //Escribe el codificador BCD
   Value = LEDS_DIGITS[Index] - 0x30;
   portc = input_c();
   portc &= 0xF0;
   portc += (Value&0x0F);
   output_c(portc);

   
   // Escribe en el multiplexor para seleccionar el siguiente
   porta = input_a();
   porta = porta & 0xF0;
   porta = porta + ((11-Index)&0x0F);
   output_a(porta);

   
   //Cambia al siguiente caracter
   Index++;
   if(Index==12){
	 Index = 0;
	}


}

/*
#task(rate=100ms)
void Bus_rtos_task ( )
{
int8  msg[32];
	return;
	if(rs485_get_message(msg, FALSE)== TRUE) {
		
	}
	if(flagSendBus==TRUE){
		rs485_wait_for_bus(TRUE);
		while(!rs485_send_message(RS485_DEST_ID,30, msg)) {
            		delay_ms(RS485_ID);
        	 }

		flagSendBus = FALSE;
	}		
}
*/

void Task_Debounce()
{
	if(ENC_PUSH ==1){
		if(DebounceTimerEncPush > 0) DebounceTimerEncPush--;
		else {
			if(input(PIN_B6) ) ENC_PUSH = 2;
			else ENC_PUSH = 0;
		}
	}
	if(TEST == 1){
		if(DebounceTimerTest > 0) DebounceTimerTest--;
		else {
			TEST = 2;
		}
	}

	if(TFR == 1)
	{
		if(DebounceTimerTfr > 0) DebounceTimerTfr--;
		else {
			if(input(PIN_B0) ) TFR = 2;
			else TFR = 0;
		}
	}
		   
	
}



void Task_TFR()
{
int x;
 signed int16 TMP_FREC;
	rtos_await(TFR==2);			//Esperamo que pulse el boton TFR
	if(TestEnabled == 0) {

	  	TMP_FREC = FREC_MHZ;
	    FREC_MHZ = STBY_FREC_MHZ;
	    STBY_FREC_MHZ = TMP_FREC;
	 
	    TMP_FREC = FREC_KHZ;
	    FREC_KHZ = STBY_FREC_KHZ;
	    STBY_FREC_KHZ = TMP_FREC;
	 
	    // Escribir los valores en el buffer
		rtos_wait(semaphore);
	    WriteFrecToBuffer(FREC_MHZ,FREC_KHZ,STBY_FREC_MHZ,STBY_FREC_KHZ);
		rtos_signal(semaphore);
		
		// Almacenar el EEPROM la ultima frecuencia
		for(x=0;x<12;x++)
			write_eeprom(x,LEDS_DIGITS[x]);
	}

	TFR=0;
}


void Task_Encoder()
{
int8 x;
//static int Cont=0;
signed char Direction;

	if(TestEnabled == 1) return;
	
	Direction = Check_Encoder();
	if(Direction != 0){
		//rtos_wait(semaphore);
	    if(KHZ_SELECT){
	       STBY_FREC_KHZ =STBY_FREC_KHZ + (Direction*25);
	       if(STBY_FREC_KHZ > 975)   STBY_FREC_KHZ=0;
	       else if(STBY_FREC_KHZ < 0)   STBY_FREC_KHZ=975;
	
	    }
	    else{
	       STBY_FREC_MHZ =STBY_FREC_MHZ + Direction;
	       if(STBY_FREC_MHZ > 136)   STBY_FREC_MHZ=118;
	       else if(STBY_FREC_MHZ < 118)   STBY_FREC_MHZ=136;
		
	    }
		//rtos_signal(semaphore);
		// Escribir los valores en el buffer
		
	//	rtos_disable (LCD_rtos_task) ;
    	WriteFrecToBuffer(FREC_MHZ,FREC_KHZ,STBY_FREC_MHZ,STBY_FREC_KHZ);
		

		//rtos_enable (LCD_rtos_task) ;
	
		for(x=6;x<12;x++)
			write_eeprom(x,LEDS_DIGITS[x]);


	}
             
}




//Tarea de seleccion de KHz y MHz
void Task_Select()
{

	rtos_await(ENC_PUSH==2);		// Esperamos que pulse el boton de cambio de frecuencia
 	// Conmuta KHz y MHz
    KHZ_SELECT = ~KHZ_SELECT;
	//DebounceTimerEncPush = 100;		// debounce de 50ms
	//rtos_disable(Task_Select); 
	ENC_PUSH = 0;
}


// Tarea del test de leds
void Task_TestLeds()
{
	rtos_await(TEST==2);		// Esperamos que pulse el boton de test
	if(!input(PIN_B7)){        // Si se mantiene pulsado
        WriteFrecToBuffer(888,888,888,888); 
		TestEnabled=1;
    }
    else{
    	WriteFrecToBuffer(FREC_MHZ,FREC_KHZ,STBY_FREC_MHZ,STBY_FREC_KHZ);
		TestEnabled=0;
    }
	TEST=0;

}

//Tarea para hacer el test inicial de leds
#task(rate=100ms)
void InitCheck()
{
static BYTE Cont=0;
    if(Cont++ < 30) return;
	rtos_wait(semaphore);
    WriteFrecToBuffer(FREC_MHZ,FREC_KHZ,STBY_FREC_MHZ,STBY_FREC_KHZ);
	rtos_signal(semaphore);
	enable_interrupts(GLOBAL);		// Habilita las interrupciones
	TestEnabled=0;
	rtos_disable(InitCheck);		// Auto desabilitarse
}



void WriteFrecToBuffer(int16 Frec1, int16 Frec2, int16 Frec3, int16 Frec4)
{
unsigned char buf[6];
	
   	sprintf(buf,"%3Lu",Frec1);
   	memcpy(&LEDS_DIGITS[0], buf,3);
   	sprintf(buf,"%3Lu",Frec2);
   	memcpy(&LEDS_DIGITS[3], buf,3);
   	sprintf(buf,"%3Lu",Frec3);
   	memcpy(&LEDS_DIGITS[6], buf,3);
   	sprintf(buf,"%3Lu",Frec4);
   	memcpy(&LEDS_DIGITS[9], buf,3);
   

}




//Compruba si el encoder a girado
//Devuelve : 
// 1 si gira hacia la derecha
// 0 NO ha girado
// -1 si gira hacia la izquierda
signed char Check_Encoder()
{
BYTE const enc_sequence[4] = {0,2,3,1};
static BYTE enc_seq_index;
static BYTE OldValue=0;
BYTE Value;
BYTE Index;

   
   Value = input_b();

   Value = 2*bit_test(Value,5) + bit_test(Value,4);
   if(OldValue == Value) return 0;
   Index = (enc_seq_index+1)&3;
   if(Value==enc_sequence[Index]) {
      enc_seq_index=Index;
      OldValue = Value;
      return 1;
   }
   Index = (enc_seq_index-1)&3;
   if(Value==enc_sequence[Index]) {
      enc_seq_index=Index;
      OldValue = Value;
      return -1;
   }
   //Error
   return 0;
}

void main()
{
int x;
   	port_b_pullups(0xFF);         
   	setup_adc_ports(NO_ANALOGS);
   	setup_adc(ADC_OFF);
   	setup_spi(SPI_SS_DISABLED);
   	setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   	setup_timer_1(T1_DISABLED);
   	setup_timer_2(T2_DISABLED,0,1);
   	enable_interrupts(INT_RB7);
	enable_interrupts(INT_RB6);
	EXT_INT_EDGE(L_TO_H);
	enable_interrupts(INT_EXT);
 

   set_tris_a(0x00);       // Todo el puerto A son salidas
   //set_tris_b(0xFF);       // Todo el puerto B son entradas
   set_tris_c(0xF0);       // 
   
   output_a(0);            // Multiplexor selecciona el primer dígito
   KHZ_SELECT =0;          //Primero modificamos los MHz
   TEST =0;    				// No se ha pulsado el boton de test
   ENC_PUSH = 0;  // No se ha pulsado el encoder
   ENC = 0;       // No se ha girado el encoder
   semaphore = 1; //Solo una tarea puede utilizar el recurso cada vez


	Check_Encoder();   //Incia el primer valor de encoder

   // Estas frecuencias se leen de la eeprom
	for(x=0;x <12;x++)
		LEDS_DIGITS[x] = (char)read_eeprom(x);
	

	FREC_MHZ =(LEDS_DIGITS[0]-0x30)*100 + (LEDS_DIGITS[1]-0x30)*10 + (LEDS_DIGITS[2]-0x30);
	FREC_KHZ = (LEDS_DIGITS[3]-0x30)*100 + (LEDS_DIGITS[4]-0x30)*10 + (LEDS_DIGITS[5]-0x30);
	FREC_KHZ=FREC_KHZ - (FREC_KHZ%25);
	STBY_FREC_MHZ = (LEDS_DIGITS[6]-0x30)*100 + (LEDS_DIGITS[7]-0x30)*10 + (LEDS_DIGITS[8]-0x30);
	STBY_FREC_KHZ = (LEDS_DIGITS[9]-0x30)*100 + (LEDS_DIGITS[10]-0x30)*10 + (LEDS_DIGITS[11]-0x30);
	STBY_FREC_KHZ=STBY_FREC_KHZ - (STBY_FREC_KHZ%25);


//Test del display
   WriteFrecToBuffer(888,888,888,888);
   
  // rs485_init();

EnableLight(0);
// rtos_run begins the loop which will call the task functions above at the
   // schedualed time
   rtos_run ( );
}
