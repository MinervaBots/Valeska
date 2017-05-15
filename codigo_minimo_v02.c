#include <codigo_minimo_v01.h>

#define PERIODO 255 //define a constante que ser� usada como per�odo para o ccp

double PWM_ATUAL_DIR = 127;//variaveis globais que guardam o valor atual do pwm em cada motor
double PWM_ATUAL_ESQ = 127;//s�o usadas na rampa de acelera��o na fun��o MOTORS_CONTROL

double TIMER1_PWM_ESQ = 0;
double TIMER1_PWM_DIR = 0;

int CONTADOR_ESQ = 0;
int CONTADOR_DIR = 0;

void INIT_ROBOT()
{
   //inicializa os registradores do interrupt
   ext_int_edge(L_TO_H); //define a mudan�a de estado na subida low to high (ou high to low, caso H_TO_L)
   enable_interrupts(INT_EXT1); //habilita interrup��o pela porta de int externo 1 -- RB14 -- Porta 25
   enable_interrupts(INT_EXT2); //habilita interrup��o pela porta de int externo 2 -- RA06 -- Porta 20
   enable_interrupts(INTR_GLOBAL); //habilita interrup��es gerais
   
   //inicializa o m�dulo CCP1 e timer1 para capture mode, para ser usado na leitura do input pwm do receptor
   // Setup Timer1 and CCP1 for Capture mode so that 
   // we can measure the input signal's frequency. 
   set_timer1(0);            
   setup_timer1(T1_INTERNAL | T1_DIV_BY_1); 
   setup_ccp1(CCP_CAPTURE_RE);
   setup_ccp5(CCP_CAPTURE_RE);
   
   // Clear the CCP1 interrupt flag before we enable 
   // CCP1 interrupts, so that we don't get an unwanted 
   // immediate interrupt (which might happen). 
   clear_interrupt(INT_CCP1); 
   clear_interrupt(INT_CCP5);
   enable_interrupts(INT_CCP1); //porta A6
   enable_interrupts(INT_CCP5); //porta A7
   
   //inicializa os registradores CCP para o PWM
   setup_ccp2(CCP_PWM);  // Configura CCP2 como PWM --- RB11 --- Porta 22
   set_timer_period_ccp2(PERIODO); //seta o per�odo do m�dulo ccp2
   setup_ccp3(CCP_PWM);  // Configura CCP3 como PWM --- RB10 --- Porta 21
   set_timer_period_ccp3(PERIODO); //seta o per�odo do m�dulo ccp3
   
   set_pwm2_duty(127); //seta os dois pwms para come�ar em zero
   set_pwm3_duty(127);
}

void MOTORS_CONTROL(int LEFT_POWER, int RIGHT_POWER)
{
   if(LEFT_POWER > PWM_ATUAL_ESQ) //caso o motor esquerdo tenha que aumentar a velocidade
   {
      for(int ACELERACAO = PWM_ATUAL_ESQ+1; ACELERACAO <= LEFT_POWER; ACELERACAO++)
      {
         //o pwm ser� setado para valores maiores a uma unidade do valor anterior, at� alcan�ar a velocidade m�xima
         //o valor a ser incrementado poder� ser alterado depois de testes
         set_pwm2_duty(256 - ACELERACAO);
         //o "256 - " � usado para normalizar o valor, j� que sem a normaliza��o o valor de maior tens�o seria 1 e o menor 256.
         //com a normaliza��o, o maior volta para o senso comum, sendo 255 e o menor 0.
      }
      
      PWM_ATUAL_ESQ = LEFT_POWER;//o valor do pwm do motor esquerdo � atualizado
   }
   
   else //caso o motor esquerdo tenha que diminuir a velocidade
   {
      for(int ACELERACAO = PWM_ATUAL_ESQ-1; ACELERACAO >= LEFT_POWER; ACELERACAO--) 
      {
         //o pwm ser� setado para valores menores a uma unidade do valor anterior, at� alcan�ar a velocidade m�nimo
         //o valor a ser decrementado poder� ser alterado depois de testes
         set_pwm2_duty(256 - ACELERACAO);
      }
      
      PWM_ATUAL_ESQ = LEFT_POWER;//o valor do pwm do motor esquerdo � atualizado
   }
   
   if(RIGHT_POWER > PWM_ATUAL_DIR) //caso o motor direito tenha que aumentar a velocidade
   {
      for(int ACELERACAO = PWM_ATUAL_DIR+1; ACELERACAO <= RIGHT_POWER; ACELERACAO++)
      {
         set_pwm3_duty(256 - ACELERACAO);
      }
      
      PWM_ATUAL_DIR = RIGHT_POWER;//o valor do pwm do motor direito � atualizado
   }
   
   else //caso o motor direito tenha que diminuir a velocidade
   {
      for(int ACELERACAO = PWM_ATUAL_DIR-1; ACELERACAO >= LEFT_POWER; ACELERACAO--)
      {
         set_pwm3_duty(256 - ACELERACAO);
      }
      
      PWM_ATUAL_DIR = RIGHT_POWER;//o valor do pwm do motor direito � atualizado
   }
}

#INT_EXT1
void  ext1_isr(void) 
{
   clear_interrupt(INT_EXT1);//zera a flag de interrupt externo 1
   MOTORS_CONTROL(80, 0); 
   delay_ms(1000);
   MOTORS_CONTROL(127, 127);
}

#INT_EXT2
void  ext2_isr(void) 
{
   clear_interrupt(INT_EXT2);//zera a flag de interrupt externo 2
   MOTORS_CONTROL(0, 80);
   delay_ms(1000);
   MOTORS_CONTROL(127, 127);
}

#INT_CCP1
void  ccp1_isr(void) 
{
   //mede a diferen�a de tempo entre um interrupt e o outro
   TIMER1_PWM_ESQ = get_timer1() - TIMER1_PWM_ESQ;
   CONTADOR_ESQ++;
   if((CONTADOR_ESQ+1 % 2) | (CONTADOR_DIR+1 % 2))
   {
      MOTORS_CONTROL(TIMER1_PWM_ESQ, TIMER1_PWM_DIR);
   }
}

#INT_CCP5
void  ccp5_isr(void) 
{
   //mede a diferen�a de tempo entre um interrupt e o outro
   TIMER1_PWM_DIR = get_timer1() - TIMER1_PWM_DIR;
   CONTADOR_DIR++;
   if((CONTADOR_ESQ+1 % 2) | (CONTADOR_DIR+1 % 2))
   {
      MOTORS_CONTROL(TIMER1_PWM_ESQ, TIMER1_PWM_DIR);
   }
}

void main()
{
   INIT_ROBOT();//chama a fun��o para inicializar o setup dos ints
   //rotina normal de funcionamento do rob�, no caso, somente o by-pass do receptor para o controlador
   while(TRUE)
   {
   }
}
