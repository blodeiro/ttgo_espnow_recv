#include <PubSubClient.h>
#include <WiFiClientSecure.h>


WiFiClientSecure espClient; // for no secure connection use WiFiClient instead of WiFiClientSecure
// WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];


int command1 = 0;

const String root_topic = "beehive/";
// const char*  sensor2_topic="sensor3";

const char *command1_topic = "command1";
// const char* command1_topic="command2";


//=====================================
bool reconnect_mqtt()
{
    Serial.print("Attempting MQTT connection...");
    String clientId = "ttgo1Client";
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password))
    {
        Serial.println("connected");

        client.subscribe(command1_topic); // subscribe the topics here
        // client.subscribe(command2_topic);   // subscribe the topics here
        return true;
    }
    else
    {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds"); // Wait 5 seconds before retrying
        delay(5000);
        return false;
    }
}

//======================================= publising as string
void publishMessage(const char *topic, String payload, boolean retained)
{
    if (client.publish(topic, payload.c_str(), true))
        Serial.println("Message publised [" + String(topic) + "]: " + payload);
}

//=======================================
// This void is called every time we have a message from the broker

void callback_mqtt(char *topic, byte *payload, unsigned int length)
{
    String incommingMessage = "";
    for (int i = 0; i < length; i++)
        incommingMessage += (char)payload[i];

    Serial.println("Message arrived [" + String(topic) + "]" + incommingMessage);

    //--- check the incomming message
    //     if( strcmp(topic,command1_topic) == 0){
    //      if (incommingMessage.equals("1")) digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on
    //      else digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off
    //   }

    //  check for other commands
    /*  else  if( strcmp(topic,command2_topic) == 0){
        if (incommingMessage.equals("1")) {  } // do something else
     }
     */
}

//================================================ setup
//================================================
void setup_mqtt()
{

#ifdef ESP8266
    espClient.setInsecure();
#else // for the ESP32
    espClient.setCACert(root_ca); // enable this line and the the "certificate" code for secure connection
#endif

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback_mqtt);
}

//================================================ loop
//================================================
void loop_mqtt()
{

    if (!client.connected())
        if (!reconnect_mqtt()) return;
    client.loop();

    //---- example: how to publish sensor values every 5 sec
    unsigned long now = millis();
    if (now - lastMsg > 5000)
    {
        lastMsg = now;
        String root_id = root_topic + String(beehiveSample.board_id);
        String topic = root_id + "/interior/temp1";
        publishMessage(topic.c_str(), String(beehiveSample.temp_1), true);
        topic = root_id + "/interior/hum1";
        publishMessage(topic.c_str(), String(beehiveSample.hum_1), true);
        topic = root_id + "/interior/temp2";
        publishMessage(topic.c_str(), String(beehiveSample.temp_2), true);
        topic = root_id + "/interior/hum2";
        publishMessage(topic.c_str(), String(beehiveSample.hum_2), true);
        topic = root_id + "/exterior/temp1";
        publishMessage(topic.c_str(), String(beehiveSample.temp_ext), true);
        topic = root_id + "/exterior/hum1";
        publishMessage(topic.c_str(), String(beehiveSample.hum_ext), true);
        topic = root_id + "/exterior/pressure";
        publishMessage(topic.c_str(), String(beehiveSample.pres_2), true);
    }
}