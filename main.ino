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

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

int chosen_time_mins = 5;
unsigned long end_time_ms = 0;
bool running = false;
Buttons button_pressed_last_loop = Buttons::NONE;

void loop()
{
  Buttons button_pressed = get_button_pressed();

  if (button_pressed && button_pressed_last_loop == button_pressed)
  {
    // TODO: Allow press & holding of UP & DOWN buttons
    draw();
    return;
  }

  switch (button_pressed)
  {
  case Buttons::UP:
    if (!running)
    {
      // Up
      ++chosen_time_mins;
      if (chosen_time_mins > 59)
      {
        chosen_time_mins = 59;
      }
    }

    break;

  case Buttons::DOWN:
    if (!running)
    {
      // Down
      --chosen_time_mins;
      if (chosen_time_mins < 1)
      {
        chosen_time_mins = 1;
      }
    }
    break;

  case Buttons::SELECT:
    if (running)
    {
      running = false;
    }
    else
    {
      end_time_ms = millis() + ((unsigned long)chosen_time_mins * 60 * 1000);
      running = true;
    }
    break;

  default:
    break;
  }

  if (end_time_ms && (end_time_ms - millis()) <= 0)
  {
    running = false;
    end_time_ms = 0;
  }

  draw();

  button_pressed_last_loop = button_pressed;
}

void draw()
{
  lcd.setCursor(0, 0);
  lcd.print(running ? "Running...      " : "Set time (mm:ss)");

  lcd.setCursor(0, 1);
  int time_to_show_secs = running ? ((end_time_ms - millis()) / 1000) : (chosen_time_mins * 60);
  char time[10];
  get_label(time, time_to_show_secs);
  lcd.print(time);
  //free(time);
}

char *get_label(char retval[10], int val_secs)
{
  //char retval[10]; // (char *)malloc(10);
  int mins = val_secs / 60;
  int secs = val_secs - mins * 60;
  snprintf(retval, 10, "%02d:%02d", mins, secs);
  //return retval;
}

Buttons get_button_pressed()
{
  int val = analogRead(A0);
  Buttons button_pressed = Buttons::NONE;
  if (val >= 0 && val <= 50)
  {
    button_pressed = Buttons::RIGHT;
  }
  else if (val >= 50 && val <= 150)
  {
    button_pressed = Buttons::UP;
  }
  else if (val >= 150 && val <= 300)
  {
    button_pressed = Buttons::DOWN;
  }
  else if (val >= 300 && val <= 500)
  {
    button_pressed = Buttons::LEFT;
  }
  else if (val >= 500 && val <= 750)
  {
    button_pressed = Buttons::SELECT;
  }

  return button_pressed;
}