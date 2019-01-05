#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>

#define NUM_LEDS1 144
#define NUM_LEDS2 64
CRGBArray<NUM_LEDS1> leds1;
CRGBArray<NUM_LEDS2> leds2;
bool LEDsMoving = true;
bool fade = true;

const char* ssid     = "RGBBlinkieLights";
const char* password = "123456789";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client,
               AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Websocket client connection received");
    client->text("Hello from ESP32 Server");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
  } else if (data[0] == 'a' && len <= (NUM_LEDS1 * 3) + 1) {
    for (int i = 0; i < (NUM_LEDS1 * 3); i++) {
      leds1[i / 3][i % 3] = data[i + 1];
    }
  } else if (data[0] == 'b' && len <= (NUM_LEDS2 * 3) + 1) {
    for (int i = 0; i < (NUM_LEDS2 * 3); i++) {
      leds2[i / 3][i % 3] = data[i + 1];
    }
  } else if (data[0] == 'T') {
    client->text("Time");
  }else if (data[0] == 'F') {
    fade = data[1];
  } else if (data[0] == 'D') {
    FastLED.setDither(data[1]);
  } else if (data[0] == 'B') {
    FastLED.setBrightness(data[1]);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);//Its own WiFi Network (better performance)
  //WiFi.begin("SSID", "Password");//uncomment, if you want to connect to a WiFi Network

  /*while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    }*/

  //Serial.println(WiFi.localIP());
  //Serial.println(WiFi.softAPIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();

  FastLED.addLeds<WS2812B, 13, GRB>(leds1, NUM_LEDS1);
  FastLED.addLeds<WS2812B, 15, GRB>(leds2, NUM_LEDS2);
  FastLED.setBrightness(100);
  FastLED.setMaxPowerInMilliWatts(1000);
}

void loop() {
  FastLED.show();
  if (fade) {
    leds1.fadeToBlackBy(1);
    leds2.fadeToBlackBy(1);
  }
}
