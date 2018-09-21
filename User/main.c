									   
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "stdlib.h" 


#define RB1	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	   
#define RB2	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)   //°´¼üµÄ¶¨Òå
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

}//°´¼üIO¿ÚµÄ³õÊ¼»¯


/*
	¶ÁÈ¡°´¼ü
*/
uint8_t Key_Scan(void)
{
	uint8_t key_value=0 ; //°´¼üµçÆ½Âß¼­Çé¿ö
	
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
	ÑÓÊ±³ÌÐò	
*/	
static void Delay ( __IO uint32_t nCount )
{
	  for ( ; nCount != 0; nCount -- );
	
}

	int PAs[2][2]={{50,0xFFFFF},{30,0xF0000}};  //¹Ø¿¨ÄÑ¶ÈÇø±ð  1:³¤¶È70  ËÙ¶È0xf0000   µÚ¶þ¹Ø³¤¶È50  	ËÙ¶È0xf000
	int COLOR[4]={0x07E0,0xFFE0,0x001F,0xFFFF}; //Åö×²×©¿éÑÕÉ«ÉèÖÃ·Ö±ðÊÇ   ÂÌ »Æ À¶ °×     
	char dispBuff[100];	         //»ý·Ö¿ØÖÆ 
	static uint8_t score = 0;	 
	uint8_t key_temp;			//½ÓÊÜ°´¼ü·µ»ØÖµ  
	
	uint8_t x=10,y=29,i=1,j=1;  //ÇòµÄ×ø±ê¿ØÖÆ
    uint8_t x1=10,y1=30;		  //µ¯°åµÄ×ø±ê¿ØÖÆ
    uint8_t x2,y2,dx=20,dy=20,color; 
	 
	int flag=0;
	int temp;
	int q;
	int Ball[8][2];
	int c=1;
				//À¶   »Æ   ÂÌ   °×	   °×	  »Æ  À¶   ÂÌ
	int close[8]={31,65504,2016,65535,65535,65504,31,2016};
	int close1[8]={31,65504,2016,65535,65535,65504,31,2016}; 
	static uint8_t m=0,n=0;

	int r=5;  //µ¯Çò°ë¾¶


int main(void)
{	

	ILI9341_Init ();         
	ILI9341_GramScan ( 6 );	 //ÉèÖÃÒº¾§ÆÁÏÔÊ¾Ä£Ê½,6µÄÄ£Ê½ÏÔÊ¾ÆÁÊÇ´Ó×óµ½ÓÒµÄË¢ÐÂÄ£Ê½
	LCD_SetColors(RED,BLACK);  //±³¾°ÑÕÉ«ÉèÖÃ
	ILI9341_Clear(0,0,240,320);
	Key_Init1();
    Key_Init2();	 //³õÊ¼»¯º¯Êý
	
	sprintf(dispBuff,"»ý·Ö:%d",score); //°ÑÐèÒªÏÔÊ¾µÄ´òÓ¡µ½×Ö·û´®Êý×éÖÐ
 	ILI9341_DispStringLine_EN_CH(LINE(0),dispBuff);

	/*
		Ëæ»úÊýµÄÉú³É

	*/ 
	for(m=0,n=0;m<N-score;m++)
	{	srand(100-m);
	    x2=rand()%23;	   //Ëæ»úÉú³ÉµÄX×ø±ê
		Ball[m][n++]=x2;
	    y2=rand()%16+4;	//Ëæ»úÊýÉú³ÉY×ø±ê
	   	Ball[m][n]=y2;
	    color=rand()%4;   //Ëæ»úÉú³ÉµÄÑÕÉ«
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
	  		ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	//»­µ¯°åµÄ¾ØÐÎ  ÑÕÉ«ÉèÖÃÎª°×É«

			key_temp = Key_Scan();	  				//¶ÁÈ¡°´¼üÊäÈëÐÅÏ¢
			if(key_temp==3)			   				//RB2:ÅÐ¶Ï°´¼üµÄÇé¿ö
			{ 
				LCD_SetTextColor(BLACK);
	  	    	ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	
				x1++;
				LCD_SetTextColor(WHITE);
				if((x1*10+60)==240)  x1--;
	            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
			}
			if(key_temp==2)		   					//RB1:ÅÐ¶Ï°´¼üµÄÇé¿ö
			{ 	
				LCD_SetTextColor(BLACK);
	  		    ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);						   
				x1--;
			    LCD_SetTextColor(WHITE);
				if(x1*10==0)  x1++;		  //µ¯°åµÄ±ßÔµ¼ì²â
	            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
			}

		    LCD_SetColors(WHITE,BLACK);
	    	sprintf(dispBuff,"»ý·Ö:%d ",score); 
			ILI9341_DispStringLine_EN_CH(LINE(0),dispBuff);

			LCD_SetTextColor(WHITE);
		    ILI9341_DrawCircle(x*10,y*10,r,1);
			Delay(PAs[flag][1]);
			LCD_SetTextColor(BLACK);
 		    ILI9341_DrawCircle(x*10,y*10,r,1);

			 for(m=0,n=0;m<N-score;m++)
			{	q=0; 
				if(0 <= Ball[m][n]-x && (Ball[m][n]-x)*10 <= r )	   //×óXÃæ
				{	 
					if(Ball[m][n+1] <= y && y <= Ball[m][n+1]+2 )	
				        {
					 	 i*= -1;	
						 q=1;
						}	
				} 
				if(0 <= x-2-Ball[m][n] && (x-2-Ball[m][n])*10 <= r )	 //ÓÒXÃæ
				{
					if(Ball[m][n+1] <= y && y <= Ball[m][n+1]+2)	
				        {
						 i*= -1;	
						 q=1;
						}	
				}
				if(0 <= y-Ball[m][n+1] && (y-Ball[m][n+1])*10 <= r )		//ÉÏYÃæ
				{
					if(Ball[m][n] <= x && x <= Ball[m][n]+2)
				    	{ 
						 j*= -1;
						 q=1;
						}	
				} 
				if(0 <= y-2-Ball[m][n+1] && (y-2-Ball[m][n+1])*10 <= r) //ÏÂYÃæ
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
			 ILI9341_Clear(0,0,240,320);    /* Ë¢ÐÂÆÁÄ» */
			 break;
			}



	
		}

		if(flag == 1||score != 7)   break;


		LCD_ClearLine(LINE(0));
		LCD_SetColors(0xFFFF,BLACK);   
	    LCD_SetFont(&Font8x16);
	    ILI9341_DisplayStringEx(0,1*24,24,24,(uint8_t *)"°´RB1½øÈëÏÂÒ»¹Ø,RB2½áÊøÓÎÏ·",0);
		
	    key_temp = Key_Scan();

	     if(key_temp == 3)			   				
		{ 	
		      break;
		}

		if(key_temp == 2)		   				
		{ 						   
			ILI9341_Clear(0,0,240,320);    /* Ë¢ÐÂÆÁÄ» */
			score = 0;
				for(m=0,n=0;m<N-score;m++)
			{	srand(100-m);
			    x2=rand()%23;	   //Ëæ»úÉú³ÉµÄX×ø±ê
				Ball[m][n++]=x2;
			    y2=rand()%16+4;	//Ëæ»úÊýÉú³ÉY×ø±ê
			   	Ball[m][n]=y2;
			    color=rand()%4;   //Ëæ»úÉú³ÉµÄÑÕ«

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

			continue;  /*½çÃæ½»»¥,ÓÃ»§ÅÐ¶ÏÊÇ·ñ½øÈëÏÂÒ»¹Ø*/
				
		}
		 				
	}
	
		    ILI9341_Clear(0,0,240,320);    /* Ë¢ÐÂÆÁÄ» */
		
            LCD_SetColors(WHITE,BLACK);   
		    LCD_SetFont(&Font8x16);
		    ILI9341_DisplayStringEx(0,1*24,24,24,(uint8_t *)"ÓÎÏ·½áÊø",0);
			sprintf(dispBuff,"»ý·Ö:%d",score);
		    ILI9341_DispStringLine_EN_CH(LINE(5),dispBuff);
//		   //1
//		    ILI9341_Clear(0,0,240,320);    /* Ë¢ÐÂÆÁÄ» */
//			LCD_SetTextColor(WHITE);
//	  		ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	//»­µ¯°åµÄ¾ØÐÎ  ÑÕÉ«ÉèÖÃÎª°×É«
//			while(1)
//			{
//				key_temp = Key_Scan();	  				//¶ÁÈ¡°´¼üÊäÈëÐÅÏ¢
//				if(key_temp==3)			   				//RB2:ÅÐ¶Ï°´¼üµÄÇé¿ö
//				{ 
//					LCD_SetTextColor(BLACK);
//		  	    	ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);	
//					x1++;
//					LCD_SetTextColor(WHITE);
//					if((x1*10+60)==240)  x1--;
//		            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
//				}
//				if(key_temp==2)		   					//RB1:ÅÐ¶Ï°´¼üµÄÇé¿ö
//				{ 	
//					LCD_SetTextColor(BLACK);
//		  		    ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);						   
//					x1--;
//				    LCD_SetTextColor(WHITE);
//					if(x1*10==0)  x1++;		  //µ¯°åµÄ±ßÔµ¼ì²â
//		            ILI9341_DrawRectangle(x1*10,y1*10,PAs[flag][0],10,1);
//				}
//				Delay(0XFFFFF);
//			}
			

}	




