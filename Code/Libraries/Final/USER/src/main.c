#include "headfile.h"
#include <math.h>
float angle;
float angle_origin=-20;
float except_angle=0;//负数向左拐，正数向右拐
int icm_gyro_x_last;
float acc_ratio = 1.6 ;//1.6        
float gyro_ratio = 4.08;//4.08    
float dt = 0.001; 
double send_to_PC[10];
char tail[4] = {0x00, 0x00, 0x80, 0x7f};   
 uint16 adc_l,adc_r;
 uint16 position,last_position;
 float kp_a=-0.040;//目前这个组合可以，PWM值在20ms内波动在30上下
 float kd_a=-0.015;//试了一下，速度给够可以上坡，但是下坡后就倒

 float now;
 int addnow=0;
 long intergral=0;
 int EX=0;
 double ki=0.00030;//0.0002;//0.00023;//0.00016;//0.000304;//0.00065;//0.009 low 0.01 high//0.012//0.010//0.0113//0.010//0.010
 double kp=0.0460;//0.073 ;//0.045 low 0.03 high//0.095//0.080//0.110//0.140//130
 double kd=-0.0046;//-0.007;//-0.01 low -0.01 high//0.009//0.011//0.008//0.0107//0.0103
 int cnt=0;
 double sum=0;

/******/
   float ki_m=0.565;//先调i
	 float kp_m=-0.040;//目前这个组合可以，PWM值在20ms内波动在30上下
   float kd_m=-0.015;//试了一下，速度给够可以上坡，但是下坡后就倒
   int target=162;//158 直跑;//范围110~300+
	 
   int temp_pluse = 0,temp_pluse_real = 0;
   int speed_bias=0,speed_last_bias=0,speed_previous_bias=0;
   int pwm_result;
	 double pwm_motor;
   int8 FLAG_TM4=0;
	 int FLAG_TM4_COUNT;
