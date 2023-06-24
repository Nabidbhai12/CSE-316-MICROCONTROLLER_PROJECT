/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL_YR784cp"
#define BLYNK_TEMPLATE_NAME         "Home Security"
#define BLYNK_AUTH_TOKEN            "SwDbEXCxyUraz2kBtSeyN7WtsU0pgx93"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
String password="";

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26,25, 33, 32}; //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Shitdroid";
char pass[] = "Washa1998";

int doorState, windowState, safeRoomState, panicModeState, secureModeState;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V1)
{
  // Set incoming value from pin V0 to a variable
  doorState = param.asInt();

  // Update state
  (doorState==0)?Serial.println("Door is closed"):Serial.println("Door is opened");
}

BLYNK_WRITE(V2)
{
  // Set incoming value from pin V0 to a variable
  windowState = param.asInt();

  // Update state
  (windowState==0)?Serial.println("Window is closed"):Serial.println("Window is opened");
}

BLYNK_WRITE(V3)
{
  // Set incoming value from pin V0 to a variable
  safeRoomState = param.asInt();

  // Update state
  (safeRoomState==0)?Serial.println("Safe Roome Door is closed"):Serial.println("Safe Room Door is opened");
}

BLYNK_WRITE(V4)
{
  // Set incoming value from pin V0 to a variable
  panicModeState = param.asInt();

  // Update state
  (panicModeState==0)?Serial.println("Panic Mode is turned off"):Serial.println("Panic Mode is turned on");
}

BLYNK_WRITE(V5)
{
  // Set incoming value from pin V0 to a variable
  secureModeState = param.asInt();

  // Update state
  (secureModeState==0)?Serial.println("Secure Mode is turned off"):Serial.println("Secure Mode is turned on");
}


// // This function is called every time the device is connected to the Blynk.Cloud
// BLYNK_CONNECTED()
// {
//   // Change Web Link Button message to "Congratulations!"
//   Blynk.setProperty(V9, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
// }

// // This function sends Arduino's uptime every second to Virtual Pin 2.
// void myTimerEvent()
// {
//   // You can send any value at any time.
//   // Please don't send more that 10 values per second.
//   int value= millis() / 1000;
//   Blynk.virtualWrite(V2, value);
//   if(value%10==0){
//     Blynk.virtualWrite(V0, 0);
//   }
// }

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
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
  if (key){
    Serial.print("Key Pressed : ");
    Serial.println(key);
    password+=key; 
    Blynk.virtualWrite(V9,password);
    Blynk.logEvent("keypad_press",String("Key Pressed: ")+key);
  }  
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
