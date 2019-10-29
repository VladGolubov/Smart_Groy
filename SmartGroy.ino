// библиотека для работы с датчиками серии DHT
#include <TroykaDHT.h>
#include <LiquidCrystal.h>
// Библиотека оборачивания меню
#include <LiquidMenu.h>
#include "Button.h"
#include <PIDController.h>
PIDController ground; // Create an instance of the PID controller class, called "ground"
PIDController air; // Create an instance of the PID controller class, called "air"


// создаём объект класса DHT
// передаём номер пина к которому подключён датчик и тип датчика
// типы сенсоров: DHT11, DHT21, DHT22
DHT dht(4, DHT21);
#define ONE_WIRE_BUS 2 //пин для подключения dallas 18b20

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer1 = {                  //TempGr
  0x28, 0x00, 0x54, 0xB6, 0x04, 0x00, 0x00, 0x92
}; // адрес 1 датчика DS18B20 280054B604000092:
DeviceAddress Thermometer2 = {                  //outTemp
  0x28, 0x00, 0x54, 0xB6, 0x04, 0x00, 0x00, 0x92
}; // адрес 2 датчика DS18B20 280054B604000092:

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define alarmCO2 300 // найти критичное значение углекислого газа
//--------------------------------------------------------------------------
// This variable can be passed to the LiquidMenu object to set the starting screen.
const byte startingScreen = 2;

// Button objects instantiation
/*
 * The Button class is not a part of the LiquidMenu library. The first
 * parameter is the button's pin, the second enables or disables the
 * internal pullup resistor (not required) and the third is the debounce
 * time (not required).
 */
// Button objects instantiation
const bool pullup = true;
Button left(14, pullup);
Button right(15, pullup);
Button up(16, pullup);
Button down(17, pullup);
Button enter(18, pullup);
 
//--------------------------------------------------------------------------

uint32_t  timing;
uint32_t  timing_1;

int16_t humGr;
int8_t humAir;
int8_t TempAir;
int8_t TempGr; // посмотреть подключение dallas 18b20
int8_t lightAir;
int8_t PumpGr;

//--------------------------------------------------------------------------
//setFunction
int16_t setHumGr;
int16_t setHumAir;
int16_t setTempAir;
int16_t setTempGr;// посмотреть подключение dallas 18b20
int16_t setPumpGr;
int16_t setLight;
//--------------------------------------------------------------------------

//int16_t outTemp; // посмотреть подключение dallas 18b20   если решу делать показания внешний температуры
// термокомпенсацию и просчитывание мощности обогрева
int16_t lightPin = A2;
int16_t sensorLightPin = A3;
//int16_t light;  не помню откуда это
int16_t sensorLight;

//*************************define***********************
#define valveAirPin  5
int16_t  humGrPin = A1;
#define fanPin  6
#define pumpGrPin  7
#define tenGrPin  8
#define tenAirPin  9
#define CO2  10 //узнать подключение MQ
// Data wire is plugged into port 2 on the Arduino


//-----------------------------------------------------------------------------------------------------------------



/*
 * An enumerator can be used for the callback functions. It sets the number
 * for similar functions. Later when a function is attached it can be passed
 * with this enum rather than a magic number.
 */
// There are two types of functions for this example, one that increases some
// value and one that decreases it.
enum FunctionTypes {
  increase = 1,
  decrease = 2,
};
/*
 * Variables used for periodic execution of code. The first one is the period
 * in milliseconds and the second one is the last time the code executed.
 */
unsigned int period_check = 1000;
unsigned long lastMs_check = 0;

unsigned int period_nextScreen = 5000;
unsigned long lastMs_nextScreen = 0;


//--------------------------------------------------------------------------

void controlAirTemp(){
dht.read();
dht.getState();
TempAir = dht.getTemperatureC();
if(TempAir > setTempAir){
       digitalWrite(valveAirPin, HIGH);
 if (millis() - timing_1 > 5000){ // Вместо 10000 подставьте нужное вам значение паузы 
      timing_1 = millis(); // задержка 5 секунд для того, чтобы открылся воздушный клапан
       digitalWrite(fanPin, HIGH);
      int output = ground.compute(TempAir);    // Let the PID compute the value, returns the optimal output
       analogWrite(tenAirPin, output);           // Write the output to the output pin
    }
  }
}

 void controlGrTemp(){
  if (millis() - timing > 1000){ // Вместо 10000 подставьте нужное вам значение паузы 
      timing = millis(); // задержка 1 секунда
         float val = sensors.getTempC(Thermometer1);
         TempGr = (int)val;
         //  int sensorValue = float tempGr;  // Read the value from the sensor .. не то
         int output = ground.compute(TempGr);    // Let the PID compute the value, returns the optimal output
         analogWrite(tenGrPin, output);           // Write the output to the output pin
     }
 }

