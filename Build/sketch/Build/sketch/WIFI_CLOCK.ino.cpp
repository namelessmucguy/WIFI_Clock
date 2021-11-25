#line 1 "c:\\Users\\earth\\Documents\\Arduino\\WIFI_CLOCK\\WIFI_CLOCK.ino"
#line 1 "c:\\Users\\earth\\Documents\\Arduino\\WIFI_CLOCK\\WIFI_CLOCK.ino"
/****************************************************************************************************

  本项目调用和风天气API    开发文档 https://dev.qweather.com/docs/api/weather/
  UserKey    私钥 https://dev.heweather.com/docs/start/get-api-key
  Location  城市代码 https://github.com/heweather/LocationList,表中的 Location_ID

  默认 IP 地址 192.168.4.1

  WiFi.softAP(ssid, psk, channel, hidden, max_connection)
    ssid - 包含网络 SSID 的字符串（最多 32 个字符）
    psk- 带有预共享密钥的可选字符串。对于 WPA2-PSK 网络，它的长度应至少为 8 个字符且不超过 64 个字符。如果未指定，接入点将开放供任何人连接。
    channel - 设置 Wi-Fi 信道的可选参数，从 1 到 13。默认信道 = 1。
    hidden- 可选参数，如果设置为true将隐藏 SSID。
    max_connection- 用于设置最大同时连接站数的可选参数，从 0 到 8。默认为 4。一旦达到最大数量，任何其他想要连接的站都将被迫等待，直到已连接的站断开连接。

  最后更新时间：2021-10-4
  qq:1538196442
**********************************************************************************************************/

#include <ESP8266_Heweather.h>
#include <Ticker.h>
#include <Arduino.h>
/*#include <ESP8266WiFiMulti.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>*/
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <EEPROM.h>
#include "image.h"


unsigned int UDPPort = 8266;      // local port to listen on
bool wifioff = true;
bool wifion = false;
char ssid[255];
char password[255];
char UserKey[255];
char Location[255];

char *chr_time = "00:00:00";
//char str_time[61];
char LastUpdate[11];
String format_time = "00:00:00";
char *str_Connecting = "Loading!";
char *str_Successful = "Success!";
int Temp;
int Humidity;
int Icon;
String WeatherText;

String Unit = "m";             // 公制-m/英制-i
String Lang = "en";            // 语言 英文-en/中文-zh
int count = 0;
int Click_count = 0;
char Click_flag = 0;
char flag = 0;
char flag_wifi = 0;
char flag_wifi_1 = 0;
int i = 0;
char packetBuffer[255];
char ssid_eeprom[255];
char password_eeprom[255];
char userkey_eeprom[255];
char city_eeprom[255];
char WiFi_addr[20];

uint8_t din   = 16; // DA
uint8_t clk   = 5; // CK6
uint8_t cs    = 4; // CS
uint8_t Reset = 0; // RS
uint8_t en    = 2; // EN

ICACHE_RAM_ATTR void Reset_eeprom_Falling();//必须加上“ICACHE_RAM_ATTR”申明
ICACHE_RAM_ATTR void Reset_eeprom_Rising();

char  ReplyBuffer[] = "UP";       // a string to send back
WeatherNow weatherNow;
Ticker myTicker; //建立定时调度的对象
WiFiUDP Udp;
WiFiUDP ntpUDP;
/*ESP8266WiFiMulti WiFiMulti;
  WiFiClient client;
  HTTPClient http;*/
SSD1306Wire display(0x3c, 12, 14);
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 60 * 60 * 8 + 1, 30 * 60 * 1000);



void drawQRimage() {
  display.clear();
  display.drawXbm(64, 0, QR_code_width, QR_code_height, QR_code_bits);
  display.drawXbm(0, 0, WX_Icon_width, WX_Icon_height, WX_Icon_bits);
  display.display();
}

