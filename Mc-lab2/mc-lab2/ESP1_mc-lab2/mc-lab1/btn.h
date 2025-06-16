#ifndef BTN_H
#define BTN_H

#include <Arduino.h>

#define BUTTON_PIN 14 // GPIO14 (D5)

// Function to detect double click
bool checkButton() {
    static int clickCount = 0;
    static unsigned long firstClickTime = 0;
    static bool buttonState = false;
    const int debounceDelay = 50;
    const int doubleClickThreshold = 500;

    bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;

    if (buttonPressed && !buttonState && millis() - firstClickTime > debounceDelay) {
        clickCount++;
        if (clickCount == 1) {
            firstClickTime = millis();
        } else if (clickCount == 2 && millis() - firstClickTime < doubleClickThreshold) {
            clickCount = 0;
            return true;
        }
    }

    if (!buttonPressed && buttonState) {
        buttonState = false;
    } else {
        buttonState = buttonPressed;
    }

    if (clickCount == 1 && millis() - firstClickTime > doubleClickThreshold) {
        clickCount = 0;
    }

    return false;
}

#endif
