//Programa : Teste MPU6050 e LCD 16*2
//Alteracoes e adaptacoes : FILIPEFLOP
//
//Baseado no programa original de JohnChi
 
//Carrega a biblioteca Wire
#include<Wire.h>
//Carrega a biblioteca do LCD
#include <LiquidCrystal.h>
 
// Inicializa o LCD
//LiquidCrystal(rs, rw, enable, d4, d5, d6, d7) 
LiquidCrystal lcd(5, 6, 7, 10, 11, 12,13);
 
//Endereco I2C do MPU6050
const int MPU=0x68;  
//Variaveis para armazenar valores dos sensores
int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
void setup()
{
  Serial.begin(9600);
  //Inicializa o LCD begin(colunas,linhas)
  lcd.begin(16, 2);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
   
  //Inicializa o MPU-6050
  Wire.write(0); 
  Wire.endTransmission(true);
}
void loop()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  //Solicita os dados do sensor
  Wire.requestFrom(MPU,14,true);  
  //Armazena o valor dos sensores nas variaveis correspondentes
  AcX=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  //vamos colocar o acelerometro na primeira linha e giro na segunda, x na coluna 0, y coluna 6 e z coluna 11 do lcd
  //Envia valor X do acelerometro para a serial e o LCD
  Serial.print("AcX = "); Serial.print(AcX);
  lcd.setCursor(0,0);
  lcd.print("X");
  lcd.print(AcX);
   
  //Envia valor Y do acelerometro para a serial e o LCD
  Serial.print(" | AcY = "); Serial.print(AcY);
  lcd.setCursor(6,0);
  lcd.print("Y");
  lcd.print(AcY);
   
  //Envia valor Z do acelerometro para a serial e o LCD
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  lcd.setCursor(11,0);
  lcd.print("Z");
  lcd.print(AcZ);
   
  //Envia valor da temperatura para a serial e o LCD
  //Calcula a temperatura em graus Celsius
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);
  //lcd.setCursor(13,0);
  //lcd.print("T:");
  //lcd.print(Tmp/340.00+36.53);
   
  //Envia valor X do giroscopio para a serial e o LCD
  Serial.print(" | GyX = "); Serial.print(GyX);
  lcd.setCursor(0,1);
  lcd.print("X");
  lcd.print(GyX);
   
  //Envia valor Y do giroscopio para a serial e o LCD  
  Serial.print(" | GyY = "); Serial.print(GyY);
  lcd.setCursor(6,1);
  lcd.print("Y");
  lcd.print(GyY);
   
  //Envia valor Z do giroscopio para a serial e o LCD
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  lcd.setCursor(11,1);
  lcd.print("Z");
  lcd.print(GyZ);
   
  //Aguarda 300 ms e reinicia o processo
  delay(300);
}