void controlCO2(){  // найти тип датчика и подключение!!!!
  if(CO2 > alarmCO2){
    lcd.print("CO2");
  //  digitalWrite(fan, HIGH);
  }
 }

void controlHumGr(){
//https://vk.com/im?sel=c35&w=product-79970674_3088022_f07b6081355ff8514e
humGr = readMedian(humGrPin, 5);
 if (humGr < setHumGr){
   digitalWrite(pumpGrPin, HIGH);
  }else{
   digitalWrite(pumpGrPin, LOW);
 }
}

void controlPumpGr(){
   
  }
  
void controlLight(){
  
  }
  
void controlHumAir(){
  }
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
 // функция считывает аналоговый вход заданное количество раз
// и возвращает медианное отфильтрованное значение
int readMedian (int pin, int samples ){
  // массив для хранения данных
  int raw[samples];
  // считываем вход и помещаем величину в ячейки массива
  for (int i = 0; i < samples; i++){
    raw[i] = analogRead(pin);
  }
  // сортируем массив по возрастанию значений в ячейках
  int temp = 0; // временная переменная

  for (int i = 0; i < samples; i++){
    for (int j = 0; j < samples - 1; j++){
      if (raw[j] > raw[j + 1]){
        temp = raw[j];
        raw[j] = raw[j + 1];
        raw[j + 1] = temp;
      }
    }
  }
  // возвращаем значение средней ячейки массива
  return raw[samples/2];
 }
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// A LiquidLine object can be used more that once.
/*LiquidLine back_line(11, 1, "/BACK");

LiquidLine welcome_line1(1, 0, "Smart " );
LiquidLine welcome_line2(1, 1, "Greenhouse");
LiquidScreen welcome_screen(welcome_line1, welcome_line2);

// These lines direct to other menus.

LiquidLine setGrScreen_line(0, 0, "/setGrScreen");
LiquidLine setAirScreen_line(0, 1, "/setAirScreen");
LiquidScreen io1_screen(setGrScreen_line, setAirScreen_line);

LiquidLine Air_Screen_line(0, 0, "/Air_Screen");
LiquidLine Gr_Screen_line(0, 1, "/Gr_Screen");
LiquidScreen io2_screen(Air_Screen_line, Gr_Screen_line);

// This is the first menu.

LiquidMenu main_menu(lcd, welcome_screen, io1_screen, io2_screen, 1);

*/
LiquidLine setHumidityGround_1(0, 0, "Hum. ground ");
LiquidLine setHumidityGround_2(0, 1, setHumGr);
LiquidScreen setHumidityGround_screen(setHumidityGround_1, setHumidityGround_2);

LiquidLine setTemperatureGround_1(0, 0, "Temp. ground");
LiquidLine setTemperatureGround_2(0, 1, setTempGr);
LiquidScreen setTemperatureGround_screen(setTemperatureGround_1, setTemperatureGround_2);

LiquidLine setPumpGround_1(0, 0, "Pump. ground");
LiquidLine setPumpGround_2(0, 1, setPumpGr);
LiquidScreen setPumpGround_screen(setPumpGround_1, setPumpGround_1);

LiquidMenu menu_1(lcd, setHumidityGround_screen, setTemperatureGround_screen, setPumpGround_screen);

//--------------------------------------------------------------------------

LiquidLine setHumidityAir_1(0, 0, "Hum. air ");
LiquidLine setHumidityAir_2(0, 1, setHumAir);
LiquidScreen setHumidityAir_screen(setHumidityAir_1, setHumidityAir_2);

LiquidLine setTemperatureAir_1(0, 0, "Temp. air");
LiquidLine setTemperatureAir_2(0, 1, setTempAir);
LiquidScreen setTemperatureAir_screen(setTemperatureAir_1, setTemperatureAir_2);

