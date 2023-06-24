/*Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL_YR784cp"
#define BLYNK_TEMPLATE_NAME         "Home Security"
#define BLYNK_AUTH_TOKEN            "SwDbEXCxyUraz2kBtSeyN7WtsU0pgx93"

//initialize email server
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "microprojectgroup5@gmail.com"
#define AUTHOR_PASSWORD "uhtihilxmvlvszca"

/* Recipient's email*/
#define RECIPIENT_EMAIL "wsf.hmd99@gmail.com"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include "FS.h"
#include "SD.h"
#include "SPI.h"
//The FS library for handling files.
//the SD library for the microSD card functionality.
//the SPI library for using SPI communication protocol 
//between the ESP32 board and the microSD card module.
#include <WiFi.h>
#include <WiFiClient.h>
//this two headers are for connecting to wifi
#include <BlynkSimpleEsp32.h>
//this is for Blynk connection
#include <Keypad.h>
//this for keypad control
#include <ESP32Servo.h>
//this is for servo control
#include <ESP_Mail_Client.h>
//this is for E-mail
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
String password="",realPass="000000", tempPass;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {12, 14, 27, 26}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {25, 33, 32, 35}; //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Create an object of LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

int timerIdLCD,timerIdAlarm;

BlynkTimer timer;

const int servoPinDoor=15, servoPinWindow=4, servoPinSafeRoom=16;
const int touchPin=13, relayPin=17, pirPin=34;
const int touchThreshold=30;
int doorState=0, windowState=0, safeRoomState=0, panicModeState=0, secureModeState=0, alarmState=0;
//1 means state is off, 0 means state is off
int passwordMode=0;
int wrongPassCount=0;
//0 = taking password input, 1 = get old password for reset, 2 =resetting password, 3 = confirming password

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Redmi Note 10 Lite";
char pass[] = "qwertyuio";
// char ssid[] = "Shitdroid";
// char pass[] = "Washa1998";

///////////////////////Servo::Start/////////////////////////

Servo doorServo, windowServo, safeRoomServo;
int timerIdOpenDoorServo,timerIdOpenWindowServo,timerIdOpenSafeRoomServo;
int timerIdCloseDoorServo,timerIdCloseWindowServo,timerIdCloseSafeRoomServo;
int timeoutTimerIdCloseDoorServo;
int posWindow=0,posDoor=0,posSafeRoom=0;


void openDoorServo(){
  doorServo.write(posDoor+1);
  posDoor++;
  if(posDoor>=100){
    doorState=1;
    Blynk.virtualWrite(V1,HIGH);
    timer.setTimeout(5000L,timeoutCloseDoorServo);
    timer.disable(timerIdOpenDoorServo);
    // timer.disable(timeoutTimerIdCloseDoorServo); 
    timer.enable(timeoutTimerIdCloseDoorServo);   
  }
}
void closeDoorServo(){
  if(posDoor==0){
    timer.disable(timerIdCloseDoorServo);
    return;
  }
  doorServo.write(posDoor-1);
  posDoor--;
  if(posDoor==0){
    doorState=0;
    Blynk.virtualWrite(V1,LOW);      
    timer.disable(timerIdCloseDoorServo);   
  }
}
void timeoutCloseDoorServo(){
  if(doorState==1){
    timer.disable(timerIdOpenDoorServo);
    timer.enable(timerIdCloseDoorServo);
  }
}