void Icon_sclected() {
  switch (Icon)
  {
    case 100:
      display.drawXbm(0, 0, 50, 50, col_100);
      break;
    case 101:
      display.drawXbm(0, 0, 50, 50, col_101);
      break;
    case 102:
      display.drawXbm(0, 0, 50, 50, col_102);
      break;
    case 103:
      display.drawXbm(0, 0, 50, 50, col_103);
      break;
    case 104:
      display.drawXbm(0, 0, 50, 50, col_104);
      break;
    case 301:
      display.drawXbm(0, 0, 50, 50, col_301);
      break;
    case 302:
      display.drawXbm(0, 0, 50, 50, col_302);
      break;
    case 306:
      display.drawXbm(0, 0, 50, 50, col_306);
      break;
    case 307:
      display.drawXbm(0, 0, 50, 50, col_307);
      break;
    case 310:
      display.drawXbm(0, 0, 50, 50, col_310);
      break;
    case 399:
      display.drawXbm(0, 0, 50, 50, col_399);
      break;
    case 400:
      display.drawXbm(0, 0, 50, 50, col_400);
      break;
    case 401:
      display.drawXbm(0, 0, 50, 50, col_401);
      break;
    case 403:
      display.drawXbm(0, 0, 50, 50, col_403);
      break;
    case 404:
      display.drawXbm(0, 0, 50, 50, col_404);
      break;
    case 407:
      display.drawXbm(0, 0, 50, 50, col_407);
      break;
    case 499:
      display.drawXbm(0, 0, 50, 50, col_499);
      break;
    case 500:
      display.drawXbm(0, 0, 50, 50, col_500);
      break;
    case 503:
      display.drawXbm(0, 0, 50, 50, col_503);
      break;
    case 509:
      display.drawXbm(0, 0, 50, 50, col_509);
      break;
    case 999:
      display.drawXbm(0, 0, 50, 50, col_999);
      break;
    case 154:
      display.drawXbm(0, 0, 50, 50, col_154);
      break;
    case 150:
      display.drawXbm(0, 0, 50, 50, col_150);
      break;
    case 153:
      display.drawXbm(0, 0, 50, 50, col_153);
      break;
    case 303:
      display.drawXbm(0, 0, 50, 50, col_303);
      break;
    case 304:
      display.drawXbm(0, 0, 50, 50, col_304);
      break;
    case 305:
      display.drawXbm(0, 0, 50, 50, col_305);
      break;
    case 308:
      display.drawXbm(0, 0, 50, 50, col_308);
      break;
    case 309:
      display.drawXbm(0, 0, 50, 50, col_309);
      break;
    case 311:
      display.drawXbm(0, 0, 50, 50, col_311);
      break;
    case 312:
      display.drawXbm(0, 0, 50, 50, col_312);
      break;
    case 313:
      display.drawXbm(0, 0, 50, 50, col_313);
      break;
    case 314:
      display.drawXbm(0, 0, 50, 50, col_314);
      break;
    case 315:
      display.drawXbm(0, 0, 50, 50, col_315);
      break;
    case 316:
      display.drawXbm(0, 0, 50, 50, col_316);
      break;
    case 317:
      display.drawXbm(0, 0, 50, 50, col_317);
      break;
    case 318:
      display.drawXbm(0, 0, 50, 50, col_318);
      break;
    case 350:
      display.drawXbm(0, 0, 50, 50, col_350);
      break;
    case 351:
      display.drawXbm(0, 0, 50, 50, col_351);
      break;
    case 402:
      display.drawXbm(0, 0, 50, 50, col_402);
      break;
    case 405:
      display.drawXbm(0, 0, 50, 50, col_405);
      break;
    case 406:
      display.drawXbm(0, 0, 50, 50, col_406);
      break;
    case 408:
      display.drawXbm(0, 0, 50, 50, col_408);
      break;
    case 409:
      display.drawXbm(0, 0, 50, 50, col_409);
      break;
    case 410:
      display.drawXbm(0, 0, 50, 50, col_410);
      break;
    case 456:
      display.drawXbm(0, 0, 50, 50, col_456);
      break;
    case 457:
      display.drawXbm(0, 0, 50, 50, col_457);
      break;
    case 501:
      display.drawXbm(0, 0, 50, 50, col_501);
      break;
    case 502:
      display.drawXbm(0, 0, 50, 50, col_502);
      break;
    case 504:
      display.drawXbm(0, 0, 50, 50, col_504);
      break;
    case 507:
      display.drawXbm(0, 0, 50, 50, col_507);
      break;
    case 508:
      display.drawXbm(0, 0, 50, 50, col_508);
      break;
    case 510:
      display.drawXbm(0, 0, 50, 50, col_510);
      break;
    case 511:
      display.drawXbm(0, 0, 50, 50, col_511);
      break;
    case 512:
      display.drawXbm(0, 0, 50, 50, col_512);
      break;
    case 513:
      display.drawXbm(0, 0, 50, 50, col_513);
      break;
    case 514:
      display.drawXbm(0, 0, 50, 50, col_514);
      break;
    case 515:
      display.drawXbm(0, 0, 50, 50, col_515);
      break;
    case 900:
      display.drawXbm(0, 0, 50, 50, col_900);
      break;
    case 901:
      display.drawXbm(0, 0, 50, 50, col_901);
      break;
    case 300:
      display.drawXbm(0, 0, 50, 50, col_300);
      break;

    default:
      break;
  }
}