LiquidLine setSensorLight_1(0, 0, "Light sensor");
LiquidLine setSensorLight_2(0, 1, setLight);
LiquidScreen setSensorLight_screen(setSensorLight_1, setSensorLight_2);

LiquidMenu menu_2(lcd, setHumidityAir_screen, setTemperatureAir_screen, setSensorLight_screen);

//--------------------------------------------------------------------------

LiquidLine humidityAir_1(0, 0, "Hum. air ");
LiquidLine humidityAir_2(0, 1, humAir);
LiquidScreen humidityAir_screen(humidityAir_1, humidityAir_2);

LiquidLine temperatureAir_1(0, 0, "Temp. air");
LiquidLine temperatureAir_2(0, 1, humAir);
LiquidScreen temperatureAir_screen(temperatureAir_1, temperatureAir_2);

LiquidLine SensorLight_1(0, 0, "Light sensor");
LiquidLine SensorLight_2(0, 1, lightAir);
LiquidScreen SensorLight_screen(SensorLight_1, SensorLight_2);

LiquidMenu menu_3(lcd, humidityAir_screen, temperatureAir_screen, SensorLight_screen);

//--------------------------------------------------------------------------

LiquidLine HumidityGround_1(0, 0, "Hum. ground ");
LiquidLine HumidityGround_2(0, 1, humGr);
LiquidScreen HumidityGround_screen(HumidityGround_1, HumidityGround_2);

LiquidLine TemperatureGround_1(0, 0, "Temp. ground");
LiquidLine TemperatureGround_2(0, 1, TempGr);
LiquidScreen TemperatureGround_screen(TemperatureGround_1, TemperatureGround_2);

LiquidLine PumpGround_1(0, 0, "Pump. ground");
LiquidLine PumpGround_2(0, 1, PumpGr);
LiquidScreen PumpGround_screen(PumpGround_1, PumpGround_1);

LiquidMenu menu_4(lcd, HumidityGround_screen, TemperatureGround_screen, PumpGround_screen);

LiquidSystem menu_system( menu_1, menu_2, menu_3, menu_4);

//-----------------------------------------------------------------------------------------------------------------
void buttonsCheck() {
  if (right.check() == LOW) {
    menu_system.next_screen();
  }
  if (left.check() == LOW) {
    menu_system.previous_screen();
  }
  if (up.check() == LOW) {
    menu_system.call_function(increase);
  }
  if (down.check() == LOW) {
    menu_system.call_function(decrease);
  }
  if (enter.check() == LOW) {
    menu_system.switch_focus();
  }
}
//--------------------------------------------------------------------------
// функции для того чтобы выставить уровень
// Callback function that will be attached to back_line.
/*void go_back() {
  // This function takes reference to the wanted menu.
  menu_system.change_menu(main_menu);
}

void goto_menu1() {
  // This function takes reference to the wanted menu.
  menu_system.change_menu(menu_1);
}

void goto_menu_2() {
  menu_system.change_menu(menu_2);
}

void goto_menu_3() {
  menu_system.change_menu(menu_3);
}

void goto_menu_4() {
  menu_system.change_menu(menu_4);
}
*/

//             Set функции 
//-------------------------------------------------------
void increase_setHumGr() {
  if (setHumGr < 40) {
    setHumGr += 1;
  } else {
    setHumGr = 40;
  }
}
//------------------------------------
void decrease_setHumGr() {
  if (setHumGr > 5) {
    setHumGr -= 1;
  } else {
    setHumGr = 0;
  }
}
//------------------------------------
void increase_setHumAir() {
  if (setHumAir < 40) {
    setHumAir += 1;
  } else {
    setHumAir = 40;
  }
}
//------------------------------------
void decrease_setHumAir() {
  if (setHumAir > 5) {
    setHumAir -= 1;
  } else {
    setHumAir = 0;
  }
}
//------------------------------------
void increase_setTempGr() {
  if (setTempGr < 40) {
    setTempGr += 1;
  } else {
    setTempGr = 40;
  }
}
//------------------------------------
void decrease_setTempGr() {
  if (setTempGr > 5) {
    setTempGr -= 1;
  } else {
    setTempGr = 0;
  }
}
//------------------------------------
void increase_setTempAir() {
  if (setTempGr < 40) {
    setTempAir += 1;
  } else {
    setTempAir = 40;
  }
}
//------------------------------------
void decrease_setTempAir() {
  if (setTempAir > 5) {
    setTempAir -= 1;
  } else {
    setTempAir = 0;
  }
}
//------------------------------------
void increase_setPumpGr() {
  if (setPumpGr < 100) {
    setPumpGr += 1;
  } else {
    setPumpGr = 100;
  }
}
//------------------------------------
void decrease_setPumpGr() {
  if (setPumpGr > 5) {
    setPumpGr -= 1;
  } else {
    setPumpGr = 0;
  }
}
//------------------------------------
void increase_setLight() {
  if (setLight < 100) {
    setLight += 1;
  } else {
    setLight = 100;
  }
}
//------------------------------------
void decrease_setLight() {
  if (setLight > 5) {
    setLight -= 1;
  } else {
    setLight = 0;
  }
}
//-------------------------------------------------------


 