void openWindowServo(){
  windowServo.write(posWindow+1);
  posWindow++;
  if(posWindow>=100){
    windowState=1;
    Blynk.virtualWrite(V2,HIGH);
    timer.disable(timerIdOpenWindowServo);  
  }
}
void closeWindowServo(){
  if(posWindow==0)posWindow++;
  windowServo.write(posWindow-1);
  posWindow--;
  if(posWindow==0){
    Blynk.virtualWrite(V2,LOW);
    windowState=0;
    timer.disable(timerIdCloseWindowServo);   
  }
}
void openSafeRoomServo(){
  safeRoomServo.write(posSafeRoom+1);
  posSafeRoom++;
  if(posSafeRoom>=100){
    safeRoomState=1;
    Blynk.virtualWrite(V3,HIGH);
    timer.disable(timerIdOpenSafeRoomServo); 
  }
}
void closeSafeRoomServo(){
  if(posSafeRoom==0)posSafeRoom++;
  safeRoomServo.write(posSafeRoom-1);
  posSafeRoom--;
  if(posSafeRoom==0){
    Blynk.virtualWrite(V3,LOW);
    safeRoomState=0;    
    timer.disable(timerIdCloseSafeRoomServo);   
  }
}
//////////////////////////////Servo::End//////////////////////////////

////////////The Blynk Datastream Change Handler : START//////////////
// // This function is called every time the device is connected to the Blynk.Cloud
// BLYNK_CONNECTED()
// {
//   // Change Web Link Button message to "Congratulations!"
//   Blynk.setProperty(V9, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
// }
// This function is called every time the Virtual Pin 1 state changes
BLYNK_WRITE(V13){
  alarmState = param.asInt();
  if(alarmState){
    startAlarm();
  }
  else{
    stopAlarm();
  }
}

BLYNK_WRITE(V1)
{
  // Set incoming value from pin V1 to a variable
  doorState = param.asInt();

  // Update state
  if(doorState){
    timer.disable(timerIdCloseDoorServo); 
    timer.enable(timerIdOpenDoorServo); 
  }
  else{
    timer.disable(timerIdOpenDoorServo);
    timer.enable(timerIdCloseDoorServo);  
  }  
  (doorState==0)?Serial.println("Door is closed"):Serial.println("Door is opened");
}

BLYNK_WRITE(V2)
{
  // Set incoming value from pin V0 to a variable
  windowState = param.asInt();

  // Update state
  if(windowState){
    timer.disable(timerIdCloseWindowServo); 
    timer.enable(timerIdOpenWindowServo); 
  }
  else{
    timer.disable(timerIdOpenWindowServo);
    timer.enable(timerIdCloseWindowServo);  
  }
  (windowState==0)?Serial.println("Window is closed"):Serial.println("Window is opened");
}

BLYNK_WRITE(V3)
{
  // Set incoming value from pin V0 to a variable
  safeRoomState = param.asInt();

  // Update state
  if(safeRoomState){
    timer.disable(timerIdCloseSafeRoomServo); 
    timer.enable(timerIdOpenSafeRoomServo); 
  }
  else{
    timer.disable(timerIdOpenSafeRoomServo);
    timer.enable(timerIdCloseSafeRoomServo);  
  }
  (safeRoomState==0)?Serial.println("Safe Room Door is closed"):Serial.println("Safe Room Door is opened");
}

BLYNK_WRITE(V4)
{
  // Set incoming value from pin V0 to a variable
  panicModeState = param.asInt();

  if(panicModeState){
    startPanicking();
  }

  else{
    stopAlarm();
  }
  // Update state
  (panicModeState==0)?Serial.println("Panic Mode is turned off"):Serial.println("Panic Mode is turned on");
}

BLYNK_WRITE(V5)
{
  // Set incoming value from pin V0 to a variable
  secureModeState = param.asInt();
  if(secureModeState){
    timer.enable(timerIdCloseDoorServo); 
    timer.disable(timerIdOpenDoorServo); 
    timer.enable(timerIdCloseWindowServo); 
    timer.disable(timerIdOpenWindowServo); 
    timer.enable(timerIdCloseSafeRoomServo); 
    timer.disable(timerIdOpenSafeRoomServo); 
  }

  // Update state
  (secureModeState==0)?Serial.println("Secure Mode is turned off"):Serial.println("Secure Mode is turned on");
}

