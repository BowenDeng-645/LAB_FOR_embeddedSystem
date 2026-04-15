	#include <platform.h>
	#include <gpio.h>
	#include <stdio.h>
	#include "delay.h"
	#include "lcd.h"

	typedef enum 
	{
			BUTTON_UP,
			BUTTON_DOWN
	} ButtonState;

	ButtonState getButtonState() 
	{
			return (gpio_get(PA_0) == 0) ? BUTTON_DOWN : BUTTON_UP;
	}
	void main_sample(void) 
	{
		// initialisation
		lcd_init();	
		lcd_clear();
		// study lcd.h, gpio.h, delay.h for related driver functions
		while(1) 
		{      
			lcd_clear();			
			lcd_print("QMUL-BUPT JP/JEIWelcome!");
			delay_ms(1500);

			lcd_clear();
			lcd_print("EBU5477\nEmbedded Systems");
			delay_ms(1500);
		}
	}

	void main1(void) 
	{
		// Exercise 1: Display name and student IDs alternately every 3 seconds
		lcd_init();
		lcd_clear();
		while(1)
		{
			// Show first name on line 1, last name on line 2
			lcd_clear();
			lcd_print("Bowen\nDeng");
			delay_ms(5000);

			// Show BUPT ID on line 1, QMUL ID on line 2
			lcd_clear();
			lcd_print("2023213645\n231223449");
			delay_ms(3000);
		}
	}

//*****************************
//*****************************

	void main2(void) 
	{
		// Exercise 2: Count RIGHT button presses and display on LCD
		int press_count = 0;
		char buf[16];

		lcd_init();
		lcd_clear();
		gpio_set_mode(PA_0, PullUp);

		lcd_print("Press count:");
		lcd_set_cursor(0, 1);
		lcd_print("0");

		while(1)
		{
			// Wait for button press (low = pressed)
			//低电平有效，按下时为0，松开时为1
			if (gpio_get(PA_0) == 0)
			{
				press_count++;
				sprintf(buf, "%d               ", press_count);
				lcd_set_cursor(0, 1);
				lcd_print(buf);
				// Debounce: wait for button release
				//实现简单软件消抖，等待按钮释放后再继续
				while (gpio_get(PA_0) == 0);//阻塞等待直至按钮释放
				delay_ms(50);//等待机械抖动彻底稳定
			}
		}
	}

//*****************************
	void main3(void)
	{
		// Exercise 3: Toggle display between name and IDs on RIGHT button press
		int show_name = 1;  // 1 = show name, 0 = show IDs

		lcd_init();
		lcd_clear();
		gpio_set_mode(PA_0, PullUp);

		// Initially show name
		lcd_print("Bowen\nDeng");

		while(1)
		{
			if (gpio_get(PA_0) == 0)
			{
				// Wait for button release + debounce
				while (gpio_get(PA_0) == 0);
				delay_ms(50);

				show_name = !show_name;
				lcd_clear();
				if (show_name)
				{
					lcd_print("Bowen\nDeng");
				}
				else
				{
					lcd_print("2023213645\n231223449");
				}
			}
		}
	}

	int main(void){
			//main_sample();
			main1();
			//main2();
			//main3();
			//return 0;
	}

	// *********** ARM University Program Copyright   ARM Ltd 2016 ************************************   
	// *********** Queen Mary University of London Copyright Dr Matthew Tang 2024 *********************
