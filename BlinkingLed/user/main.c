#include "LIB_Config.h"


int main(void) 
{
	system_init();
	
	ssd1306_clear_screen(0xFF);
	delay_ms(1000);
	ssd1306_clear_screen(0x00);
	ssd1306_display_string(18, 0, "1.3inch OLED", 16, 1);
	ssd1306_display_string(0, 16, "This is a demo for SSD1306/1106 OLED moudle!", 16, 1);
	ssd1306_refresh_gram();
	delay_ms(1000);
	ssd1306_clear_screen(0x00);

	ssd1306_draw_bitmap(0, 2, &c_chSingal816[0], 16, 8);
	ssd1306_draw_bitmap(24, 2, &c_chBluetooth88[0], 8, 8);
	ssd1306_draw_bitmap(40, 2, &c_chMsg816[0], 16, 8);
	ssd1306_draw_bitmap(64, 2, &c_chGPRS88[0], 8, 8);
	ssd1306_draw_bitmap(90, 2, &c_chAlarm88[0], 8, 8);
	ssd1306_draw_bitmap(112, 2, &c_chBat816[0], 16, 8);
	
	ssd1306_draw_3216char(0,16, '2');
	ssd1306_draw_3216char(16,16, '3');
	ssd1306_draw_3216char(32,16, ':');
	ssd1306_draw_3216char(48,16, '5');
	ssd1306_draw_3216char(64,16, '6');
	ssd1306_draw_1616char(80,32, ':');
	ssd1306_draw_1616char(96,32, '4');
	ssd1306_draw_1616char(112,32, '7');
	ssd1306_draw_bitmap(87, 16, &c_chBmp4016[0], 40, 16);

	ssd1306_display_string(0, 52, "MUSIC", 12, 0);
	ssd1306_display_string(52, 52, "MENU", 12, 0);
	ssd1306_display_string(98, 52, "PHONE", 12, 0);
	
	ssd1306_refresh_gram();
	while (1) {

	}
}

/*-------------------------------END OF FILE-------------------------------*/