void setup() {
//--------------------------------------------------------------------------

  //dht sensor
     dht.begin();
     sensors.begin();
  
  // Instalizing PID for ground ten
     ground.begin();          // initialize the PID instance
     ground.setpoint(600);    // The "goal" the PID controller tries to "reach"
     ground.tune(1, 1, 1);    // Tune the PID, arguments: kP, kI, kD
     ground.limit(0, 255);    // Limit the PID output between 0 and 255, this is important to get rid of integral windup!
  
  // Instalizing PID for air ten
     air.begin();          // initialize the PID instance
     air.setpoint(600);    // The "goal" the PID controller tries to "reach"
     air.tune(1, 1, 1);    // Tune the PID, arguments: kP, kI, kD
     air.limit(0, 255);    // Limit the PID output between 0 and 255, this is important to get rid of integral windup!


//--------------------------------------------------------------------------
  //INPUT
 pinMode(humGrPin, INPUT);
 pinMode(CO2, INPUT);
 pinMode(sensorLightPin, INPUT);

//--------------------------------------------------------------------------
  //OUTPUT
 pinMode(lightPin, OUTPUT);
 pinMode(valveAirPin, OUTPUT);
 pinMode(fanPin, OUTPUT);
 pinMode(pumpGrPin, OUTPUT);
 pinMode(tenGrPin, OUTPUT);
 pinMode(tenAirPin, OUTPUT);
 
//--------------------------------------------------------------------------
  /* This methid needs to be called when using an I2C display library.
  menu_1.init();
  menu_2.init();
  menu_3.init();
  menu_4.init();*/

/*back_line.set_focusPosition(Position::LEFT);

  back_line.attach_function(1, go_back);
  back_line.attach_function(2, go_back);
/*
  setGrScreen_line.attach_function(1, goto_menu1);
  setGrScreen_line.attach_function(2, goto_menu1);
  setAirScreen_line.attach_function(1, goto_menu2);
  setAirScreen_line.attach_function(2, goto_menu2);

  Air_Screen_line.attach_function(1, goto_menu3);
  Air_Screen_line.attach_function(2, goto_menu3);
  Gr_Screen_line.attach_function(1, goto_menu4);
  Gr_Screen_line.attach_function(2, goto_menu4);
*/

 //            Строки инциализации set функций
//------------------------------------------------------------------------
    setHumidityGround_2.attach_function(increase, increase_setHumGr);
  setHumidityGround_2.attach_function(decrease, decrease_setHumGr);
  
    setHumidityAir_2.attach_function(increase, increase_setHumAir);
  setHumidityAir_2.attach_function(decrease, decrease_setHumAir);
  
    setTemperatureGround_2.attach_function(increase, increase_setTempGr);
  setTemperatureGround_2.attach_function(decrease, decrease_setTempGr);
  
    setTemperatureAir_2.attach_function(increase, increase_setTempAir);
  setTemperatureAir_2.attach_function(decrease, decrease_setTempAir);
  
    setPumpGround_2.attach_function(increase, increase_setPumpGr);
  setPumpGround_2.attach_function(decrease, decrease_setPumpGr);
  
    setSensorLight_2.attach_function(increase, increase_setLight);
  setSensorLight_2.attach_function(decrease, decrease_setLight);
//--------------------------------------------------------------------------

menu_system.update();

}

void loop(){


delay(10);
    controlAirTemp();
    controlGrTemp();
    controlCO2();
    controlHumGr();
    menu_system.update();
  }
