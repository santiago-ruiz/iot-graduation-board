#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 14, d4 = 33, d5 = 25, d6 = 26, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Replace the next variables with your SSID/Password combination
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "EC2-PUBLIC-DOMAIN";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
unsigned long last_millis = 0;

void setup() {
  delay(500);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  lcd.begin(20, 4);
  lcd.clear();
  lcd.home();
  lcd.print("Escanea el QR y ");
  lcd.setCursor(0,1);
  lcd.print("mandame un mensaje!");
  lcd.setCursor(0,3);
  lcd.print("Powered by AWS");
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("De: ");
  
  int i;

  for(i=9; message[i] != '"';i++){
    Serial.print((char)message[i]);
    lcd.print((char)message[i]);
  }
  lcd.setCursor(0, 1);
  lcd.print("Msg: ");
  int msg_length = 0;
  for(i+=9; message[i] != '"';i++,msg_length++){
    Serial.print((char)message[i]);
    if(msg_length == 15)
      lcd.setCursor(0, 2);
    if(msg_length == 35)
      lcd.setCursor(0, 3);
    lcd.print((char)message[i]);
  }
  
  Serial.print("-----------");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if(WiFi.status() != WL_CONNECTED){
      setup_wifi();
    }
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("cartel/msg");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
  
void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  if(millis() - last_millis > 5000){
    client.publish("cartel/keep_alive", "true");
    last_millis = millis();
  }
  
  
  client.loop();
}
