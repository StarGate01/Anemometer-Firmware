#include <TM1637.h>

#define TWOPI 6.28318530718
#define RADIUS 0.18

#define PIN_IRQ 2
#define PIN_LCD_DIO 7
#define PIN_LCD_CLK 6

int8_t lcd_text[] = {0x00,0x00,0x00,0x00};
TM1637 tm1637(PIN_LCD_CLK, PIN_LCD_DIO);

static volatile unsigned long interrupt_time_delta = 0;
static volatile unsigned long last_interrupt_time = 0;

void setup() {
  tm1637.set();
  tm1637.init();
  tm1637.point(POINT_ON);
  tm1637.display(lcd_text);
  attachInterrupt(digitalPinToInterrupt(PIN_IRQ), interrupt_handler, RISING);
}

void interrupt_handler() {
  unsigned long interrupt_time = millis();
  unsigned long interrupt_time_delta_tmp = interrupt_time - last_interrupt_time;
  if (interrupt_time_delta_tmp > 100) interrupt_time_delta = interrupt_time_delta_tmp;
  last_interrupt_time = interrupt_time;
}

unsigned long delta_time = 0;
double outer_speed = 0.0;
unsigned int outer_speed_whole = 0;
unsigned int outer_speed_hundredths = 0;

void loop() {
  delay(100);

  cli();
  delta_time = interrupt_time_delta;
  sei();
  
  if(delta_time != 0) {
    outer_speed = (TWOPI / (((double)delta_time) / 1000.0)) * RADIUS * 3.6;
  } else {
    outer_speed = 0.0;
  }
  outer_speed = round(outer_speed * 100.0) / 100.0;
  outer_speed_whole = (int)outer_speed;
  outer_speed_hundredths = (int)((outer_speed - (double)outer_speed_whole) * 100.0);
  
  lcd_text[0] = outer_speed_whole / 10;
  lcd_text[1] = outer_speed_whole % 10;
  lcd_text[2] = outer_speed_hundredths / 10;
  lcd_text[3] = outer_speed_hundredths % 10;
  tm1637.display(lcd_text);
}
