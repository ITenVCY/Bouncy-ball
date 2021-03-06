									   
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "stdlib.h" 


#define RB1	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	   
#define RB2	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)   //按键的定义
#define N 7

void Key_Init1(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Key_Init2(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}//按键IO口的初始化


/*
	读取按键
*/
uint8_t Key_Scan(void)
{
	uint8_t key_value=0 ; //按键电平逻辑情况
	
	if(RB1 == 0&&RB2 == 0){
		key_value = 1;
	}
	if(RB1 == 1&&RB2 == 0){
		key_value = 2;
	}
	if(RB1 == 0&&RB2 == 1){
		key_value = 3;
	}
	if(RB1 == 1&&RB2 == 1){
		key_value = 3;
	}

	return key_value;
}


/*
	延时程序	
*/	
static void Delay ( __IO uint32_t nCount )
{
	  for ( ; nCount != 0; nCount -- );
	
}

	int PAs[2][2]={{50,0xFFFFF},{30,0xF0000}};  //关卡难度区别  1:长度70  速度0xf0000   第二关长度50  	速度0xf000
	int COLOR[4]={0x07E0,0xFFE0,0x001F,0xFFFF}; //碰撞砖块颜色设置分别是   绿 黄 蓝 白     
	char dispBuff[100];	         //积分控制 
	static uint8_t score = 0;	 
	uint8_t key_temp;			//接受按键返回值  
	
	uint8_t x=10,y=29,i=1,j=1;  //球的坐标控制
    uint8_t x1=10,y1=30;		  //弹板的坐标控制
    uint8_t x2,y2,dx=20,dy=20,color; 
	 
	int flag=0;
	int temp;
	int q;
	int Ball[8][2];
	int c=1;
				//蓝   黄   绿   白	   白	  黄  蓝   绿
	int close[8]={31,65504,2016,65535,65535,65504,31,2016};
	int close1[8]={31,65504,2016,65535,65535,65504,31,2016}; 
	static uint8_t m=0,n=0;

	int r=5;  //弹球半径


int main(void)
{	

	ILI9341_Init ();         
	ILI9341_GramScan ( 6 );	 //设置液晶屏显示模式,6的模式显示屏是从左到右的刷新模式
	LCD_SetColors(RED,BLACK);  //背景颜色设置
	ILI9341_Clear(0,0,240,320);
	Key_Init1();
    Key_Init2();	 //初始化函数
	
	sprintf(dispBuff,"积分:%d",score); //把需要显示的打印到字符串数组中
 	ILI9341_DispStringLine_EN_CH(LINE(0),dispBuff);

	/*
		随机数的生成

	*/ 
	for(m=0,n=0;m<N-score;m++)
	{	srand(100-m);
	    x2=rand()%23;	   //随机生成的X坐标
		Ball[m][n++]=x2;
	    y2=rand()%16+4;	//随机数生成Y坐标
	   	Ball[m][n]=y2;
	    color=rand()%4;   //随机生成的颜色
	}


   while(1)
   {
		while (y != 30)
		{

		   for(m=0,n=0;m<N-score;m++)
			{	
				srand(100-m);
			    x2=Ball[m][n++];   
			    y2=Ball[m][n];
				LCD_SetTextColor(close[m]);
				ILI9341_DrawRectangle(x2*10,y2*10,dx,dy,1);
				n=0;
			}


			LCD_SetTextColor(WHITE);
	  		ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	//画弹板的矩形  颜色设置为白色

			key_temp = Key_Scan();	  				//读取按键输入信息
			if(key_temp==3)			   				//RB2:判断按键的情况
			{ 
				LCD_SetTextColor(BLACK);
	  	    	ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	
				x1++;
				LCD_SetTextColor(WHITE);
				if((x1*10+60)==240)  x1--;
	            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
			}
			if(key_temp==2)		   					//RB1:判断按键的情况
			{ 	
				LCD_SetTextColor(BLACK);
	  		    ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);						   
				x1--;
			    LCD_SetTextColor(WHITE);
				if(x1*10==0)  x1++;		  //弹板的边缘检测
	            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
			}

		    LCD_SetColors(WHITE,BLACK);
	    	sprintf(dispBuff,"积分:%d ",score); 
			ILI9341_DispStringLine_EN_CH(LINE(0),dispBuff);

			LCD_SetTextColor(WHITE);
		    ILI9341_DrawCircle(x*10,y*10,r,1);
			Delay(PAs[flag][1]);
			LCD_SetTextColor(BLACK);
 		    ILI9341_DrawCircle(x*10,y*10,r,1);

			 for(m=0,n=0;m<N-score;m++)
			{	q=0; 
				if(0 <= Ball[m][n]-x && (Ball[m][n]-x)*10 <= r )	   //左X面
				{	 
					if(Ball[m][n+1] <= y && y <= Ball[m][n+1]+2 )	
				        {
					 	 i*= -1;	
						 q=1;
						}	
				} 
				if(0 <= x-2-Ball[m][n] && (x-2-Ball[m][n])*10 <= r )	 //右X面
				{
					if(Ball[m][n+1] <= y && y <= Ball[m][n+1]+2)	
				        {
						 i*= -1;	
						 q=1;
						}	
				}
				if(0 <= y-Ball[m][n+1] && (y-Ball[m][n+1])*10 <= r )		//上Y面
				{
					if(Ball[m][n] <= x && x <= Ball[m][n]+2)
				    	{ 
						 j*= -1;
						 q=1;
						}	
				} 
				if(0 <= y-2-Ball[m][n+1] && (y-2-Ball[m][n+1])*10 <= r) //下Y面
				{
				if(Ball[m][n] <= y && y <= Ball[m][n]+2)
				         {
						  j*= -1;
						  q=1;
						 }	
				} 


				if(q == 1)
				{
				   ILI9341_Clear(0,0,240,320);
				   for(n=0;m<N-score;m++)
					{
						Ball[m][n]=Ball[m+1][n];
						Ball[m][n+1]=Ball[m+1][n+1];
						close[m]=close[m+1];
					}
					score++;
					break;
				}

        	} 
	
			if (x >=24 || x<=0 )
			{ 
				  i*= -1;
			}
			if (y<=0 || y==29 && x >= x1-1 && x <= x1+7) 
			{  
				  j*= -1;
			}		    

		  	if(y==29) 
			{
				  if(key_temp == 3 && i > 0 )			   			
				{ 	
			 	   	x = x+1;
				}

				 if(key_temp == 2 && i > 0)		   				
				{ 						   
				   x = x+1;
				} 

			
			}

			x+= i;
			y+= j;

			if(score == 7 ) 
			{
			 y=30;
			 ILI9341_Clear(0,0,240,320);    /* 刷新屏幕 */
			 break;
			}



	
		}

		if(flag == 1||score != 7)   break;


		LCD_ClearLine(LINE(0));
		LCD_SetColors(0xFFFF,BLACK);   
	    LCD_SetFont(&Font8x16);
	    ILI9341_DisplayStringEx(0,1*24,24,24,(uint8_t *)"按RB1进入下一关,RB2结束游戏",0);
		
	    key_temp = Key_Scan();

	     if(key_temp == 3)			   				
		{ 	
		      break;
		}

		if(key_temp == 2)		   				
		{ 						   
			ILI9341_Clear(0,0,240,320);    /* 刷新屏幕 */
			score = 0;
				for(m=0,n=0;m<N-score;m++)
			{	srand(100-m);
			    x2=rand()%23;	   //随机生成的X坐标
				Ball[m][n++]=x2;
			    y2=rand()%16+4;	//随机数生成Y坐标
			   	Ball[m][n]=y2;
			    color=rand()%4;   //随机生成的颜�

				close[m]=close1[m];
			}
			x = 10;
			y = 29;
			i = 1;
			j = 1;
		    x1 = 10;
		   	y1 = 30;
			flag++;

			LCD_SetTextColor(0xFFFF);
	  		ILI9341_DrawRectangle(x1*10,y1*10,PAs[0][0],15,1);
			ILI9341_DrawCircle(x*10,y*10,10,1);

			continue;  /*界面交互,用户判断是否进入下一关*/
				
		}
		 				
	}
	
		    ILI9341_Clear(0,0,240,320);    /* 刷新屏幕 */
		
            LCD_SetColors(WHITE,BLACK);   
		    LCD_SetFont(&Font8x16);
		    ILI9341_DisplayStringEx(0,1*24,24,24,(uint8_t *)"游戏结束",0);
			sprintf(dispBuff,"积分:%d",score);
		    ILI9341_DispStringLine_EN_CH(LINE(5),dispBuff);
//		   //1
//		    ILI9341_Clear(0,0,240,320);    /* 刷新屏幕 */
//			LCD_SetTextColor(WHITE);
//	  		ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	//画弹板的矩形  颜色设置为白色
//			while(1)
//			{
//				key_temp = Key_Scan();	  				//读取按键输入信息
//				if(key_temp==3)			   				//RB2:判断按键的情况
//				{ 
//					LCD_SetTextColor(BLACK);
//		  	    	ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	
//					x1++;
//					LCD_SetTextColor(WHITE);
//					if((x1*10+60)==240)  x1--;
//		            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
//				}
//				if(key_temp==2)		   					//RB1:判断按键的情况
//				{ 	
//					LCD_SetTextColor(BLACK);
//		  		    ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);						   
//					x1--;
//				    LCD_SetTextColor(WHITE);
//					if(x1*10==0)  x1++;		  //弹板的边缘检测
//		            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
//				}
//				Delay(0XFFFFF);
//			}
			

}	