void drawFace() {
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  //display.drawXbm(60, 0, Temp_Icon_width, Temp_Icon_height, Temp_Icon_bits);
  display.drawString(60, 0, String(Temp) + "°" + "C");
  //display.drawXbm(60, 28, Humidity_Icon_width, Humidity_Icon_height, Humidity_Icon_bits);
  display.drawString(60, 22, String(Humidity) + "%");
  display.setFont(ArialMT_Plain_10);
  display.drawString(62, 44, String(LastUpdate));
  display.drawString(0, 51, String(WeatherText));
  display.display();
}

void judge() {
  for (int d = 0; d < 256; d++ ) {
    ssid_eeprom[d] = EEPROM.read(d);
    password_eeprom[d] = EEPROM.read(d + 256);
    userkey_eeprom[d] = EEPROM.read(d + 512);
    city_eeprom[d] = EEPROM.read(d + 768);
  }
  for (int d = 0; d < 1025; d++ ) {
    EEPROM.read(d);
    if (EEPROM.read(d) != 0)
    {
      for (int i = 0; i < 256; i++ ) {
        ssid[i] = ssid_eeprom[i];
        password[i] = password_eeprom[i];
      }
      weatherNow.config(userkey_eeprom, city_eeprom, Unit, Lang); // 配置请求信息
      ConnectWiFi();
      d = 1026;
    }
    else
    {
      flag = 0;
      drawQRimage();
    }
  }
}

void ConnectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  S1201_WriteStr(0, str_Connecting);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(2, 0, "Connecting");
  display.drawString(45, 20, "To");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 40, String(ssid));
  display.display();
  int a = 0;
  while (WiFi.status() != WL_CONNECTED && (a <= 30)) {
    delay(1000);
    Serial.print('.');
    a++;
  }
  if (a >= 31) {    // 30s失败
    Serial.println("WiFi Connection Failed");
    flag_wifi = 0;
    flag_wifi_1 = 0;
    i = 0;
  } else {      // 成功
    WiFi.softAPdisconnect(wifioff);//设置为true，关闭AP模式,即esp8266不发出wifi热点
    Serial.println("");
    Serial.println("WiFi Connection Successful!");
    Serial.print("IP address:   ");
    Serial.println(WiFi.localIP());
    if ((packetBuffer[0] == 'c') && (packetBuffer[1] == 'o') && (packetBuffer[2] == 'n') && (packetBuffer[3] == 'n') && (packetBuffer[4] == 'e') && (packetBuffer[5] == 'c') && (packetBuffer[6] == 't')) EEPROM.commit(); //写入EEPROM
    S1201_WriteStr(0, str_Successful);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "WiFi Connection Successful");
    display.drawString(0, 16, "IP address:");
    display.drawString(0, 32, WiFi.localIP().toString());
    display.drawString(0, 48, "Product made by BigSugar");
    display.display();
    delay(1000);
    display.clear();
    if (weatherNow.get()) { // 获取天气更新
      Serial.println(F("======Weahter Now Info======"));
      Serial.print("Server Response: ");
      Serial.println(weatherNow.getServerCode());  // 获取API状态码
      Serial.print(F("Last Update: "));
      Serial.println(weatherNow.getLastUpdate());  // 获取服务器更新天气信息时间
      weatherNow.getLastUpdate().toCharArray(LastUpdate, 11);
      Serial.print(F("Temperature: "));
      Serial.println(weatherNow.getTemp());        // 获取实况温度
      Temp = weatherNow.getTemp();
      WeatherText = weatherNow.getWeatherText();
      Serial.print(F("FeelsLike: "));
      Serial.println(weatherNow.getFeelLike());    // 获取实况体感温度
      Serial.print(F("Icon: "));
      Serial.println(weatherNow.getIcon());        // 获取当前天气图标代码
      Icon = weatherNow.getIcon();
      Icon_sclected();
      Serial.print(F("Weather Now: "));
      Serial.println(weatherNow.getWeatherText()); // 获取实况天气状况的文字描述
      WeatherText = weatherNow.getWeatherText();
      Serial.print(F("windDir: "));
      Serial.println(weatherNow.getWindDir());     // 获取实况风向
      Serial.print(F("WindScale: "));
      Serial.println(weatherNow.getWindScale());   // 获取实况风力等级
      Serial.print(F("Humidity: "));
      Serial.println(weatherNow.getHumidity());    // 获取实况相对湿度百分比数值
      Humidity = weatherNow.getHumidity();
      Serial.print(F("Precip: "));
      Serial.println(weatherNow.getPrecip());      // 获取实况降水量,毫米
      Serial.println(F("========================"));
      drawFace();
      flag = 1;
    }
  }
}


