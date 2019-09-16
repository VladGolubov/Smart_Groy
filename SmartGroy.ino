// библиотека для работы с датчиками серии DHT
#include <TroykaDHT.h>
#include <LiquidCrystal.h>
// создаём объект класса DHT
// передаём номер пина к которому подключён датчик и тип датчика
// типы сенсоров: DHT11, DHT21, DHT22
DHT dht(4, DHT21);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int humGrPin = ;
int humGr = ;
int humAir = ;
int tempAir = ;
int setHumAir = ;
int setTempAir = ;
int tempGr = ; // посмотреть подключение dallas 18b20
int setTempGr = ;// посмотреть подключение dallas 18b20
int outTemp = ; // посмотреть подключение dallas 18b20
int lightPin = ;
int light = ;
int valveAirPin = ;
int fan = ;
int pumpGrPin = ;
int tenGrPin = ;
int tenAirPin = ;
int CO2 = ;

void setup() {
  dht.begin();

}

void loop() {
void controlAirTemp(){
dht.read();
dht.getState();
tempAir = dht.getTemperatureC();
if(tempAir < setTempAir){
  digitalWrite(valveAirPin, HIGH);
 // сделать счетчик с замедленнием перед нагревом, чтобы успела открыться заслонка
  digitalWrite(fan, HIGH);
  digitalWrite(tenAirPin, HIGH);
}else{
  digitalWrite(tenAirPin, LOW);
  digitalWrite(valveAirPin, LOW);
 // сделать счетчик с замедленнием после нагрева, чтобы успел охладиться тэн
  digitalWrite(fan, LOW);
 
}
}
}
}
