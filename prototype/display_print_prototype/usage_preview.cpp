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
				disp_print('H');
				disp_printEnd((int) dht_hum_buffer.getAverage());
			}

			// Hours and Minutes
			else {
				disp_clear();
				disp_setCursor(0);
				disp_print("0000");

				disp_setCursor(rtc_now.hour() < 10 ? 1 : 0);
				disp_print(rtc_now.hour());

				disp_setCursor(rtc_now.minute() < 10 ? 2 : 3);
				disp_print(rtc_now.minute());
			}

			break;

		case MAIN_SCREEN_LDR:
			disp_clear();
			disp_setCursor(0);
			disp_print("BR");
			disp_printEnd((int) disp_brightness_buffer.getAverage());
			break;

        case MAIN_SCREEN_CHRONOMETER:
			disp_clear();
			disp_printEnd(chronometer_counter);
            break;

        default:
			disp_clear();
			disp_setCursor(0);
			disp_print("UNKOWN ERROR");
            break;
        }
}