////////////The Blynk Datastream Change Handler : END///////////////
///////////////////////////////////////////////////////////////////
void startPanicking(){
  if(panicModeState==1){
    Blynk.virtualWrite(V4,HIGH);
    timer.enable(timerIdCloseDoorServo); 
    timer.disable(timerIdOpenDoorServo); 
    timer.enable(timerIdCloseWindowServo); 
    timer.disable(timerIdOpenWindowServo); 
    timer.disable(timerIdCloseSafeRoomServo);
    timer.enable(timerIdOpenSafeRoomServo);  
    startAlarm();
  }
}
//////////////////////File Operations::Start///////////////////////


void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        realPass="000000";
        return;
    }
    realPass="";
    Serial.print("Read from file: ");
    while(file.available()){
        realPass+=(char)file.read();
    }
    file.close();
    return;
}

void writeFile(fs::FS &fs, const char * path, String message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void initSdCard(){
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    readFile(SD, "/pass.txt");
    // Serial.println("Your pass is : "+pass);
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));  
}

//////////////////////File Operations::End///////////////////////

/////////////////////E-mail::Start////////////////////////////

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

void sendEmail(String subject, String body){
  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "Group5";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = subject;
  message.addRecipient("Wasif", RECIPIENT_EMAIL);

  /*Send HTML message*/
  String htmlMsg = body;
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  // if (!MailClient.sendMail(&smtp, &message))
    // Serial.printf("Error sending Email, %s", smtp.errorReason());
}


/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
  }
}
/////////////////////E-mail::End///////////////////////////////
//////////////////////buzzer Functions//////////////////////////
void initAlarm(){
  pinMode(relayPin,OUTPUT);
  timerIdAlarm=timer.setInterval(1000L,soundAlarm);
  timer.disable(timerIdAlarm);
  digitalWrite(relayPin,HIGH);
}
void startAlarm(){
  timer.enable(timerIdAlarm);
  Blynk.virtualWrite(V13,HIGH);
}
void stopAlarm(){
  timer.disable(timerIdAlarm);
  digitalWrite(relayPin,HIGH);
  Blynk.virtualWrite(V13,LOW);
}

void soundAlarm(){
  if(alarmState==0){
    digitalWrite(relayPin,LOW);
    alarmState=1;
  }
  else{
    digitalWrite(relayPin,HIGH);
    alarmState=0;
  }
}
//////////////////////init Functions//////////////////////////
void initLCD(){
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  showLCD();          
}

void initServo(){
  doorServo.attach(servoPinDoor);
  windowServo.attach(servoPinWindow);
  safeRoomServo.attach(servoPinSafeRoom);
  timeoutTimerIdCloseDoorServo=timer.setInterval(5000L,timeoutCloseDoorServo);
  timerIdOpenDoorServo=timer.setInterval(10L,openDoorServo); 
  timer.disable(timerIdOpenDoorServo);   
  timerIdOpenWindowServo=timer.setInterval(10L,openWindowServo);
  timer.disable(timerIdOpenWindowServo);  
  timerIdOpenSafeRoomServo=timer.setInterval(10L,openSafeRoomServo);
  timer.disable(timerIdOpenSafeRoomServo);       
  timerIdCloseDoorServo=timer.setInterval(10L,closeDoorServo);
  timer.disable(timerIdCloseDoorServo);  
  timerIdCloseWindowServo=timer.setInterval(10L,closeWindowServo);
  timer.disable(timerIdCloseWindowServo);  
  timerIdCloseSafeRoomServo=timer.setInterval(10L,closeSafeRoomServo); 
  timer.disable(timerIdCloseSafeRoomServo);
}

void showLCD(){
  if(passwordMode==0){
    lcd.setCursor(4,0);
    lcd.print("Password");
  }
  else if(passwordMode==1){
    lcd.setCursor(2,0);                
    lcd.print("Old Password");    
  }
  else if(passwordMode==2){
    lcd.setCursor(2,0);                
    lcd.print("New Password");    
  }
  else if(passwordMode==3){
    lcd.setCursor(0,0);                
    lcd.print("Confirm Password");    
  }  
  lcd.setCursor(5,1);
  for(int i=0;i<password.length();i++)lcd.print("*");
  for(int i=password.length();i<6;i++)lcd.print("_");      
}

