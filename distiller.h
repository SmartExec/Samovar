void distiller_proc(){

  if (!PowerOn){
    set_power(true);
  }
  
  float c_temp; //текущая температура в кубе с учетом корректировки давления или без
  c_temp = get_temp_by_pressure(SteamSensor.Start_Pressure, TankSensor.BodyTemp, bme_pressure);
  
  if (c_temp > SamSetup.DistTemp) {
    //Выключаем нагрев куба
    set_power(false);
    reset_sensor_counter();
  }

}

void IRAM_ATTR check_alarm_distiller() {
  //сбросим паузу события безопасности
  if (alarm_t_min > 0 && alarm_t_min <= millis()) alarm_t_min = 0;

  if (!valve_status && TankSensor.avgTemp >= OPEN_VALVE_TANK_TEMP) {
    open_valve(true);
  }

  //Проверяем, что температурные параметры не вышли за предельные значения
  if ((WaterSensor.avgTemp >= MAX_WATER_TEMP) && PowerOn) {
    //Если с температурой проблемы - выключаем нагрев, пусть оператор разбирается
    set_power(false);
    Msg = "Emergency power OFF! Temperature error";
#ifdef SAMOVAR_USE_BLYNK
    //Если используется Blynk - пишем оператору
    Blynk.notify("Alarm! {DEVICE_NAME} emergency power OFF! Temperature error");
#endif
  }
  
#ifdef USE_WATERSENSOR
  //Проверим, что вода подается
  if (WFAlarmCount > WF_ALARM_COUNT) {
    //Если с водой проблемы - выключаем нагрев, пусть оператор разбирается
    set_power(false);
    Msg = "Emergency power OFF! Water error";
#ifdef SAMOVAR_USE_BLYNK
    //Если используется Blynk - пишем оператору
    Blynk.notify("Alarm! {DEVICE_NAME} emergency power OFF! Water error");
#endif
  }
#endif

  if ((WaterSensor.avgTemp >= ALARM_WATER_TEMP - 5) && PowerOn && alarm_t_min == 0) {
    //Если уже реагировали - надо подождать 30 секунд, так как процесс инерционный
    Msg = "Water temp is critical!";
#ifdef SAMOVAR_USE_BLYNK
    //Если используется Blynk - пишем оператору
    Blynk.notify("Warning! {DEVICE_NAME} water temp is critical!");
#endif

#ifdef SAMOVAR_USE_POWER
    if (WaterSensor.avgTemp >= ALARM_WATER_TEMP) {
      Msg = "Water temp is critical! Water error. Voltage down from " + (String)target_power_volt;
#ifdef SAMOVAR_USE_BLYNK
      //Если используется Blynk - пишем оператору
      Blynk.notify("Alarm! {DEVICE_NAME} water temp is critical! Water error. Voltage down from " + (String)target_power_volt);
#endif
      //Попробуем снизить напряжение регулятора на 5 вольт, чтобы исключить перегрев колонны.
      set_current_power(target_power_volt - 5);
    }
#endif
    alarm_t_min = millis() + 30000;
  }
}