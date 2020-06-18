
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>

#define LINE_LEN 16
#define ROWS 2
#define RUNNING_TEXT "Running..."
#define NOT_RUNNING_TEXT "Set time (mm:ss)"
#define BUTTON_HOLD_TIME 250

enum Button
{
  NONE = 0,
  RIGHT,
  UP,
  DOWN,
  LEFT,
  SELECT
};

const int textLen = max(strlen(RUNNING_TEXT), strlen(NOT_RUNNING_TEXT)) + 1;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int chosenTimeMins = 5;
unsigned long endTimeMs = 0;
bool running = false;
Button buttonPressedLastLoop = Button::NONE;
unsigned long buttonPressedLastLoopTimeMs = 0;

void setup()
{
  lcd.begin(LINE_LEN, ROWS);
}

void loop()
{
  Button buttonPressed = getButtonPressed();

  bool buttonHeld = false;
  if (buttonPressed && buttonPressedLastLoop == buttonPressed)
  {
    if (((long)millis() - (long)buttonPressedLastLoopTimeMs) > BUTTON_HOLD_TIME)
    {
      buttonHeld = true;
    }
    else
    {
      draw();
      return;
    }
  }

  switch (buttonPressed)
  {
  case Button::UP:
    if (!running && chosenTimeMins < 99)
    {
      ++chosenTimeMins;
    }

    break;

  case Button::DOWN:
    if (!running && chosenTimeMins > 1)
    {
      --chosenTimeMins;
    }
    break;

  case Button::SELECT:
    if (!buttonHeld)
    {
      if (!running)
      {
        endTimeMs = millis() + ((unsigned long)chosenTimeMins * 60 * 1000);
      }

      running = !running;
    }
    break;

  default:
    // No button was pressed, do nothing
    break;
  }

  bool endTimeReached = endTimeMs && ((long)endTimeMs - (long)millis()) <= 0;
  if (endTimeReached)
  {
    running = false;
    endTimeMs = 0;
  }

  draw();

  buttonPressedLastLoop = buttonPressed;
  buttonPressedLastLoopTimeMs = millis();
}

void draw()
{
  lcd.setCursor(0, 0);
  char textToShow[textLen];
  snprintf(textToShow, textLen, "%-16s", running ? RUNNING_TEXT : NOT_RUNNING_TEXT);
  lcd.print(textToShow);

  lcd.setCursor(0, 1);
  int timeToShowSecs = running ? ((endTimeMs - millis()) / 1000) + 1 : (chosenTimeMins * 60);
  char time[10];
  getLabel(time, sizeof(time), max(timeToShowSecs, 0));
  lcd.print(time);
}

char *getLabel(char *buf, size_t bufSize, int valSecs)
{
  int mins = valSecs / 60;
  int secs = valSecs - mins * 60;
  snprintf(buf, bufSize, "%02d:%02d", mins, secs);
}

Button getButtonPressed()
{
  int val = analogRead(A0);

  if (val >= 0 && val <= 50)
  {
    return Button::RIGHT;
  }
  else if (val >= 50 && val <= 150)
  {
    return Button::UP;
  }
  else if (val >= 150 && val <= 300)
  {
    return Button::DOWN;
  }
  else if (val >= 300 && val <= 500)
  {
    return Button::LEFT;
  }
  else if (val >= 500 && val <= 750)
  {
    return Button::SELECT;
  }

  return Button::NONE;
}
