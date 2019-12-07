#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pic18f4520.h>
#include <pic18.h>
#include <time.h>
#include "configura.h"
#include "lcd.h"

#define NIVEIS 6
#define _XTAL_FREQ 4000000

//Botoes
#define botao1 PORTCbits.RC1
#define botao2 PORTCbits.RC2
#define botao3 PORTCbits.RC3
#define botao4 PORTCbits.RC0

//Leds
#define led1 PORTBbits.RB7
#define led2 PORTBbits.RB6
#define led3 PORTBbits.RB5
#define led4 PORTBbits.RB0

#define max_pessoas 100 //define o numero maximo de pessoas permitidas no local

int quant_pessoas = 0;
int estado = 0;
int num_mostra = 0;
int estado;
char num[3];

void acende(int quant_pessoas){
    
    if(quant_pessoas == 0)
    {
        //todas as luzes apagadas
        led1 = 0;
        led2 = 0;
        led3 = 0;
        led4 = 0;
    }
    if(quant_pessoas > 0 && quant_pessoas <= max_pessoas*0,25)
    {
        // 1/4 das luzes do local acessas
        led1 = 1;
    }
    if(quant_pessoas > max_pessoas*0,25 && quant_pessoas <= max_pessoas*0,5)
    {
        // metade das luzes do local acessas
        led1 = 1;
        led2 = 1;
    }
    if(quant_pessoas > max_pessoas*0,5 && quant_pessoas <= max_pessoas*0,75)
    {
        // 3/4 das luzes do local acessas
        led1 = 1;
        led2 = 1;
        led3 = 1;
    }
    if(quant_pessoas > max_pessoas*0,75 && quant_pessoas <= max_pessoas)
    {
        // todas as luzes do logar acessas
        led1 = 1;
        led2 = 1;
        led3 = 1;
        led4 = 1;
    }
}

void mostra(int quant_pessoas){
    //irá mostrar o número de pessoas que estão dentro do local
    lcd_init();
  
    if(num_mostra > quant_pessoas)
    {
        __delay_ms(5);
        lcd_clear();
        lcd_goto(3);        
        lcd_puts("Entrando");        
        __delay_ms(5000);
        lcd_clear();
        lcd_goto(3);
        sprintf(num, "%d", num_mostra);
        lcd_puts(num);
    }
    if(num_mostra < quant_pessoas)
    {
        __delay_ms(5);
        lcd_clear();
        lcd_goto(3);
        lcd_puts("Saindo");
         __delay_ms(5000);
        lcd_clear();
        lcd_goto(3);
        sprintf(num, "%d", num_mostra);
        lcd_puts(num);
    }
}

void atualza_entrada()
{
    //incrementa contador
    quant_pessoas = quant_pessoas + 1;
    mostra(quant_pessoas);
    //ilumina 
    acende(quant_pessoas);
}

void gira_horario()
{
    if(estado != 1)
    {
        //inicia a abertura para que a pessoa entre
        //__delay_ms(tempo pra executar);
        //desliga motor
        atualza_entrada();
    }
}
void gira_horarioF()
{
    //libera a saida
    __delay_ms(5);
    lcd_clear();
    lcd_goto(3);
    lcd_puts("Saindo");
     __delay_ms(5000);
}

void bloqueada(int estado, int quant_pessoas)
{
    if(estado == 3) //entrada bloqueada
    {
        __delay_ms(5);
        lcd_clear();
        lcd_goto(3);
        lcd_puts("Entrada Bloqueada");
         __delay_ms(5000);
        lcd_goto(40);
        sprintf(num, "%d", num_mostra);
        lcd_puts(num);
    }
    if(estado == 4) //entrada bloqueada
    {
        __delay_ms(5);
        lcd_clear();
        lcd_goto(3);
        lcd_puts("Saida Bloqueada");
         __delay_ms(5000);
        lcd_goto(40);
        num_mostra = quant_pessoas;
        sprintf(num, "%d", num_mostra);
        lcd_puts(num);
    }
    if(estado == 1) //entrada bloqueada
    {
        __delay_ms(5);
        lcd_clear();
        lcd_goto(3);
        lcd_puts("Limite maximo");
        lcd_goto(40);
        lcd_puts("atingido."); 
        __delay_ms(5000);
    }
}

void gira_anti_horario()
{
    //inicia a abertura para que a pessoa saia
    //__delay_ms(tempo pra executar);
    //desliga motor
    
    if(estado == 1 && quant_pessoas != max_pessoas)
    {
        estado = 0;
    }
}

void __interrupt(high_priority) tmr (void)          
{
    if(RCIF)                                        
    {
       RCIF = 0;                                   
       if(RCREG == 3){ //ao receber o valor, inicia o fechamento do portão da entrada
           gira_anti_horario(); 
           bloqueada(3, quant_pessoas);
       }
       else if(RCREG == 4){ //ao receber o valor, inicia o fechamento do portão da saida
           gira_horario();
           bloqueada(4, quant_pessoas);
       }
       else if(quant_pessoas == 100){ //ao receber o valor, inicia o fechamento do portão da saida
           estado = 1;
       }
	}
}

void main(int argc, char** argv) {
    config();
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    TMR0 = 6;
    
    num_mostra = 0;
    quant_pessoas = 0;
    
    while(1)
    {
        //Inicio codigo de validação de entrada e saida por botões
        if (!botao1) 
        {            
            __delay_ms(55);
            if(!botao1){
                __delay_ms(55);
                while (!botao1);
                //chama função de abrir portão para entrar
                gira_horario();
            }
        }
        
        if(!botao3){
            __delay_ms(55);
            if(!botao3){
                __delay_ms(55);
                while (!botao3){}
                gira_anti_horario();  
            }                         
        }
        
        if (!botao2) 
        {
            __delay_ms(55);
            if(!botao2){
                __delay_ms(55);
                while (!botao2){};
                //chama função de abrir portão para sair
                gira_anti_horario();
                //decrementa contador
                quant_pessoas = quant_pessoas - 1;
                //ilumina
                acende(quant_pessoas); 
            }            
        }
        if(!botao4){
            __delay_ms(55);
            if(!botao4){
                __delay_ms(55);
                while (!botao4){}
                gira_horarioF();  
            }                             
        }
        //Fim codigo de validação de entrada e saida por botões
    }
}