/**********************VFD屏驱动*******************************/
void write_6302(unsigned char w_data)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(clk, LOW);
    if ( (w_data & 0x01) == 0x01)
    {
      digitalWrite(din, HIGH);
    }
    else
    {
      digitalWrite(din, LOW);
    }
    w_data >>= 1;
    digitalWrite(clk, HIGH);
  }
}

void VFD_cmd(unsigned char command)
{
  digitalWrite(cs, LOW);
  write_6302(command);
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
}

void S1201_show(void)
{
  digitalWrite(cs, LOW);//开始传输
  write_6302(0xe8);     //地址寄存器起始位置
  digitalWrite(cs, HIGH); //停止传输
}

void VFD_init()
{
  //SET HOW MANY digtal numbers
  digitalWrite(cs, LOW);
  write_6302(0xe0);
  delayMicroseconds(5);
  write_6302(0x07);//8 digtal
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);

  //set bright
  digitalWrite(cs, LOW);
  write_6302(0xe4);
  delayMicroseconds(5);
  write_6302(0xff);//leve 255 max
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
}

/******************************
  在指定位置打印一个字符(用户自定义,所有CG-ROM中的)
  x:0~11;chr:要显示的字符编码
*******************************/
void S1201_WriteOneChar(unsigned char x, unsigned char chr)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  write_6302(chr );
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();
}
/******************************
  在指定位置打印字符串
  (仅适用于英文,标点,数字)
  x:0~11;str:要显示的字符串
*******************************/
void S1201_WriteStr(unsigned char x, char *str)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  while (*str)
  {
    write_6302(*str); //ascii与对应字符表转换
    str++;
  }
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();
}

/*void VFD_Show(){
  S1201_WriteOneChar(0,str_time[54]);
  S1201_WriteOneChar(1,str_time[55]);
  S1201_WriteStr(2,":");
  S1201_WriteOneChar(3,str_time[56]);
  S1201_WriteOneChar(4,str_time[57]);
  S1201_WriteStr(5,":");
  S1201_WriteOneChar(6,str_time[58]);
  S1201_WriteOneChar(7,str_time[59]);
  }*/
/****************************************************************************/
void Get_Time() {
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  format_time = timeClient.getFormattedTime();
  char *chr_time = &format_time[0];
  S1201_WriteStr(0, chr_time);
  /* if (http.begin(client, "http://quan.suning.com/getSysTime.do")) {  // HTTP
    int httpCode = http.GET();
    if (httpCode > 0) {
     // Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
        payload.toCharArray(str_time, 62);
        VFD_Show();
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    } else {
    Serial.printf("[HTTP} Unable to connect\n");
    }*/
}
/*****************************************************************************/
void Reset_eeprom_Falling()
{
  Click_flag = 1;
  Serial.println("pressed");
  attachInterrupt(digitalPinToInterrupt(13), Reset_eeprom_Rising, RISING);
}

void Reset_eeprom_Rising() {
  Click_flag = 0;
  Serial.println("released");
  attachInterrupt(digitalPinToInterrupt(13), Reset_eeprom_Falling, FALLING);
}

/*****************************************************************************/

void tickerHandle()     //一秒定时器，标志位计数
{
  count++;
  if (count == 601) {
    count = 0;
  }
  if (Click_flag == 1)
  {
    Click_count++;
  }
  if ((Click_count == 5) && (digitalRead(13) == 1))    //按下五秒时判断引脚电平 即确认按键是否仍然被摁下
  {
    for (int d = 0; d < 1025; d++ ) { //清除EEPROM上 0~1024 的数据
      EEPROM.write(d, 0);
    }
    EEPROM.commit();
    Serial.println("EEPROM Is Clear!");
    Click_count = 0;
    ESP.restart();
  }

}

