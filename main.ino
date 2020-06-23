
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>

#define LINE_LEN 16
#define ROWS 2
#define RUNNING_TEXT "Running..."
#define NOT_RUNNING_TEXT "Set time (mm:ss)"
#define BUTTON_HOLD_TIME 250
#define MOTOR_CONTROL_PIN PIN2
#define STEP_SECS 15

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
int chosenTimeSecs = 3 * 60;
unsigned long endTimeMs = 0;
bool isRunning = false;
uint8_t motorState = LOW;
Button buttonPressedLastLoop = Button::NONE;
unsigned long buttonPressedLastLoopTimeMs = 0;

void setup()
{
  lcd.begin(LINE_LEN, ROWS);
  pinMode(MOTOR_CONTROL_PIN, OUTPUT);

  digitalWrite(MOTOR_CONTROL_PIN, LOW);
}

void loop()
{
  Button buttonPressed = getButtonPressed();

  bool isButtonHeld = false;
  if (buttonPressed && buttonPressedLastLoop == buttonPressed)
  {
    if (((long)millis() - (long)buttonPressedLastLoopTimeMs) > BUTTON_HOLD_TIME)
    {
      isButtonHeld = true;
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
    if (!isRunning && chosenTimeSecs < 99 * 60)
    {
      chosenTimeSecs += STEP_SECS;
    }

    break;

  case Button::DOWN:
    if (!isRunning && chosenTimeSecs > STEP_SECS)
    {
      chosenTimeSecs -= STEP_SECS;
    }
    break;

  case Button::SELECT:
    if (!isButtonHeld)
    {
      if (!isRunning)
      {
        endTimeMs = millis() + ((unsigned long)chosenTimeSecs * 1000);
      }

      isRunning = !isRunning;
    }
    break;

  default:
    // No button was pressed, do nothing
    break;
  }

  bool endTimeReached = endTimeMs && ((long)endTimeMs - (long)millis()) <= 0;
  if (endTimeReached)
  {
    isRunning = false;
    endTimeMs = 0;
  }

  draw();

  buttonPressedLastLoop = buttonPressed;
  buttonPressedLastLoopTimeMs = millis();

  if (motorState != isRunning)
  {
    motorState = isRunning;
    digitalWrite(MOTOR_CONTROL_PIN, motorState);
  }
}

void draw()
{
  lcd.setCursor(0, 0);
  char textToShow[textLen];
  snprintf(textToShow, textLen, "%-16s", isRunning ? RUNNING_TEXT : NOT_RUNNING_TEXT);
  lcd.print(textToShow);

  lcd.setCursor(0, 1);
  int timeToShowSecs = isRunning ? ((endTimeMs - millis()) / 1000) + 1 : chosenTimeSecs;
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
