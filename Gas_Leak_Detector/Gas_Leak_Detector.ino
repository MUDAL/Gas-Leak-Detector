/**
 * @Description: Gas Leak Detector
 * @Components:
 * 1.)Arduino board
 * 2.)MQ-2 Gas sensor
 * 3.)SIM800L GSM module
 * 4.)Buzzer
 * 5.)Power supply
*/
#include <SoftwareSerial.h>

const char phoneNumber[] = "+2348144086708"; //Change to number of choice
const int gasThreshold = 500;
const int gasSensorPin = A0;
const int alarmPin = 7; //leakage indication
const int sim800TxPin = 10;
const int sim800RxPin = 11;
bool gasLeakPreviouslyDetected = false;
SoftwareSerial Sim800l(sim800TxPin,sim800RxPin);

/**
 * @brief Sends an SMS to a phone number.  
 * @param phoneNo: number to send an SMS to.  
 * @param message: content of SMS to be sent.  
*/
void SendSMS(char* phoneNo,char* message);

void setup()
{
  Serial.begin(9600); //for debugging
  Sim800l.begin(9600);
  pinMode(alarmPin,OUTPUT);
}

void loop()
{
  int gasValue = analogRead(gasSensorPin);
  Serial.println(gasValue);
  
  if(gasValue >= gasThreshold)
  {
    if(!gasLeakPreviouslyDetected)
    {
      gasLeakPreviouslyDetected = true;
      SendSMS(phoneNumber,"Gas leakage detected!!!");
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
}

void SendSMS(char* phoneNo,char* message)
{
  char smsATCmd[25] = "AT+CMGS=\"";
  Sim800l.println("AT+CMGF=1");
  delay(50);
  strcat(smsATCmd,phoneNo);
  strcat(smsATCmd,"\"\r\n");
  Sim800l.print(smsATCmd);
  delay(50);
  Sim800l.println(message);
  delay(50);
  Sim800l.write(26); //Command termination
  delay(50);
}
