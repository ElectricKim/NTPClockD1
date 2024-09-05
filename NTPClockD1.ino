#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TM1637Display.h>

extern const char* ssid;
extern const char* password;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "kr.pool.ntp.org", 9 * 3600); // UTC+9
TM1637Display led_display(D1, D2);

unsigned long timePrevMillis = 0;
unsigned long displayPrevMillis = 0;
boolean colonState = false;

void setup() {
  Serial.begin(115200);
  led_display.setBrightness(0);

  // "Conn" 메시지 출력
  uint8_t connPattern[] = { 0x39, 0x5C, 0x54, 0x54 };
  led_display.setSegments(connPattern); 
  
  // d1 mini 보드 후면 led 끄기
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Serial.println();
  // Serial.print("WiFi에 연결 중 ");
  // Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Serial.println("");
  // Serial.println("WiFi에 연결되었습니다");
  // Serial.println("IP 주소: ");
  // Serial.println(WiFi.localIP());
  
  delay(500);
  timeClient.begin();
  timeClient.update();
}

void loop() {
  unsigned long currentMillis = millis();

  // NTP 시간 업데이트
  if (currentMillis - timePrevMillis >= 1000*60*60) { // 1시간마다 업데이트
    timePrevMillis = currentMillis;
    timeClient.update();
  }

  // 디스플레이 업데이트
  if (currentMillis - displayPrevMillis >= 500) { // 500ms마다 업데이트
    displayPrevMillis = currentMillis;
    unsigned long epochTime = timeClient.getEpochTime();
    int hours = (epochTime % 86400L) / 3600;
    int minutes = (epochTime % 3600) / 60;
    colonState = !colonState;
    led_display.showNumberDecEx(hours * 100 + minutes, (colonState ? 0x40 : 0), true);
  }
}
