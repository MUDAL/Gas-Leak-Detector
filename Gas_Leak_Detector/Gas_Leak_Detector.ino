/**
 * @Description: Gas Leak Detector
 * @Components:
 * 1.)Arduino board
 * 2.)MQ-2 Gas sensor
 * 3.)SIM800L GSM module
 * 4.)LED
 * 5.)Power supply
 * 6.)Bluetooth HC-05 module
*/
#include <SoftwareSerial.h>
const int gasConcentrationThreshold = 300;
const int gasSensorPin = A0;
const int hc05TxPin = 2;
const int hc05RxPin = 3;
const int alarmPin = 7; //leakage indication
const int sim800TxPin = 10;
const int sim800RxPin = 11;
const int phoneNoLen = 14;
char hc05RxData[phoneNoLen+1] = "+234xxxxxxxxxx";
bool gasLeakPreviouslyDetected = false;
SoftwareSerial hc05(hc05TxPin,hc05RxPin);
SoftwareSerial sim800l(sim800TxPin,sim800RxPin);

/**
 * @brief gets phone number received via bluetooth
*/
void GetPhoneNumber(void);
/**
 * @brief Sends an SMS to a phone number.  
 * @param phoneNo: number to send an SMS to.  
 * @param message: content of SMS to be sent.  
*/
void SendSMS(char* phoneNo,char* message);

void setup()
{
  Serial.begin(9600); //for debugging
  while(!Serial)
  {
    //wait for serial port to connect
  }
  hc05.begin(9600);
  sim800l.begin(9600);
  pinMode(alarmPin,OUTPUT);
}

void loop()
{
  GetPhoneNumber();
  int gasConcentration = analogRead(gasSensorPin);
  if(gasConcentration >= gasConcentrationThreshold)
  {
    if(!gasLeakPreviouslyDetected)
    {
      gasLeakPreviouslyDetected = true;
      SendSMS(hc05RxData,"Gas leakage detected!!!");
      digitalWrite(alarmPin,HIGH);
    }
  }
  else
  {
    if(gasLeakPreviouslyDetected)
    {
      gasLeakPreviouslyDetected = false;
      digitalWrite(alarmPin,LOW);
    }
  }
  //Debug
  Serial.print("Gas concentration = ");
  Serial.println(gasConcentration);
  Serial.print("Phone number = ");
  Serial.println(hc05RxData);
  delay(1000);
}

void GetPhoneNumber(void)
{
  static volatile bool newNumber;
  int i = 0;
  hc05.listen();
  while(hc05.available())
  {
    newNumber = true;
    if(i >= phoneNoLen)
    {
      break;
    }
    hc05RxData[i] = hc05.read();
    i++;
  }
  if(newNumber)
  {
    hc05.print("phone: ");
    hc05.println(hc05RxData);
    newNumber = false;
  }
}

void SendSMS(char* phoneNo,char* message)
{
  char smsATCmd[25] = "AT+CMGS=\"";
  sim800l.println("AT+CMGF=1");
  delay(50);
  strcat(smsATCmd,phoneNo);
  strcat(smsATCmd,"\"\r\n");
  sim800l.print(smsATCmd);
  delay(50);
  sim800l.println(message);
  delay(50);
  sim800l.write(26); //Command termination
  delay(50);
}
