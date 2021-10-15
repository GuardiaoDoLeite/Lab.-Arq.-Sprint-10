/*
 * Semaforo_USART_mestre.c
 *
 * Created: 25/09/2021 18:01:14
 * Author : Andre Oliveira
 */ 


#define F_CPU 16000000UL		//Frequência de trabalho da CPU
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "PCD8544\nokia5110.h"

//----------------------Protótipos das funções-----------------------------//

void executa_a_cada_250ms();
void velocidade_carro();
void LED_VERMELHO();
void LED_VERDE();
void LED_AMARELO();
void carros_por_minuto();
void LCD(uint8_t vermelho, uint8_t amarelo, uint8_t verde);
void checalux();
void USART_Transmit(unsigned char data);
unsigned char USART_Receive();

//--------------------------Declaração de variáveis globais---------------//

uint8_t vermelho = 1, amarelo = 1, verde = 1 , distancia = 5 , velocidade_ms = 0 , tempo = 0;
uint32_t tempo_1ms = 0 ,tempo_1ms_anterior =0, tempo_1ms_freq_anterior = 0 , tempo_1ms_velocidade = 0;
uint32_t freq_modo_automatico = 0 , frequencia_carros_ms = 0;
uint32_t flag_500ms = 0 , flag_velocidade = 5;
float velocidade_kh = 0;
unsigned char t_0[2], t_1[2], t_2[2], freq[4] , vel[6];
int selecao = 0 , auxiliar = 1, t_vermelho = 1, t_amarelo = 1, t_verde = 1, tempoaux = 0;
int n_carros = 0, conta_frequencia = 0;
int LUX = 0;
unsigned char LE_LUX[4];

//-------------------------------------------------------------------------------------------------------------------------------------------------

// Interrupções

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

ISR(INT0_vect)//Interrupção relacionada ao pino PD3
{
	//--Alteração das variáveis correspondentes a inteligência do contador de carros em função dos pulsos aplicados na porta PD1 ---------------------
	n_carros++;
	carros_por_minuto();
}

