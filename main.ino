/*
  Liquid Crystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The Liquid Crystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * Button pin to analog pin A0
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>

enum Buttons
{
  NONE = 0,
  RIGHT,
  UP,
  DOWN,
  LEFT,
  SELECT
};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define LINE_LEN 16
#define ROWS 2
#define RUNNING_TEXT "Running..."
#define NOT_RUNNING_TEXT "Set time (mm:ss)"
#define BUTTON_HOLD_TIME 250

int text_len = max(strlen(RUNNING_TEXT), strlen(NOT_RUNNING_TEXT)) + 1;

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(LINE_LEN, ROWS);
}

int chosen_time_mins = 5;
unsigned long end_time_ms = 0;
bool running = false;
Buttons button_pressed_last_loop = Buttons::NONE;
unsigned long button_pressed_last_loop_time_ms = 0;

void loop()
{
  Buttons button_pressed = get_button_pressed();

  bool button_held = false;
  if (button_pressed && button_pressed_last_loop == button_pressed)
  {
    if (((long)millis() - (long)button_pressed_last_loop_time_ms) > BUTTON_HOLD_TIME)
    {
      button_held = true;
    }
    else
    {
      draw();
      return;
    }
  }

  switch (button_pressed)
  {
  case Buttons::UP:
    if (!running)
    {
      ++chosen_time_mins;
      if (chosen_time_mins > 99)
      {
        chosen_time_mins = 99;
      }
    }

    break;

  case Buttons::DOWN:
    if (!running)
    {
      --chosen_time_mins;
      if (chosen_time_mins < 1)
      {
        chosen_time_mins = 1;
      }
    }
    break;

  case Buttons::SELECT:
    if (!button_held)
    {
      if (running)
      {
        running = false;
      }
      else
      {
        end_time_ms = millis() + ((unsigned long)chosen_time_mins * 60 * 1000);
        running = true;
      }
    }
    break;

  default:
    // No button was pressed, do nothing
    break;
  }

  if (end_time_ms && ((long)end_time_ms - (long)millis()) <= 0)
  {
    running = false;
    end_time_ms = 0;
  }

  draw();

  button_pressed_last_loop = button_pressed;
  button_pressed_last_loop_time_ms = millis();
}

void draw()
{
  lcd.setCursor(0, 0);
  char text_to_show[text_len];
  snprintf(text_to_show, text_len, "%-16s", running ? RUNNING_TEXT : NOT_RUNNING_TEXT);
  lcd.print(text_to_show);

  lcd.setCursor(0, 1);
  int time_to_show_secs = running ? ((end_time_ms - millis()) / 1000) : (chosen_time_mins * 60);
  char time[10];
  get_label(time, sizeof(time), max(time_to_show_secs, 0));
  lcd.print(time);
}

char *get_label(char *buf, size_t buf_size, int val_secs)
{
  int mins = val_secs / 60;
  int secs = val_secs - mins * 60;
  snprintf(buf, buf_size, "%02d:%02d", mins, secs);
}

Buttons get_button_pressed()
{
  int val = analogRead(A0);

  if (val >= 0 && val <= 50)
  {
    return Buttons::RIGHT;
  }
  else if (val >= 50 && val <= 150)
  {
    return Buttons::UP;
  }
  else if (val >= 150 && val <= 300)
  {
    return Buttons::DOWN;
  }
  else if (val >= 300 && val <= 500)
  {
    return Buttons::LEFT;
  }
  else if (val >= 500 && val <= 750)
  {
    return Buttons::SELECT;
  }

  return Buttons::NONE;
}