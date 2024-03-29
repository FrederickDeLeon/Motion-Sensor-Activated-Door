/*** header block ***
 * code file name: Motion_Sensor_Activated_Door_1
 * code description: This program is loaded on the first Arduino board and it governs the PIR sensor and the 16x2 LCD's output
 * MCU board used: Arduino MEGA 2560 Microcontroller Development Board
 * programmer(s) name: Frederick De Leon
 * date when code is created/modified: 05/17/2022
 * code version: v1.0
 ***/

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void update_lcd(void);
void heartbeat(void);
void timers(void);

int is_door_closed(void);
int is_door_open(void);
int is_sensor_on(void);

void motor_drive_open(void);
void motor_drive_close(void);
void motor_stop(void);

//INPUTS
const int open_signal_pin = 8;
const int closed_signal_pin = 9;
const int sensor_pin = 10;
const int ch_A_pin = 8; //match to your project for encoder signals
const int ch_B_pin = 9; //match to your project for encoder signals

//OUTPUTS
const int LED_pin = 13;
const int open_output_pin = 6;
const int close_output_pin = 7;


int door_state = 0;

unsigned int motor_position = 0;

unsigned long int timer1;
unsigned long int timer2;
unsigned long int heartbeat_timer;
unsigned long int update_lcd_timer;


void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //INPUTS
  pinMode(sensor_pin, INPUT);
  pinMode(open_signal_pin, INPUT);
  pinMode(closed_signal_pin, INPUT);

  //OUTPUTS
  pinMode(LED_pin, OUTPUT);
  pinMode(open_output_pin, OUTPUT);
  pinMode(close_output_pin, OUTPUT);
}

void loop() //repeat
{
  door_control();
  update_lcd();
  heartbeat();
  timers();
  read_encoder();
}

void door_control(void) //controls door_state
{

  if (door_state == 0) //door is closed
  {
    motor_stop(); //motor stops moving
    if (is_sensor_on() == 1) //if motion sensor detects movement start opening door
      door_state = 1;

  }
  else if (door_state == 1) //door is opening
  {
    motor_drive_open(); //motor starts opening door
    if (is_door_open() == 1)
    {
      door_state = 2; //door is open
      timer1 = 0;
    }

  }
  else if (door_state == 2) //door is open
  {
    motor_stop();
    if (is_sensor_on() == 1) //if sensor is on, then timer is reset
      timer1 = 0;

    if (timer1 >= 50) //5 seconds
    {
      door_state = 3; //door is closing
      timer1 = 0;
    }
  }
  else if (door_state == 3) //door is closing
  {
    motor_drive_close(); //motor starts closing door
    if (is_sensor_on() == 1) //if sensor is on, then door starts opening
    {
      door_state = 1; //door is opening
      timer1 = 0; //timer1 reset
    }

    if (is_door_closed() == 1) //if door is closed
    {
      motor_position = 0; //resets motor position
      lcd.clear(); //clears lcd of old data so that "Enc=0" is visible
      motor_stop(); //stops motor
      door_state = 0; //door is closed
    }

  }
}

void motor_drive_open(void) //turns motor on to open door
{
  digitalWrite(open_output_pin, HIGH);
  digitalWrite(close_output_pin, LOW);
}
void motor_drive_close(void) //turns motor on to close door
{
  digitalWrite(open_output_pin, LOW);
  digitalWrite(close_output_pin, HIGH);
}
void motor_stop(void) //turns motor off
{
  digitalWrite(open_output_pin, LOW);
  digitalWrite(close_output_pin, LOW);
}


void update_lcd(void) //updates the data printed on the LCD, the Input from the motion sensor, the motor position, and the state of the door
{
  if (update_lcd_timer >= 1)
  {
    update_lcd_timer = 0; //resets LCD timer
    lcd.setCursor(0, 0);
    lcd.print("Input="); //input from motion sensor, 1 if motion is detected, 0 if no motion is detected
    lcd.print(is_sensor_on());
    lcd.setCursor(8, 0);
    lcd.print("Enc=");
    lcd.print(motor_position); //position of the motor, at >=30 door is open, at <=0 door is closed
    lcd.setCursor(0, 1);
    lcd.print("Door State=");
    lcd.print(door_state); //state of the door, 0 closed, 1 opening, 2 open, 3 closing

  }
}

void heartbeat(void)
{
  if (heartbeat_timer < 10)   //first 10 (10 times 100 ms = 1s)
    digitalWrite(LED_pin, HIGH);
  else
  {
    digitalWrite(LED_pin, LOW);
    if (heartbeat_timer >= 20) //(between 10 and 20 - another 1 s)
      heartbeat_timer = 0;  //When does the timer get cleared?
  }
}


int is_sensor_on(void) //returns 1 when motion sensor detects motion, else returns 0
{
  if (digitalRead(sensor_pin) == 1)
    return 1;
  else
    return 0;


}
int is_door_open(void) //returns 1 when door is open
{
  return (motor_position >= 30); //return when position of motor is greater than or equal to 30
}

int is_door_closed(void) //returns 1 when door is closed
{
  return (motor_position <= 0); //return when motor position less than or equal to 0
}

void timers(void) // controls timers
{
  static unsigned long ms_runtime = 0;
  static int one_ms_timer = 0;

  if (millis() > ms_runtime)
  {
    ms_runtime++;
    one_ms_timer++;
  }

  if (one_ms_timer > 99)
  { // our choice for 99 gives us increments of 100 ms
    timer1++;
    timer2++;
    heartbeat_timer++;
    update_lcd_timer++;
    one_ms_timer = 0;
  }
}


void read_encoder(void)
{
  static int encoder_case;
  int encoder_case_temp = 0;

  if (digitalRead(ch_A_pin))
    encoder_case_temp += 1;
  if (digitalRead(ch_B_pin))
    encoder_case_temp += 2;

  if (encoder_case != encoder_case_temp)
  {
    Serial.print("case = ");
    Serial.print(encoder_case_temp);
    Serial.print("  position = ");
    Serial.println(motor_position);
  }


  switch (encoder_case)
  {
    case 0:
      if (encoder_case_temp == 2)
        motor_position--;
      if (encoder_case_temp == 1)
        motor_position++;
      break;
    case 1:
      if (encoder_case_temp == 0)
        motor_position--;
      if (encoder_case_temp == 3)
        motor_position++;
      break;
    case 3:
      if (encoder_case_temp == 1)
        motor_position--;
      if (encoder_case_temp == 2)
        motor_position++;
      break;

    case 2:
      if (encoder_case_temp == 3)
        motor_position--;
      if (encoder_case_temp == 0)
        motor_position++;
      break;

  }

  if (encoder_case != encoder_case_temp)
  {
    Serial.print("case = ");
    Serial.print(encoder_case_temp);
    Serial.print("  position = ");
    Serial.println(motor_position);
  }

  encoder_case = encoder_case_temp;
}
