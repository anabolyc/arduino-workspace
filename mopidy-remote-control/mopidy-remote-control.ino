#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <IRremoteESP8266.h>
#include "Buttons.h"
#include "wificreditnails.h"
#include "httpresult.h"

#define CLIENT_TIMEOUT 10000
#define CLIENT_REPLY_LEN  32
#define MOPIDY_HOST    "192.168.1.60"
#define MOPIDY_PORT    6680

#define LEDS_COUNT     3
#define PIN_LED_RED   13 //D7
#define PIN_LED_GREEN 12 //D6
#define PIN_LED_BLUE  16 //D0

#define VOLUME_STEP    3

const int RECV_PIN = 14;

IRrecv irrecv(RECV_PIN);
decode_results results;

int leds[LEDS_COUNT] = {
  PIN_LED_RED,
  PIN_LED_GREEN,
  PIN_LED_BLUE
};

char* out;

void setup()
{
  out = (char*)malloc(sizeof(char) * CLIENT_REPLY_LEN);

  for (int i = 0; i < LEDS_COUNT; i++)
    pinMode(leds[i], OUTPUT);

  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("IR started");

  Serial.print("Wifi connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
}

bool wifi_displayed = 0;
bool wifi_led_state = 0;

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    wifi_led_state = !wifi_led_state;
    digitalWrite(PIN_LED_RED, wifi_led_state);
    digitalWrite(PIN_LED_BLUE, wifi_led_state);
    Serial.print(".");
    delay(500);
  } else {
    for (int i = 0; i < LEDS_COUNT; i++)
      digitalWrite(leds[i], LOW);

    if (!wifi_displayed) {
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      wifi_displayed  = true;
    }

    if (irrecv.decode(&results)) {
      Serial.print("IR says: ");
      Serial.println(results.value, HEX);

      RequestBind _req;
      RequestBind* req = NULL;
      switch (results.value) {
        case BTN_A:
          digitalWrite(PIN_LED_BLUE, HIGH);
          if (doRequest(&CMD_GET_STATE, MOPIDY_HOST, MOPIDY_PORT, out) == CONN_OK) {
            digitalWrite(PIN_LED_GREEN, HIGH);
            if (strcmp(out, REPLY_PLAYING) == 0)
              req = &CMD_PAUSE;
            else
              req = &CMD_PLAY;
          } else {
            digitalWrite(PIN_LED_RED, HIGH);
            Serial.println("Failed to get state :(");
            req = &CMD_PLAY;
          }
          digitalWrite(PIN_LED_BLUE, LOW);
          break;
        case BTN_B:
          req = &CMD_STOP;
          break;
        case BTN_C:
        case BTN_D:
          digitalWrite(PIN_LED_BLUE, HIGH);
          if (doRequest(&CMD_GET_VOLUME, MOPIDY_HOST, MOPIDY_PORT, out) == CONN_OK) {
            digitalWrite(PIN_LED_GREEN, HIGH);
            int volume = atoi(out);
            if ((volume == 0 && results.value == BTN_C) || (volume == 100 && results.value == BTN_D)) {
              Serial.println("Volume already at limit, skipping");
            }
            else
            {
              Serial.print("Current volume: ");
              Serial.println(volume);
              if (results.value == BTN_C)
                volume -= VOLUME_STEP;
              if (results.value == BTN_D)
                volume += VOLUME_STEP;
              _req = CMD_SET_VOLUME(volume);
              Serial.print(" *** ");
              Serial.println(volume);
              Serial.print(" *** ");
              Serial.println(_req.payload);
              req = &_req;
            }
          } else {
            digitalWrite(PIN_LED_RED, HIGH);
            Serial.println("Failed to get volume :(");
          }
          digitalWrite(PIN_LED_BLUE, LOW);
          break;
      }

      if (req != NULL) {
        digitalWrite(PIN_LED_BLUE, HIGH);
        int req_result = doRequest(req, MOPIDY_HOST, MOPIDY_PORT, out);

        switch (req_result) {
          case CONN_OK:
            digitalWrite(PIN_LED_BLUE, LOW);
            digitalWrite(PIN_LED_GREEN, HIGH);
            break;
          case CONN_FAILED:
          case CONN_TIMEOUT:
            Serial.println("*** failed!");
            digitalWrite(PIN_LED_BLUE, LOW);
            digitalWrite(PIN_LED_RED, HIGH);
            delay(500);
            break;
        }
        Serial.print("execution result: ");
        Serial.println(out);
      }

      irrecv.resume(); // Receive the next value
    }
    delay(100);
  }
}

const char* RESULT_MARK = "result\": ";

int doRequest(const RequestBind * request, char* host, const int port, char* out) {
  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return CONN_FAILED;
  }

  Serial.print("Requesting URL: ");
  Serial.println(request->path);

  client.print(request->method);
  client.print(" ");
  client.print(request->path);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.print(host);
  client.print(":");
  client.println(port);
  client.println("Connection: close");
  if (strcmp(request->method, "POST") == 0) {
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(strlen(request->payload));
    client.println();
    client.println(request->payload);
  }

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > CLIENT_TIMEOUT) {
      client.stop();
      return CONN_TIMEOUT;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (line.indexOf("jsonrpc") != -1) {
      int a = line.lastIndexOf(RESULT_MARK);
      if (a != -1) {
        int b = line.lastIndexOf("}");
        if (b != -1) {
          strcpy(out, line.substring(a + strlen(RESULT_MARK), b).c_str());
        } else
          out = "";
      } else
        out = "";
    }
    // Serial.print(line);
  }

  Serial.println();
  return CONN_OK;
}
