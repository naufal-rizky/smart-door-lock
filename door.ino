#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <EEPROM.h>

const int ROW_NUM = 4;
const int COLUMN_NUM = 3;

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte pin_rows[ROW_NUM] = {2, 3, 5, 4};
byte pin_column[COLUMN_NUM] = {6, 7, 8};
byte pin_buzzer = 11;
byte pin_green_led = 9;
byte pin_red_led = 13;
byte buttonPin = 10; // Pin for the push button
Servo myServo;

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

char password[7] = "123456";
char enteredPassword[7] = "";
char configOldPassword[7] = "";
byte passwordIndex = 0;
byte wrongAttempts = 0;
bool configMode = false;
bool oldPasswordEntered = false;

void savePasswordToEEPROM()
{
  for (int i = 0; i < 6; ++i)
  {
    EEPROM.write(i, password[i]);
  }
  EEPROM.write(6, '\0'); // Null-terminate the stored password
}

void readPasswordFromEEPROM()
{
  for (int i = 0; i < 6; ++i)
  {
    password[i] = EEPROM.read(i);
  }
  password[6] = '\0'; // Null-terminate the password
}

void setup()
{
  pinMode(pin_buzzer, OUTPUT);
  pinMode(pin_green_led, OUTPUT);
  pinMode(pin_red_led, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Set the push button as input with an internal pull-up resistor
  myServo.attach(12);
  lcd.begin(16, 2);
  myServo.write(0);

  // Read password from EEPROM
  readPasswordFromEEPROM();

  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  Serial.begin(9600);
  digitalWrite(pin_red_led, HIGH);
}

void displayMenu()
{
  lcd.clear();
  lcd.print("Menu:");
  lcd.setCursor(0, 1);
  lcd.print("1. Ganti Password");
  lcd.setCursor(0, 2);
  lcd.print("2. Lihat Password");
}

void playToneForDigit(char digit)
{
  int frequency;

  switch (digit)
  {
  case '1':
    frequency = 261; // C4
    break;
  case '2':
    frequency = 293; // D4
    break;
  case '3':
    frequency = 329; // E4
    break;
  case '4':
    frequency = 349; // F4
    break;
  case '5':
    frequency = 392; // G4
    break;
  case '6':
    frequency = 440; // A4
    break;
  case '7':
    frequency = 493; // B4
    break;
  case '8':
    frequency = 523; // C5
    break;
  case '9':
    frequency = 587; // D5
    break;
  case '0':
    frequency = 659; // E5
    break;
  case '*':
    frequency = 698; // F5
    break;
  case '#':
    frequency = 784; // G5
    break;
  default:
    frequency = 0; // No tone for other characters
  }

  if (frequency > 0)
  {
    tone(pin_buzzer, frequency, 100);
    delay(100); // Add a short delay to distinguish tones
  }
}

void loop()
{
  char key = keypad.getKey();

  if (key)
  {
    Serial.println(key);
    tone(pin_buzzer, 1000, 100);

    if (configMode)
    {
      if (!oldPasswordEntered)
      {
        // Enter old password first
        if (key == '#')
        {
          // Check if entered old password is correct
          if (strcmp(enteredPassword, password) == 0)
          {
            lcd.clear();
            lcd.print("Old Password Benar!");
            delay(2000);
            lcd.clear();
            lcd.print("New Password:");
            lcd.setCursor(0, 1);
            lcd.print("                ");
            passwordIndex = 0;
            oldPasswordEntered = true;
          }
          else
          {
            lcd.clear();
            lcd.print("Old Password Salah!");
            digitalWrite(pin_red_led, HIGH);
            delay(1000);
            digitalWrite(pin_red_led, LOW);
            wrongAttempts++;

            if (wrongAttempts == 3)
            {
              tone(pin_buzzer, 200, 1500);
              delay(2000);
              wrongAttempts = 0;
            }
            else
            {
              tone(pin_buzzer, 200, 500);
            }

            delay(2000);
            lcd.clear();
            lcd.print("Password:");
            lcd.setCursor(0, 1);
            lcd.print("                ");
            passwordIndex = 0;
            oldPasswordEntered = false;
          }
        }
        else if (key == '*')
        {
          // Delete the last entered character
          if (passwordIndex > 0)
          {
            passwordIndex--;
            enteredPassword[passwordIndex] = '\0';
            lcd.setCursor(passwordIndex, 1);
            lcd.print(" ");
          }
        }
        else
        {
          if (passwordIndex < 6)
          {
            enteredPassword[passwordIndex] = key;
            enteredPassword[++passwordIndex] = '\0';
            lcd.setCursor(passwordIndex - 1, 1);
            lcd.print('*');
          }
        }
      }
      else
      {
        // Enter new password
        if (key == '#')
        {
          lcd.clear();
          lcd.print("Password changed!");
          delay(2000);
          lcd.clear();
          lcd.print("Password:");
          lcd.setCursor(0, 1);
          lcd.print("                ");
          passwordIndex = 0;
          oldPasswordEntered = false;
          configMode = false;
          savePasswordToEEPROM();
        }
        else
        {
          if (passwordIndex < 6)
          {
            password[passwordIndex] = key;
            password[++passwordIndex] = '\0';
            lcd.setCursor(passwordIndex - 1, 1);
            lcd.print('*');
          }
        }
      }
    }
    else
    {
      if (key == '#')
      {
        if (strcmp(enteredPassword, password) == 0)
        {
          digitalWrite(pin_red_led, LOW);
          lcd.clear();
          lcd.print("Password Benar!");
          digitalWrite(pin_green_led, HIGH);
          myServo.write(93);
          delay(5000);
          digitalWrite(pin_green_led, LOW);
          myServo.write(0);
          wrongAttempts = 0;
		      passwordIndex = 0;

          displayMenu();
        }
        else
        {
          lcd.clear();
          lcd.print("Password Salah!");
          digitalWrite(pin_red_led, HIGH);
          delay(1000);
          digitalWrite(pin_red_led, LOW);
          wrongAttempts++;

          if (wrongAttempts == 3)
          {
            tone(pin_buzzer, 200, 1500);
            delay(2000);
            wrongAttempts = 0;
          }
          else
          {
            tone(pin_buzzer, 200, 500);
          }
        }

        delay(5000);
        lcd.clear();
        lcd.print("Enter Password:");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        passwordIndex = 0;
      }
      else if (key == '0' && strcmp(enteredPassword, password) == 0)
      {
        // Enter password configuration mode if '*' is pressed
        configMode = true;
        lcd.clear();
        lcd.print("Old Password:");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        passwordIndex = 0;
        enteredPassword[passwordIndex] = '\0';
      }
      else if (key == '*' && strcmp(enteredPassword, password) == 0)
      {
        lcd.clear();
        lcd.print("Stored Password:");
        lcd.setCursor(0, 1);

        // Read password from EEPROM
        char storedPassword[7];
        for (int i = 0; i < 6; ++i)
        {
          storedPassword[i] = EEPROM.read(i);
          lcd.print(storedPassword[i]);
        }
        storedPassword[6] = '\0'; // Null-terminate the stored password

        delay(3000); // Display the stored password for 3 seconds
        passwordIndex = 0;
        configMode = false;
        lcd.clear();
        lcd.print("Password:");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        passwordIndex = 0;
      }
      else if (key == '*')
      {
        // Delete the last entered character
        if (passwordIndex > 0)
        {
          passwordIndex--;
          enteredPassword[passwordIndex] = '\0';
          lcd.setCursor(passwordIndex, 1);
          lcd.print(" ");
        }
      }
      else
      {
        if (passwordIndex < 6)
        {
          enteredPassword[passwordIndex] = key;
          enteredPassword[++passwordIndex] = '\0';
          lcd.setCursor(passwordIndex - 1, 1);
          lcd.print('*');
        }
      }
    }
  }
  // Check if the push button is pressed
  if (digitalRead(buttonPin) == LOW)
  {
    digitalWrite(pin_red_led, LOW);
    myServo.write(93);
    digitalWrite(pin_green_led, HIGH);
    tone(pin_buzzer, 421, 450);
    delay(5000); // Debounce delay
    myServo.write(0);
    digitalWrite(pin_green_led, LOW);
    while (digitalRead(buttonPin) == LOW)
    {
      // Wait for the button to be released
    }
  }
}