void setup() {

  Serial.begin(115200);

  WiFi.softAPdisconnect(wifion);
  WiFi.softAP("ESP8266", "", 1, false, 1); //wifi名为“ESP8266”  无密码  信道1  不隐藏wifi  最大连接数1

  EEPROM.begin(1025);                  //开启EEPROM，开辟(1025-1)字节空间

  Udp.begin(UDPPort);

  timeClient.begin();

  Serial.println();

  Serial.println("Started ap. Local ip: " + WiFi.localIP().toString());

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  pinMode(13, INPUT);

  attachInterrupt(digitalPinToInterrupt(13), Reset_eeprom_Falling, FALLING);

  myTicker.attach(1, tickerHandle); //初始化调度任务，每1秒执行一次tickerHandle()

  pinMode(en, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(din, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(Reset, OUTPUT);

  digitalWrite(en, HIGH);
  digitalWrite(Reset, LOW);
  delayMicroseconds(5);
  digitalWrite(Reset, HIGH);
  VFD_init();
  judge();
}



void loop() {
  if (flag == 1) {
    Get_Time();
    delay(1000);
    if (count == 600) {   //十分钟更新一次天气
      display.clear();
      if (weatherNow.get()) { // 获取天气更新
        Serial.println(F("======Weahter Now Info======"));
        Serial.print("Server Response: ");
        Serial.println(weatherNow.getServerCode());  // 获取API状态码
        Serial.print(F("Last Update: "));
        Serial.println(weatherNow.getLastUpdate());  // 获取服务器更新天气信息时间
        weatherNow.getLastUpdate().toCharArray(LastUpdate, 11);
        Serial.print(F("Temperature: "));
        Serial.println(weatherNow.getTemp());        // 获取实况温度
        Temp = weatherNow.getTemp();
        Serial.print(F("FeelsLike: "));
        Serial.println(weatherNow.getFeelLike());    // 获取实况体感温度
        Serial.print(F("Icon: "));
        Serial.println(weatherNow.getIcon());        // 获取当前天气图标代码
        Icon = weatherNow.getIcon();
        Icon_sclected();
        Serial.print(F("Weather Now: "));
        Serial.println(weatherNow.getWeatherText()); // 获取实况天气状况的文字描述
        WeatherText = weatherNow.getWeatherText();
        Serial.print(F("windDir: "));
        Serial.println(weatherNow.getWindDir());     // 获取实况风向
        Serial.print(F("WindScale: "));
        Serial.println(weatherNow.getWindScale());   // 获取实况风力等级
        Serial.print(F("Humidity: "));
        Serial.println(weatherNow.getHumidity());    // 获取实况相对湿度百分比数值
        Humidity = weatherNow.getHumidity();
        Serial.print(F("Precip: "));
        Serial.println(weatherNow.getPrecip());      // 获取实况降水量,毫米
        Serial.println(F("========================"));
        drawFace();
      } else {    // 更新失败
        Serial.println("Update Failed...");
        Serial.print("Server Response: ");
        Serial.println(weatherNow.getServerCode());
        delay(1000);
      }
    }
  }


  // if there's data available, read a packet

  int packetSize = Udp.parsePacket();

  if (packetSize) {

    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);

    if (len > 0) {
      packetBuffer[len] = 0;
    }

    Serial.println("Contents:");
    Serial.println(packetBuffer);

    while (i < packetSize)
    {
      if (packetBuffer[i] == ' ') {
        break;
      }
      i++;
    }

    if (i >= packetSize)
    {
      Serial.println("格式错误:WIFI名称与密码之间未输入空格");
    }

    if ((i < packetSize) && (flag_wifi == 0)) //first accept (ssid and password)
    {
      Serial.println("position:");
      Serial.println(i + 1);
      strncpy(ssid, packetBuffer, i);
      strncpy(password, packetBuffer + (i + 1), packetSize - (i + 1));
      Serial.println(ssid);
      Serial.println(password);

      for (int d = 0; d < 256; d++ ) {
        EEPROM.write(d, ssid[d]);
        EEPROM.write(d + 256, password[d]);
      }

      flag_wifi = 1;
      i = 0;
    }
    if ((i < packetSize) && (flag_wifi == 1 ) && (i > 0)) //second accept (UserKey and city)
    {
      strncpy(UserKey, packetBuffer, i);
      strncpy(Location, packetBuffer + (i + 1), packetSize - (i + 1));
      Serial.println(UserKey);
      Serial.println(Location);
      weatherNow.config(UserKey, Location, Unit, Lang); // 配置请求信息

      for (int d = 0; d < 256; d++ ) {
        EEPROM.write(d + 512, UserKey[d]);
        EEPROM.write(d + 768, Location[d]);
      }

      flag_wifi_1 = 1;
      i = 0;
    }
    if ((i > 0) && (flag_wifi_1 == 1) && (packetBuffer[0] == 'c') && (packetBuffer[1] == 'o') && (packetBuffer[2] == 'n') && (packetBuffer[3] == 'n') && (packetBuffer[4] == 'e') && (packetBuffer[5] == 'c') && (packetBuffer[6] == 't'))
    {
      ConnectWiFi();
    }
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());

    Udp.write(ReplyBuffer);

    Udp.endPacket();

  }
}