void timedShowLCD(){
  lcd.clear();
  showLCD(); 
}

void setTimedShowLCD(){
  timer.setTimeout(2000L,timedShowLCD); 
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  timer.setTimeout(3600000L, [] () {} );
  initLCD();   
  Serial.println("Starting Connection to Blynk");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connected to Blynk");
  initServo();   
  initSdCard();
  initAlarm();   
  pinMode(pirPin,INPUT); 
  Serial.println(realPass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  // timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  char key = keypad.getKey();// Read the key
  
  // Print if key pressed
  if (key && !panicModeState){
    Serial.print("Key Pressed : ");
    Serial.println(key);
    //reset password
    if(key=='A'){
      passwordMode=1;//setting up reset password mode
      password="";
      lcd.clear();
      showLCD();      
    }
    else if(key=='B'){
      passwordMode=0;//cancelling reset password mode
      password="";
      lcd.clear();
      showLCD();
    }
    else if(key=='C'){
      password="";
      lcd.clear();//clearing given password  
      showLCD();    
    }     
    else if(key=='D'){
      password=password.substring(0,password.length()-1);
      lcd.clear();
      showLCD();
    }     
    else {
      password+=key;
      if(password.length()==6){
        if(passwordMode==0){
          if(password.equals(realPass)){
            password="";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Correct Password");
            setTimedShowLCD();       
            timer.disable(timerIdCloseDoorServo);
            timer.enable(timerIdOpenDoorServo);
          }
          else{
            password="";
            wrongPassCount++;
            if(wrongPassCount==3){
              panicModeState=1;
              startPanicking();
              Serial.println("Panicking");
              lcd.clear();
              lcd.setCursor(4,0);
              lcd.print("Too Many");
              lcd.setCursor(0,1);
              lcd.print("Wrong Passwords");
              setTimedShowLCD();    
              Blynk.logEvent("wrong_pass");
              sendEmail("Too Many Wrong Passwords","Someone tried to open the door with wrong password <b>3 times</b> in a row. </br>The alarm is now activated.");   
              ////warn people//////////////////////////////////////               
            }
            else {
              lcd.clear();
              lcd.setCursor(1,0);
              lcd.print("Wrong Password");
              lcd.setCursor(0,1);
              lcd.print("Remaining Try:");
              lcd.print((char)(3-wrongPassCount+'0'));
              setTimedShowLCD();                   
            }           
          }
        }
        else if(passwordMode==1){
          if(password.equals(realPass)){
            tempPass=password;
            password="";
            passwordMode=2;
            showLCD();
          }
          else{
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("Wrong Password");
            setTimedShowLCD();                           
          }          
        }        
        else if(passwordMode==2){
          tempPass=password;
          password="";
          passwordMode=3;         
        }
        else if(passwordMode==3){        
          if(password.equals(tempPass)){
            realPass=tempPass;
            writeFile(SD, "/pass.txt", realPass);
            password="";
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Password Changed");
            setTimedShowLCD();       
          }
          else{
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("Differant Pass");
            password="";
            passwordMode=2;
            setTimedShowLCD();                 
          }           
        }
      }
      else showLCD();
    } 
  }  

  if(touchRead(touchPin)<touchThreshold){
    Serial.println("touch sensor fired");
    Blynk.logEvent("door_touch");//warning touch sensor
    startAlarm();
  }
  if(analogRead(pirPin)>=1700 && secureModeState==1 ){
    Serial.println("pir sensor fired");
    secureModeState=0;
    Blynk.virtualWrite(V5,LOW);
    panicModeState=1;
    //warning pir sensor
    startAlarm();
    Blynk.logEvent("breach");
    sendEmail("Breach Detected","Someone has <b>infiltrated</b> your house");
  }
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
