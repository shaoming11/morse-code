#include <Arduino.h>
#include <LiquidCrystal/LiquidCrystal.h>

// LCD pins (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Button pins
const int morseButton = 2;    // Button for morse code input
const int displayButton = 4;  // Button to display text

// Timing constants (in milliseconds)
const int dotThreshold = 200;     // Max time for a dot
const int dashThreshold = 600;    // Max time for a dash
const int letterGap = 1000;       // Gap between letters
const int debounceDelay = 50;     // Debounce delay

// Variables
String morseBuffer = "";          // Current morse pattern
String textBuffer = "";           // Decoded text buffer
String displayText = "";          // Text currently on display
unsigned long pressStartTime = 0;
unsigned long lastReleaseTime = 0;
bool buttonPressed = false;
bool lastMorseState = HIGH;
bool lastDisplayState = HIGH;
int cursorPosition = 0;

// Morse code lookup table
struct MorseCode {
  String pattern;
  char letter;
};

MorseCode morseTable[] = {
  {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, {".", 'E'},
  {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'}, {"..", 'I'}, {".---", 'J'},
  {"-.-", 'K'}, {".-..", 'L'}, {"--", 'M'}, {"-.", 'N'}, {"---", 'O'},
  {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'},
  {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, {"-.--", 'Y'},
  {"--..", 'Z'},
  {".----", '1'}, {"..---", '2'}, {"...--", '3'}, {"....-", '4'}, {".....", '5'},
  {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}, {"-----", '0'},
  {"", ' '}  // Space character (empty pattern)
};

const int morseTableSize = sizeof(morseTable) / sizeof(morseTable[0]);

void handleMorseInput();
void handleDisplayButton();
void checkLetterTimeout();
void processCurrentLetter();
char decodeMorse(String pattern);
void updateDisplay();
void displayCompleteText();

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Morse Decoder");
  lcd.setCursor(0, 1);
  lcd.print("Ready...");
  
  // Initialize buttons
  pinMode(morseButton, INPUT_PULLUP);
  pinMode(displayButton, INPUT_PULLUP);
  
  // Initialize serial for debugging
  Serial.begin(9600);
  Serial.println("Morse Code Generator Ready");
  Serial.println("Short press = dot, Long press = dash");
  Serial.println("Wait for letter gap to complete letter");
  
  delay(2000);
  lcd.clear();
  lcd.print("Pattern: ");
  lcd.setCursor(0, 1);
  lcd.print("Text: ");
}

void loop() {
  handleMorseInput();
  handleDisplayButton();
  checkLetterTimeout();
}

void handleMorseInput() {
  int morseState = digitalRead(morseButton);
  
  // Debounce
  static unsigned long lastDebounceTime = 0;
  if (morseState != lastMorseState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Button pressed (LOW because of INPUT_PULLUP)
    if (morseState == LOW && !buttonPressed) {
      buttonPressed = true;
      pressStartTime = millis();
      Serial.println("Button pressed");
    }
    
    // Button released
    if (morseState == HIGH && buttonPressed) {
      buttonPressed = false;
      unsigned long pressDuration = millis() - pressStartTime;
      lastReleaseTime = millis();
      
      // Determine if it's a dot or dash
      if (pressDuration < dotThreshold) {
        morseBuffer += ".";
        Serial.print("Dot added. Pattern: ");
      } else if (pressDuration < dashThreshold) {
        morseBuffer += "-";
        Serial.print("Dash added. Pattern: ");
      } else {
        // Very long press - treat as dash
        morseBuffer += "-";
        Serial.print("Long dash added. Pattern: ");
      }
      
      Serial.println(morseBuffer);
      updateDisplay();
    }
  }
  
  lastMorseState = morseState;
}

void handleDisplayButton() {
  int displayState = digitalRead(displayButton);
  
  // Button pressed (LOW because of INPUT_PULLUP)
  if (displayState == LOW && lastDisplayState == HIGH) {
    delay(50); // Simple debounce
    
    // Process any remaining morse pattern
    if (morseBuffer.length() > 0) {
      processCurrentLetter();
    }
    
    // Display the complete text
    displayCompleteText();
    Serial.println("Display button pressed - showing complete text");
  }
  
  lastDisplayState = displayState;
}

void checkLetterTimeout() {
  // If enough time has passed since last input, process the current letter
  if (morseBuffer.length() > 0 && (millis() - lastReleaseTime) > letterGap) {
    processCurrentLetter();
  }
}

void processCurrentLetter() {
  if (morseBuffer.length() == 0) return;
  
  char decodedChar = decodeMorse(morseBuffer);
  if (decodedChar != 0) {
    textBuffer += decodedChar;
    Serial.print("Decoded letter: ");
    Serial.print(decodedChar);
    Serial.print(" from pattern: ");
    Serial.println(morseBuffer);
  } else {
    Serial.print("Unknown pattern: ");
    Serial.println(morseBuffer);
    textBuffer += "?"; // Add question mark for unknown patterns
  }
  
  morseBuffer = "";
  updateDisplay();
}

char decodeMorse(String pattern) {
  for (int i = 0; i < morseTableSize; i++) {
    if (morseTable[i].pattern == pattern) {
      return morseTable[i].letter;
    }
  }
  return 0; // Not found
}

void updateDisplay() {
  lcd.clear();
  lcd.print("Pattern: ");
  lcd.print(morseBuffer);
  lcd.setCursor(0, 1);
  lcd.print("Text: ");
  
  // Show last few characters of text buffer
  String displayPortion = textBuffer;
  if (displayPortion.length() > 10) {
    displayPortion = displayPortion.substring(displayPortion.length() - 10);
  }
  lcd.print(displayPortion);
}

void displayCompleteText() {
  lcd.clear();
  
  if (textBuffer.length() == 0) {
    lcd.print("No text yet!");
    delay(1000);
    updateDisplay();
    return;
  }
  
  // If text fits on one line, display it simply
  if (textBuffer.length() <= 16) {
    lcd.print("Complete text:");
    lcd.setCursor(0, 1);
    lcd.print(textBuffer);
    delay(3000);
  } else {
    // Scroll through longer text
    lcd.print("Complete text:");
    lcd.setCursor(0, 1);
    
    for (int i = 0; i <= (int)textBuffer.length() - 16; i++) {
      lcd.setCursor(0, 1);
      lcd.print(textBuffer.substring(i, i + 16));
      delay(500);
    }
    delay(1000);
  }
  
  updateDisplay();
}