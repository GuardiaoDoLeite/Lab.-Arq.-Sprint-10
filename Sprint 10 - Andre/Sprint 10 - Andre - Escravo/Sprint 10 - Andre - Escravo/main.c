/*
 * Semaforo_USART_escravo.c
 *
 * Created: 25/09/2021 20:18:57
 * Author : Andre Oliveira
 */ 

#define F_CPU 16000000UL		//Frequência de trabalho da CPU
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

//-------------------------------------------------------------------------------------------------------------------------------------------------

// Protótipos das funções

void LED_VERMELHO();
void LED_VERDE();
void LED_AMARELO();

//-------------------------------------------------------------------------------------------------------------------------------------------------

// Variaveis globais

int16_t liberado = 0, cont = 0, resultado;		// Variaveis da recepção de dados
uint8_t vermelho = 1, amarelo = 1, verde = 1;
uint32_t tempo_1ms = 0 ,tempo_1ms_anterior =0, tempo_1ms_freq_anterior = 0;
uint32_t freq_modo_automatico = 0 , frequencia_carros_ms = 0;
uint32_t flag_500ms = 0;
unsigned char t_0[2], t_1[2], t_2[2], freq[4];
int selecao = 0 , auxiliar = 1, t_vermelho = 1, t_amarelo = 1, t_verde = 1, tempoaux = 0;
int n_carros = 0, conta_frequencia = 0;
int LUX = 0;
unsigned char LE_LUX[4];

//-------------------------------------------------------------------------------------------------------------------------------------------------

// Interrupções

ISR(USART_RX_vect) // Interrupção para a recepção de dados
{
	char RX_dado;
	RX_dado = UDR0;
	
	switch(cont)
	{
		case 0:
			amarelo = RX_dado;
			cont ++;
			break;
		case 1:
			vermelho = RX_dado;
			cont ++;
			break;
		case 2:
			verde = RX_dado;
			cont = 0;
			break;
		default:
		
			break;
			
	}
}

ISR(TIMER0_COMPA_vect) //Interrupção do TC0 a cada 1ms
{
	tempo_1ms++;
	if((tempo_1ms%500)==0){ //Verdadeiro a cada 500ms
		flag_500ms = 1;
	}
	else{
		flag_500ms = 0;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------

// Função main

int main(void)
{
	// GPIO
	DDRB	= 0b11111111;		// Habilita os pinos PB0...7 como saida
	DDRD    = (1<<7);		// Habilita os pinos PD7 como saida
	
	// Configuração  da USART
	UBRR0H = (unsigned char)(MYUBRR>>8);				// Ajusta a taxa de transmissão
	UBRR0L = (unsigned char)MYUBRR;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);			// Habilita o transmissor e o receptor e a interrupção de recepção da USART
	UCSR0C = (3<<UCSZ00);								// Ajusta o formato do frame: 8 bits de dados e 1 de parada
	
	// Interrupções do timer
	TCCR0A = 0b00000010;		// Habilita modo Ctc do TC0
	TCCR0B = 0b00000011;		// Liga TC0 com prescaler = 64
	OCR0A  = 249;				// Ajusta o comparador para o TC0 contar até 249
	TIMSK0 = 0b00000010;		// Habilita a interrupção na igualdade de comparação com OCR0A.
	// A interrupção ocorre a cada 1 ms = (64 * (249 + 1)) / 16 MHz
	sei();						// Habilita interrupções globais, ativando o bit I do SREG
	
	while (1)
	{
		LED_VERMELHO();
		LED_VERDE();
		LED_AMARELO();
	}
}

// Implementação das funções

void LED_VERMELHO(){
	t_vermelho = (vermelho*1000)/4;
	if(auxiliar == 1){
		PORTB = 0b11110000; //Pinos PB4...7 em nivel alto
		if((tempo_1ms - tempo_1ms_anterior) >= t_vermelho)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b11100000; //Pinos PB4...6 em nivel alto
			auxiliar++;
		}
		//else{
		//tempoaux = 0;
		//}
	}
	if(auxiliar == 2){
		if((tempo_1ms - tempo_1ms_anterior) >= t_vermelho)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b11000000; //Pinos PB4...5 em nivel alto
			auxiliar++;
		}
		//else{
		//tempoaux = 0;
		//}
	}
	if(auxiliar == 3){
		if((tempo_1ms - tempo_1ms_anterior) >= t_vermelho)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b10000000; //Pino PB4 em nivel alto
			auxiliar++;
		}
		else{
			tempoaux = 0;
		}
	}
	if(auxiliar == 4){
		if((tempo_1ms - tempo_1ms_anterior) >= t_vermelho)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b00000000; //Pinos PB em nivel baixo
			auxiliar++;
		}
		else{
			tempoaux = 0;
		}
	}
}

void LED_AMARELO(){
	t_amarelo = (amarelo*1000);
	if(auxiliar == 5){
		PORTD  |= (1<<7);  //Pino PD7 em nivel alto
		if((tempo_1ms - tempo_1ms_anterior) >= t_amarelo)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTD  &= ~(1<<7);  //Pinos PB7 em nivel baixo;
			auxiliar++;
		}
		else{
			tempoaux = 0;
		}
	}
	
}

void LED_VERDE(){
	t_verde = (verde*1000)/4;
	if(auxiliar == 6){
		PORTB = 0b00001111; //Pinos PB0...3 em nivel alto
		if((tempo_1ms - tempo_1ms_anterior) >= t_verde)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b00001110; //Pinos PB0...2 em nivel alto
			auxiliar++;
		}
		else{
			tempoaux = 0;
		}
	}
	if(auxiliar == 7){
		if((tempo_1ms - tempo_1ms_anterior) >= t_verde)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b00001100; //Pinos PB0...1 em nivel alto
			auxiliar++;
		}
		else{
			tempoaux = 0;
		}
	}
	if(auxiliar == 8){
		if((tempo_1ms - tempo_1ms_anterior) >= t_verde)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b00001000; //Pino PB0 em nivel alto
			auxiliar++;
		}
		else{
			tempoaux = 0;
		}
	}
	if(auxiliar == 9){
		if((tempo_1ms - tempo_1ms_anterior) >= t_verde)
		{
			tempo_1ms_anterior = tempo_1ms;
			PORTB = 0b00000000; //Pinos PB0 em nível baixo
			auxiliar = 1;
		}
		else{
			tempoaux = 0;
		}
	}
}