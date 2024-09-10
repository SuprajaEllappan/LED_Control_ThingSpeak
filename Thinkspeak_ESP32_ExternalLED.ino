#include <WiFi.h>
#include <ArduinoJson.h>

WiFiClient client;
const int ledPin = 4; // GPIO 2 is usually the built-in LED on the ESP32
int Value;
// Function to get data from ThingSpeak
String ThingSpeakGet(String IPcache, String monmessagecache, String field) {
  // Use DynamicJsonDocument instead of DynamicJsonBuffer
  DynamicJsonDocument jsonDoc(3000);
  String line = "";
  String myurl = "/";
  
  // Send the HTTP GET request
  client.print(String("GET ") + myurl + monmessagecache + " HTTP/1.1\r\n" +
               "Host: " + IPcache + "\r\n" +
               "Connection: close\r\n\r\n");
               
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      return "Client Timeout!";
    }
  }

  // Read the response
  while (client.available()) {
    line += client.readStringUntil('\r');
  }

  // Process the request
  line = (line.substring((line.indexOf("{"))));
  DeserializationError error = deserializeJson(jsonDoc, line);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "Error parsing JSON";
  }

  // Extract the value
  String result = jsonDoc["feeds"][0][("field" + field).c_str()].as<String>();
  return result;
}

void setup() {
  Serial.begin(115200); // Increased baud rate for faster serial communication
   pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
  WiFi.begin("Praveen act", "slact7204813084");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print("..");
  }
  Serial.println("\nConnected");
  Serial.println("Your IP is");
  Serial.println(WiFi.localIP());
  
}
  
void loop() {
  if (client.connect("api.thingspeak.com", 80)) {
    Serial.println("Fetching value from ThingSpeak...");
    
    // Get the latest value for field 1
    String field1Value = ThingSpeakGet("api.thingspeak.com", "channels/2621893/fields/1.json?api_key=XN1OYNIO6RDOEY8I&results=1", "1");
    Value = field1Value.toInt();
    // Print the value
    Serial.print("Field 1 Value: ");
    Serial.println(field1Value);
    // Optionally, you can control the LED or take other actions here
    // controlLED(field1Value.toFloat());  // Uncomment if using LED control
    blinkLED();

  } else {
    Serial.println("Unable to connect to the site.");
  }
 
    // Wait for a while before the next request
  delay(10000);  // Adjust the delay as needed 
}
void blinkLED() {
  if (Value % 2 != 0) {
      Serial.println("Number is odd, blinking LED.");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("Number is even, LED stays off.");
      digitalWrite(ledPin, LOW);
    }
  
}