ISR(PCINT2_vect) // Interrupção das portas PD4, PD5 e PD6 que representam os botões de seleção
{
	
	
		
	// Botao de seleção de estado
	if(!(PIND & (1<<6)))
	{
		selecao++;
		
		if (selecao == 0)
		{
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		if (selecao == 1)
		{
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		if (selecao == 2)
		{
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		if (selecao == 3)
		{
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		
	}
	
	if (selecao > 3)
	{
		selecao = 0;
		USART_Transmit(amarelo);
		USART_Transmit(vermelho);
		USART_Transmit(verde);
		LCD(vermelho,amarelo,verde);
	}
	
	
	// Botão para aumentar o tempo
	if(!(PIND & (1<<4)))
	{
		if (selecao == 0)	// Aumenta o tempo do vermelho
		{
			if (vermelho >= 9)
			{
				vermelho = 9;
				}else {
				vermelho ++;
			}
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		
		if (selecao == 1)	// Aumenta o tempo do amarelo
		{
			if (amarelo >= 9)
			{
				amarelo = 9;
				}else {
				amarelo ++;
			}
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		
		if (selecao == 2)	// Aumenta o tempo do verde
		{
			if (verde >= 9)
			{
				verde = 9;
				}else {
				verde ++;
			}
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho, amarelo, verde);
		}
		
		if (selecao == 3)	// Controla o tempo no modo automatico
		{
			conta_frequencia = 1;
			verde = (frequencia_carros_ms/60) + 1;
			vermelho = 10 - verde;
			amarelo = 1;
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho, amarelo, verde);
		}
	}
	
	// Botão para diminuir o tempo
	
	if(!(PIND & (1<<5)))
	{
		if (selecao == 0)	// Diminui o tempo do vermelho
		{
			if (vermelho <= 1)
			{
				vermelho = 1;
				}else {
				vermelho --;
			}
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		
		if (selecao == 1)	// Diminui o tempo do amarelo
		{
			if (amarelo <= 1)
			{
				amarelo = 1;
				}else {
				amarelo --;
			}
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		
		if (selecao == 2)	// Diminui o tempo do verde
		{
			if (verde <= 1)
			{
				verde = 1;
				}else {
				verde --;
			}
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
		
		if (selecao == 3)	// Controla no modo automatico
		{
			conta_frequencia = 0;
			verde = (frequencia_carros_ms/60) + 1;
			vermelho = 10 - verde;
			amarelo = 1;
			
			USART_Transmit(amarelo);
			USART_Transmit(vermelho);
			USART_Transmit(verde);
			LCD(vermelho,amarelo,verde);
		}
	}
	
	// Medidor de velocidade
		if((!(PIND & (1<<0)))){
			tempo_1ms_velocidade = tempo_1ms;
			flag_velocidade = 1;
			if((!(PIND & (1<<1)))){
				tempo = (tempo_1ms - tempo_1ms_velocidade);
				velocidade_ms = (distancia/(tempo*1000));
				velocidade_kh = (velocidade_ms*3.6);
				flag_velocidade = 0;
			}
		}
			
			
}

int main(void)
{
	// GPIO
	DDRB	= 0b11111111;							// Habilita os pinos PB0...7 como saida
	DDRD	= ~((1<<2)|(1<<4)|(1<<5)|(1<<6)|(1<<1)|(1<<0));		// Habilita os  pinos  PD2, PD4, PD5 e PD6 como entrada
	PORTD  |= ((1<<2)|(1<<4)|(1<<5)|(1<<6)|(1<<1)|(1<<0));		// Habilita os pull-up de PD2, PD4, PD5 e PD6
	DDRC	= ~((1<<6)|(1<<0));						// Habilita os pinos PC0 e PC6 como entrada
	PORTC  |= (1<<6);								// Habilita o pull-up de PC6

	// Configuração das interrupções
	EICRA  = 0b00000010;		// Interrrupçãp externa INT0 na borda de descida
	EIMSK  = 0b00000001;		// Habilita a interrupção externa INT0
	PCICR  = 0b00000100;		// Habilita a interrupção externa PCINT2
	PCMSK2 = 0b01110011;		// Habilita individualmente as portas PD4, PD5 e PD6
	PCIFR  = 0b00000100;		// Indica se alguma interrupção ocorreu em PCINT2
	
	// Configuração do ADC
	ADMUX  = 0b01000000;		// Vcc como referência, canal 0
	ADCSRA = 0b11100111;		// Habilita o AD, modo de conversão continua, prescaler = 128
	ADCSRB = 0b00000000;		// Modo de conversão continua
	DIDR0  = 0b00111110;		// Habilita apenas o pino PC0 como entrada do ADC0
	
	// Configuração do PWM
	
	//Fast PWM, OC2A e OC2B habilitados
	TCCR2A = 0b00100011;	// PWM não invertido no pino OC2B
	TCCR2B = 0b00000100;	// Liga TC2, prescaler = 64, fpwm =	f0sc/(256*prescaler) = 16MHz/(256*64) = 976Hz
	OCR2B = 256;
	
	/* Configuração  da USART
	UBRR0H = (unsigned char)(MYUBRR>>8);				// Ajusta a taxa de transmissão
	UBRR0L = (unsigned char)MYUBRR;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);			// Habilita o transmissor e o receptor e a interrupção de recepção da USART
	UCSR0C = (3<<UCSZ00);	*/							// Ajusta o formato do frame: 8 bits de dados e 1 de parada
	
	// Interrupções do timer
	TCCR0A = 0b00000010;		// Habilita modo Ctc do TC0
	TCCR0B = 0b00000011;		// Liga TC0 com prescaler = 64
	OCR0A  = 249;				// Ajusta o comparador para o TC0 contar até 249
	TIMSK0 = 0b00000010;		// Habilita a interrupção na igualdade de comparação com OCR0A.
	// A interrupção ocorre a cada 1 ms = (64 * (249 + 1)) / 16 MHz
	sei();						// Habilita interrupções globais, ativando o bit I do SREG
	
	// Configuração do LCD
	nokia_lcd_init();
	LCD(vermelho, amarelo, verde);
	
	while (1)
	{
		LED_VERMELHO();
		LED_VERDE();
		LED_AMARELO();
		carros_por_minuto();
		checalux();
		//velocidade_carro();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

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

void carros_por_minuto(){
	if((tempo_1ms - tempo_1ms_freq_anterior) >= 5000) //contagem de quantos carros passam a cada 5s
	{
		frequencia_carros_ms = n_carros * 12 ;    //Quantos carros passam em 1 minuto
		tempo_1ms_freq_anterior = tempo_1ms;
		n_carros = 0;
		LCD(vermelho,amarelo,verde);
	}
	
}

void LCD(uint8_t vermelho, uint8_t amarelo, uint8_t verde) // Função para animação do Nokia LCD PCD8544
{
	sprintf(vel, "%u", velocidade_kh);
	sprintf(t_0, "%u", vermelho);
	sprintf(t_1, "%u", amarelo);
	sprintf(t_2, "%u", verde);
	sprintf(freq, "%u", frequencia_carros_ms);
	sprintf(LE_LUX, "%u",(1023000/ADC) - 1000);

	nokia_lcd_clear();

	nokia_lcd_set_cursor(48, 0);
	nokia_lcd_write_string("|", 2);
	nokia_lcd_set_cursor(48, 10);
	nokia_lcd_write_string("|", 2);
	nokia_lcd_set_cursor(48, 20);
	nokia_lcd_write_string("|", 2);
	nokia_lcd_set_cursor(48, 30);
	nokia_lcd_write_string("|", 2);
	nokia_lcd_set_cursor(48, 40);
	nokia_lcd_write_string("|", 2);

	nokia_lcd_set_cursor(0, 0);
	nokia_lcd_write_string("Verm:", 1);
	nokia_lcd_set_cursor(37, 0);
	nokia_lcd_write_string(t_0, 1);

	nokia_lcd_set_cursor(0, 13);
	nokia_lcd_write_string("Amar:", 1);
	nokia_lcd_set_cursor(37, 13);
	nokia_lcd_write_string(t_1, 1);

	nokia_lcd_set_cursor(0, 27);
	nokia_lcd_write_string("Verde:", 1);
	nokia_lcd_set_cursor(37, 27);
	nokia_lcd_write_string(t_2, 1);

	if(selecao == 3 && conta_frequencia == 1)
	{
		nokia_lcd_set_cursor(0, 40);
		nokia_lcd_write_string("Autom:", 1);

		}else {
		nokia_lcd_set_cursor(0, 40);
		nokia_lcd_write_string("Manual:", 1);
		//nokia_lcd_set_cursor(37, 42);
		//nokia_lcd_write_string(t_2, 1);
	}
	
	nokia_lcd_set_cursor(60, 5);
	nokia_lcd_write_string(vel, 1);
	nokia_lcd_set_cursor(60, 15);
	nokia_lcd_write_string("VEL", 1);
	
	/*nokia_lcd_set_cursor(60, 5);
	nokia_lcd_write_string(LE_LUX, 1);
	nokia_lcd_set_cursor(60, 15);
	nokia_lcd_write_string("LUX", 1);*/

	nokia_lcd_set_cursor(60, 25);
	nokia_lcd_write_string(freq, 1);
	nokia_lcd_set_cursor(60, 35);
	nokia_lcd_write_string("c/m", 1);

	nokia_lcd_set_cursor(45, 0 + selecao * 13);
	nokia_lcd_write_string("<", 1);

	nokia_lcd_render();
}

void checalux(){
	LUX = (1023000/ADC) - 1000;
	if(flag_500ms = 1){
		
		if((((1023000/ADC) - 1000) <= 300) && ((!(PINC & (1<<6))) || (frequencia_carros_ms > 0)))
		{
		OCR2B = 250;}
		
		else if((((1023000/ADC) - 1000) <= 300) &&  !(!(PINC & (1<<6)) || (frequencia_carros_ms > 0)))
		{
			OCR2B = 77;
		}
		
		else{
			OCR2B = 0;
		}
		
	}
	LCD(vermelho,amarelo,verde);
}

void USART_Transmit(unsigned char data)
{
	while(!( UCSR0A & (1<<UDRE0))); // Espera a limpeza do registrador de transmissão 
	UDR0 = data;					// Coloca o dado no registrador e o envia
}

unsigned char USART_Receive(void)
{
	while(!(UCSR0A & (1<<RXC0)));	//Espera o dado ser recebido
	return UDR0;					//Lê o dado recebido e retorna
}

void velocidade_carro(void){
	if((!(PIND & (1<<0)))){
		flag_velocidade = 1;
		tempo_1ms_velocidade = tempo_1ms;
		if((flag_velocidade == 1)){
			if((!(PIND & (1<<1)))){
				tempo = (tempo_1ms - tempo_1ms_velocidade);
				velocidade_ms = (distancia/(tempo)); //Em metros por segundo;
				velocidade_kh = (velocidade_ms * 3.6);  //COnversão para km/h;
				flag_velocidade = 5;
			}
			}
	}
} 

