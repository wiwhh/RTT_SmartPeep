
#include "esp_camera.h"
#include <WiFi.h>
 
WiFiUDP Udp;
#define Port 1024
 
#define maxData 1024
 
#define index(a, b) a.substring(0, b.length()) == b
#define afterStr(a, b) a.substring(b.length())
#define stoi(a) atoi(a.c_str())
#define minimum(a, b) (((a) < (b)) ? (a) : (b))
 
const char *ssid = "OpenWrt";     //wifi名称
const char *password = "541956555";  //wifi密码

IPAddress staticIP(192, 168, 15, 100); // 静态IP地址
IPAddress gateway(192, 168, 15, 1);    // 网关地址
IPAddress subnet(255, 255, 255, 0);   // 子网掩码
IPAddress primaryDNS(8, 8, 8, 8);      // DNS服务器地址
IPAddress secondaryDNS(8, 8, 4, 4);    // 备用DNS服务器地址

// 定义目标 IP 地址和端口
IPAddress targetIP(192,168,15,249); // 替换为目标 IP 地址
const int targetPort = 2345;          // 替换为目标端口号
 
//======== esp32-cam摄像头配置 ========
 
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
 
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
 
static camera_config_t camera_config = {
  .pin_pwdn = PWDN_GPIO_NUM,
  .pin_reset = RESET_GPIO_NUM,
  .pin_xclk = XCLK_GPIO_NUM,
  .pin_sscb_sda = SIOD_GPIO_NUM,
  .pin_sscb_scl = SIOC_GPIO_NUM,
 
  .pin_d7 = Y9_GPIO_NUM,
  .pin_d6 = Y8_GPIO_NUM,
  .pin_d5 = Y7_GPIO_NUM,
  .pin_d4 = Y6_GPIO_NUM,
  .pin_d3 = Y5_GPIO_NUM,
  .pin_d2 = Y4_GPIO_NUM,
  .pin_d1 = Y3_GPIO_NUM,
  .pin_d0 = Y2_GPIO_NUM,
  .pin_vsync = VSYNC_GPIO_NUM,
  .pin_href = HREF_GPIO_NUM,
  .pin_pclk = PCLK_GPIO_NUM,
 
  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,
 
  .pixel_format = PIXFORMAT_RGB565,
  .frame_size = FRAMESIZE_96X96,
  .jpeg_quality = 63,
  .fb_count = 1,
};
 
esp_err_t camera_init() {
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    Serial.print("\r\n摄像头加载失败：error:0x");
    Serial.println(err);
    return err;
  }
  sensor_t *s = esp_camera_sensor_get();
  Serial.println("\r\n摄像头加载成功");
  return ESP_OK;
}
 
bool hasfb = false;
camera_fb_t *fb;
 
//====================================
 
void wifi_init() {
  WiFi.mode(WIFI_STA);
  if (WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) 
  {
    WiFi.begin(ssid, password);
    Serial.print("\r\nConnecting to ");           // 串口监视器输出网络连接信息
    Serial.print(ssid); Serial.println(" ...");   // 显示NodeMCU正在尝试WiFi连接;
    while (WiFi.status() != WL_CONNECTED)         // WiFi.status()函数的返回值是由NodeMCU的WiFi连接状态所决定的。 
    {                                             // 如果WiFi连接成功则返回值为WL_CONNECTED
      delay(1000);                                // 此处通过While循环让NodeMCU每隔一秒钟检查一次WiFi.status()函数返回值
      Serial.print(".");                                
    }      
    Serial.println("");                           // WiFi连接成功后
    Serial.println("WiFi connected!");            // NodeMCU将通过串口监视器输出"连接成功"信息。
  }
  else
  {
    Serial.println("WiFi.config failed");
  }
  if (Udp.begin(Port)) {
    Serial.println("UDP启动成功");
    Serial.print("IP address: ");                 // 同时还将输出NodeMCU的IP地址。这一功能是通过调用
    Serial.println(WiFi.localIP());               // WiFi.localIP()函数来实现的。该函数的返回值即NodeMCU的IP地址。
    Serial.print("Port: "); 
    Serial.println(Port);
  }
}
 
int getCam() {
  if (hasfb) esp_camera_fb_return(fb);
  fb = esp_camera_fb_get();
  hasfb = true;
  if (!fb) {
    Serial.print("Camera capture failed");
    return -1;
  } else {
    Serial.print("fb->len: ");
    Serial.println(fb->len);
    return 0;
  }
}
IPAddress remoteIp;
int remotePort;
void sendDataOverUDP() {
  if (getCam() != 0) return; // 如果摄像头数据获取失败，则退出

  // 分割数据
  const int bufferSize = 1460; // 根据 MTU 调整
  int totalLength = fb->len;
  int packets = (totalLength + bufferSize - 2) / (bufferSize - 1);

  // 发送数据
  for (uint8_t i = 0; i < packets; i++) {
    int start = i * (bufferSize - 1);
    int end = min(start + bufferSize - 1, totalLength);
    int len = end - start;

    Udp.beginPacket(remoteIp, remotePort);
    Udp.write(&i, 1);
    Udp.write(&fb->buf[start], len);
    Udp.endPacket();

    Serial.printf("Sent packet %d of size %d\n", i, len);
    delay(100);
  }
}

void sendDataOverUDP1() {
  if (getCam() != 0) return; // 如果摄像头数据获取失败，则退出

  int totalLength = fb->len;
  Serial.print("totalLength ");
  Serial.println(totalLength);
  // 发送数据
  Udp.beginPacket(remoteIp, remotePort);
  Udp.write(fb->buf, totalLength);
  Udp.endPacket();

  Serial.printf("Sent packet\n");
  delay(100);
  
}

void setup() {
  Serial.begin(115200);
  while (camera_init() != ESP_OK) {
    delay(1000);
  }
  wifi_init();
}

char c;
void loop() 
{
  // 检查是否有新的数据包
  int packetSize = Udp.parsePacket();
  if (packetSize) 
  {
    Serial.print("Received packet of size ");
    Serial.print(packetSize);
    Serial.print(" from ");
    remoteIp = Udp.remoteIP();
    remotePort = Udp.remotePort();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(remotePort);
    
    // 读取数据
    while (Udp.available()) {
      c = Udp.read();
      Serial.print(c);
    }
    Serial.println();
    if(c == 0xaa)
    {
      sendDataOverUDP();
    }
    
  }
  //sendDataOverUDP();
}