/*******/
int flag_turn=0;
int16 fk_turn=1;
int flag_key=1;
int f52=1;
char ch1[3]="kp",ch2[3]="ki",ch3[3]="kd";
void change_key()
{
	if(P07==0)
		{
			delay_ms(100);
			if(P07==0)
			{
				flag_key++;
				if(flag_key==4)
				{
					flag_key=1;
				}
				if(flag_key==1)
				{
					oled_p6x8str(0,1,ch1);
				}
				else if(flag_key==2)
				{
					oled_p6x8str(0,1,ch2);
				}
				else if(flag_key==3)
				{
					oled_p6x8str(0,1,ch3);
				}
			}
			while(P07==0);
		}
		if(P46==0)
		{
			delay_ms(100);
			if(P46==0)
			{
				if(flag_key==1)
				{
					kp-=0.0001;
				}
				else if(flag_key==2)
				{
					ki-=0.00001;
				}
				else if(flag_key==3)
				{
					kd+=0.0001;
				}
			}
			while(P46==0);
		}
		if(P45==0)
		{
			delay_ms(100);
			if(P45==0)
			{
				if(flag_key==1)
				{
					kp+=0.0001;
				}
				else if(flag_key==2)
				{
					ki+=0.00001;
				}
				else if(flag_key==3)
				{
					kd-=0.0001;
				}
			}
			while(P45==0);
		}
	/* if(P46==0)
	 {
			fk_turn=(-1)*fk_turn;
			flag_turn=0;
			f52=(-1)*f52;
		  if(f52==1)
				P52=1;
			if(f52==-1)
				P52=0;
			while(P46==0);
	 }*/
/*	 oled_p6x8str(0,2,ch1);
	 oled_p6x8str(0,4,ch2);
	 oled_p6x8str(0,6,ch3);
	 oled_printf_float(20,2,kp,1,3);
	 oled_printf_float(20,4,ki,1,5);
	 oled_printf_float(20,6,kd,1,4);
		*/
}
void FloatToByte(float floatNum,unsigned char* byteArry)//??????????2
{
		int i;
    char* pchar=(char*)&floatNum;
    for(i=0;i<sizeof(float);i++)
    {
		*byteArry=*pchar;
		pchar++;
		byteArry++;
	
    }
}
void output_uart(float number)
{
		int i;
		unsigned char floatToHex[4];
		FloatToByte(number,floatToHex);
		for(i=3;i>=0;i--)
		{
			uart_putchar(UART_1,floatToHex[i]);
		}
		
}
void output_uart_wireless(float number)
{
		int i;
		unsigned char floatToHex[4];
		FloatToByte(number,floatToHex);
		for(i=3;i>=0;i--)
			seekfree_wireless_send_buff(&floatToHex[i], 1);
}
void get_encoder()
{
			temp_pluse_real = ctimer_count_read(CTIM0_P34);
		if(FLAG_TM4%5==0) 
		{
			temp_pluse = temp_pluse_real;
			speed_bias = target - temp_pluse; 
	    //pwm_motor += kp_m*(speed_bias - speed_last_bias) + ki_m*speed_bias
	    pwm_motor+= kp_m*(speed_bias-speed_last_bias)+ki_m*speed_bias;
	    pwm_motor+= kd_m*(speed_bias-2*speed_last_bias-speed_previous_bias);
	    
			speed_previous_bias = speed_last_bias;
	    speed_last_bias = speed_bias;
	
	    if(pwm_motor>840)	pwm_motor = 840;// 最大限幅即启动速度
	    if(pwm_motor<500)	pwm_motor = 500;
	    pwm_result = (int)pwm_motor;//500~2000 //760时是40转/ms,target=220
	
			ctimer_count_clean(CTIM0_P34);
			//oled_printf_float(80,0,temp_pluse,3,1);
		}
}
int pid()   
{
	int MAX=24;//??????<50,????
	long IMAX=400000;
  now=(angle+angle_origin+except_angle)*kp+icm_gyro_x*kd+intergral*ki;
	intergral+=(angle+angle_origin+except_angle);
	if(intergral>IMAX) intergral=IMAX;
	if(intergral<-IMAX) intergral=-IMAX;
	
	now=(int)now;
  if(now<-MAX) return -MAX;
  else if(now>MAX) return MAX; 
  else return now;
}
void get_direction()
{
	if(temp_pluse>=20)
	   pwm_duty(PWMB_CH3_P33, 159+pid());//pid为正左，负右
	else 
		 pwm_duty(PWMB_CH3_P33,159);
}
void get_speed()
{
	get_encoder();
	pwm_duty(PWMA_CH1N_P11,pwm_result); //500~2000
	pwm_duty(PWMA_CH2N_P13,0);
	
	//if(angle>=2000||angle<=-2000)
	//{pwm_duty(PWMA_CH1N_P11,0);
	//pwm_duty(PWMA_CH2N_P13,0);}
	//pwm_duty(PWMA_CH1N_P11,760);
	//pwm_duty(PWMA_CH2N_P13,0);
} 
float angle_calc(float angle_m, float gyro_m)    
{    
   float temp_angle;               
   float gyro_now;    
   float error_angle;        
   static float last_angle;    
   static uint8 first_angle;           
    if(!first_angle)   
    {    
        first_angle = 1;    
        last_angle = angle_m;    
    }
		gyro_now = gyro_m * gyro_ratio;            
    error_angle = (angle_m - last_angle)*acc_ratio;        
    temp_angle = last_angle + (error_angle + gyro_now)*dt;   
    last_angle = temp_angle;		     
    return temp_angle;    
}    
void get_adc()
{
		adc_r = adc_once(ADC_P15, ADC_12BIT);	//右电感
		adc_l = adc_once(ADC_P00, ADC_12BIT);	//左电感
	  position= ((adc_r - adc_l)<<7)/(adc_r+adc_l);
	  except_angle=kp_a*position+kd_a*(last_position-position);
}
	int main(void)    
{                       
    DisableGlobalIRQ();    
	  board_init();
    delay_ms(300);
    oled_init();     
    icm20602_init_spi(); 
	  pwm_init(PWMB_CH3_P33,50,159);
	  //adc_init(ADC_P15, ADC_SYSclk_DIV_2);	//初始化ADC,P1.2通道 ，ADC时钟频率：SYSclk/2
	  //adc_init(ADC_P00, ADC_SYSclk_DIV_2);	//初始化ADC,P1.1通道 ，ADC时钟频率：SYSclk/2
	  ctimer_count_init(CTIM0_P34);
	  seekfree_wireless_init();
	  RTS_PIN = 0; //SEEKFREE_WIRELESS.h
	  delay_ms(80);
	  EnableGlobalIRQ();
   	pwm_init(PWMA_CH1N_P11,10000,0);
	  pwm_init(PWMA_CH2N_P13,10000,0);
	  //pit_timer_ms(TIM_3, 1);
	  pit_timer_ms(TIM_4, 1);	
     while (1)
{ 
	    //oled_printf_float(1,2,intergral,8,1);
	 oled_p6x8str(0,2,ch1);
	 oled_p6x8str(0,4,ch2);
	 oled_p6x8str(0,6,ch3);
	 oled_printf_float(20,2,kp,1,3);
	 oled_printf_float(20,4,ki,1,5);
	 oled_printf_float(20,6,kd,1,4);
			//send_to_PC[0]=pwm_result;
			//send_to_PC[1]=kp_m*(speed_bias-speed_last_bias);
			//send_to_PC[2]=ki_m*speed_bias;
	    //send_to_PC[3]=kd_m*(speed_bias-2*speed_last_bias-speed_previous_bias);
	
	    send_to_PC[0]=(angle+angle_origin+except_angle)*kp;
	    send_to_PC[1]=pid();    
	    send_to_PC[2]=icm_gyro_x*kd;
	    send_to_PC[3]=intergral*ki;
	    //send_to_PC[0]=(angle+angle_origin)/100;
			//send_to_PC[1]=icm_acc_y;
	    //send_to_PC[3]=-icm_gyro_x;
			
			output_uart_wireless(send_to_PC[0]);output_uart_wireless(send_to_PC[1]);
			output_uart_wireless(send_to_PC[2]);output_uart_wireless(send_to_PC[3]);
			seekfree_wireless_send_buff(tail,4);
			delay_ms(5);
    }     
}
void TM4_Isr() interrupt 20
{
	 FLAG_TM4++;
	 if(FLAG_TM4==120) FLAG_TM4_COUNT++;
	 if(FLAG_TM4_COUNT>=10&&FLAG_TM4_COUNT<=17) except_angle=EX;
	 //if(FLAG_TM4_COUNT>=18) except_angle=0;
	 //if(FLAG_TM4_COUNT>=8&&FLAG_TM4_COUNT>=16) except_angle=100;
	 change_key();
	 get_icm20602_accdata_spi();       
   get_icm20602_gyro_spi(); 	
   icm_gyro_x+=18;
	 if(abs(icm_gyro_x-icm_gyro_x_last)>5000)
		 icm_gyro_x=icm_gyro_x_last;
	 icm_gyro_x_last=icm_gyro_x;
	 icm_acc_y+=0;
   angle = angle_calc(icm_acc_y, -icm_gyro_x)+160;//左正右负
	 //get_adc();
	 get_direction();
	 get_speed();
	 if(FLAG_TM4==120) FLAG_TM4=0;
	 TIM4_CLEAR_FLAG; 
}