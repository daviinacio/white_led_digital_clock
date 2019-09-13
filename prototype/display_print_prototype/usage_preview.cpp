void thr_main_func(){
	switch(main_current_screen){
		case MAIN_SCREEN_HOME:
			// Splash screen
			if(m <= 2000){
				disp_clear();
				disp_setCursor(0);
				disp_print("DAVI");
			} else

			// Temperature
			if(m / 2000 % 10 = 8){
				disp_clear();
				disp_setCursor(0);
				disp_print((int) dht_temp_buffer.getAverage());
				disp_print("*C");
			} else

			// Humidity
			if(m / 2000 % 10 = 9){
				disp_clear();
				disp_setCursor(0);
				disp_print("H ");
				disp_print((int) dht_hum_buffer.getAverage());
			}

			// Hours and Minutes
			else {
				disp_clear();
				disp_setCursor(0);
				disp_print(rtc_now.hour());
				disp_print(rtc_now.minute());
			}

			break;

		case MAIN_SCREEN_LDR:
			disp_clear();
			disp_setCursor(0);
			disp_print("BR");
			disp_print(disp_brightness_buffer.getAverage());

			break;
	}
}
