
extern volatile float current_power_volt;                              // Текущее напряжение регулятора
extern volatile float target_power_volt;                               // Заданное напряжение регулятора
extern String current_power_mode;                                      // Режим работы регулятора напряжения
extern volatile bool PowerOn;
extern volatile float target_power_percent;

const int stepsPerRevolution = 2048;

SmoothStepper smoothStepper(stepsPerRevolution, 18, 19, 23, 25);

static unsigned long currentTime;
static unsigned long windowStartTime;

uint32_t windowSize = 2400;
static int output;

void IRAM_ATTR user_trigger_power_status(void *parameter) {
  while (true) {
    if (PowerOn) {
      target_power_percent = current_power_volt = target_power_volt;
    }
    vTaskDelay(1000);
  }
  
}
void IRAM_ATTR user_set_current_power(float Volt) {
  Serial.print("user_set_current_power ");
  Serial.println(Volt);
  target_power_percent = Volt;
  output = (int)(Volt * 10.0);
}
void IRAM_ATTR user_set_power_mode(String Mode) {
  current_power_mode = Mode;
  Serial.print("user_set_power_mode ");
  Serial.println(Mode);

  if(current_power_mode == POWER_SLEEP_MODE) {
    
  }
  else if(current_power_mode == POWER_SPEED_MODE) {
    
  }
  
}


void user_servo_init(void){
  if (!smoothStepper.accelerationEnable(3, 15, 500)) {
      Serial.println("Non correct parameter(s)");
      while (1) {
      }
  }
  smoothStepper.begin();
  //Go to the absolute position 0.
  smoothStepper.absolutePosition(-1024);
  smoothStepper.waitUntilArrived();
  smoothStepper.setOrigin();
}
void IRAM_ATTR user_set_capacity(byte cap){

}


void IRAM_ATTR user_setup(void) {
  
}

void IRAM_ATTR user_loop(void) {
  if(current_power_mode == POWER_WORK_MODE) {

    currentTime = millis();

    if ((currentTime - windowStartTime) > windowSize)
    {
      // Time to shift the Relay Window
      windowStartTime += windowSize;
    }
    if (output > (currentTime - windowStartTime)) digitalWrite(ssrPin, HIGH);
    else digitalWrite(ssrPin, LOW);

  }
}
