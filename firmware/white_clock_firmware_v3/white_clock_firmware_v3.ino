#define DISP_BR_MAX 64
#define DISP_BR_MIN 1
#define DISP_BR_BUFFER_LENGTH 10
#define DISP_BR_BUFFER_TIME 2000

#define DISP_LENGTH 4
#define DISP_PIN_INIT PB2


// Library imports
#include <Thread.h>
#include <ThreadController.h>

// Threads
ThreadController cpu = ThreadController();
Thread thr_display = Thread();
Thread thr_ldr = Thread();
Thread thr_rtc = Thread();
Thread thr_dht = Thread();
Thread thr_panel = Thread();


byte seven_seg_numbers [10] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100100, // 7
  0b11111110, // 8
  0b11110110  // 9
};

// Display
byte disp_content [DISP_BR_BUFFER_LENGTH];

int disp_digit = 0;

int disp_brightness = 0;
int disp_brightness_buffer[DISP_BR_BUFFER_LENGTH];

int disp_count = 0; 

void setup() {

  // PinMode display digits pins
  DDRD = 0xff;                    // Set all PortD pins to output (0 - 7)
  PORTD = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  DDRB = B00111100;               // Set pins 10, 11, 12, 13 to output and others to input
  PORTB = 0x00;                   // Set all PortB pins to LOW

  /*    *    *    THREADS    *    *    */

  // Initialize threads
  thr_display.onRun(thr_display_func);
  thr_display.setInterval(1000);

  thr_ldr.onRun(thr_ldr_func);
  thr_ldr.setInterval(DISP_BR_BUFFER_TIME / DISP_BR_BUFFER_LENGTH);

  //thr_rtc.onRun();
  //thr_rtc.setInterval();

  //thr_dht.onRun();
  //thr_dht.setInterval();

  //thr_panel.onRun(thread_panel_loop);
  //thr_panel.setInterval(1000);


  // Add thread controll
  cpu.add(&thr_display);
  cpu.add(&thr_ldr);

  /*    *    ATMEGA TIMER1    *    */

  // Modo de Comparação
  TCCR1A = 0;

  // Prescaler 1:256
  TCCR1B |=  (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);

  // Inicializa Registradores
  TCNT1 = 0;
  OCR1A = 2;

  // Habilita Interrupção do Timer1
  TIMSK1 = (1 << OCIE1A);

  /*    *     END OF BOOT     *    */

  disp_content[0] = 0b01111010; // d
  disp_content[1] = 0b11101110; // a
  disp_content[2] = 0b01111100; // v
  disp_content[3] = 0b00001100; // i
}

void loop() {
  cpu.run();

  // DEBUG
  int toshow = disp_brightness;

  disp_content[0] = ((toshow / 1000) % 10) > 0 || toshow >= 10000 ? seven_seg_numbers[(toshow / 1000) % 10] : 0x00;
  disp_content[1] = ((toshow / 100) % 10) > 0 || toshow >= 1000 ? seven_seg_numbers[(toshow / 100) % 10] : 0x00;
  disp_content[2] = ((toshow / 10) % 10) > 0 || toshow >= 100 ? seven_seg_numbers[(toshow / 10) % 10] : 0x00;;
  disp_content[3] = seven_seg_numbers[(toshow       ) % 10];
}

/*    *    *    *    THREAD    *    *    *    */
void thr_display_func() {
  
}

void thr_ldr_func(){
  int br_read = map(analogRead(A2), 50, 800, DISP_BR_MIN, DISP_BR_MAX);

  // Avoid out of range problems
  if(br_read > DISP_BR_MAX) br_read = DISP_BR_MAX; else
  if(br_read < DISP_BR_MIN) br_read = DISP_BR_MIN;

  // Avoid first read problems
  //if(disp_br_average_calc == 0)
  //  for(int i = 0; i < DISP_BR_BUFFER_LENGTH; i++)
  //    disp_brightness_buffer[i] = br_read;

  // Move the buffer to side
  for(int i = DISP_BR_BUFFER_LENGTH - 2; i >= 0; i--)
      disp_brightness_buffer[i + 1] = disp_brightness_buffer[i];
      
  // Insert the new value on buffer
  disp_brightness_buffer[0] = br_read;

  disp_brightness = disp_br_average_calc();
}

int disp_br_average_calc(){
  int sum = 0;

  for(int i = 0; i < DISP_BR_BUFFER_LENGTH; i++)
    sum += disp_brightness_buffer[i];

  return sum / DISP_BR_BUFFER_LENGTH;
}

/*    *    *    *    TIMER1    *    *    *    */
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;

  // Clean display
  PORTD = B11111111;                            // Sets all PORTD pins to HIGH

  // 
  if((disp_count/DISP_LENGTH) < disp_brightness){
    PORTB |= 0b00111100;                          // Puts pins PB2, PB3, PB4, PB5 to HIGH
    PORTB ^= 0b00111100;                          // Toggle pins PB2, PB3, PB4, PB5 to LOW
  
    // Active the current digit of display
    PORTB ^= (1 << DISP_PIN_INIT + disp_digit);   // Toggle the current digit pin to HIGH
  
    // Set display content
    PORTD ^= disp_content[disp_digit];            // Sets the display content and uses ^= to invert the bits
                                                  // (the display actives with LOW state)
  }

  // Increment the current digit register
  disp_digit++;
  disp_digit %= DISP_LENGTH;

  // Increment the current brightness loop counter
  disp_count++;
  disp_count %= DISP_BR_MAX * DISP_LENGTH;
}
