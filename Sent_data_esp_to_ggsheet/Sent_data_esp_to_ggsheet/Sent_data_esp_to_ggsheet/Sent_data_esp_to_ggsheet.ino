//#include<ESP8266WiFi.h> // esp8266 get dữ liệu về đc
#include<WiFi.h> //esp32 ko get dc dữ liệu về 
#include "HTTPSRedirect.h"

//Khai báo thông tin về Wifi
const char* ssid = "THANH LONG";
const char* password = "0919902493thanhlong";

//Khai báo thông tin về server của google
const char *GScriptID = "AKfycbx4uN47VwlgNIrDeWmOQDhy67gQgVPw72GtnwACehT-CgJEaujt7eLdwy4jFz2hP9v4"; //Google Script ID đã publish trước đó
const char* host = "script.google.com";
const char *googleRedirectHost = "script.googleusercontent.com";
const int httpsPort = 443;

HTTPSRedirect client(httpsPort);

//Khai báo URL chứa thông tin gủi dữ liệu
String url = String("/macros/s/") + GScriptID + "/exec?";

//Khai báo biến chứa thời gian lặp để gửi dữ liệu vào google sheet (ở đây biến tính bằng mili giay)
unsigned long preMillis = 0;
const long interval = 6000; // ví dụ mình post 1 phút 1 lần sẽ bằng 1 * 60 * 1000 = 60000
String dataget = "";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password); //Kết nói wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print(" IP Address: ");
  Serial.println(WiFi.localIP());

  //Do sử dụng HTTPS nên mình sẽ tạo 1 TLS connection
  client.setInsecure();
  client.setPrintResponseBody(false);
  client.setContentTypeHeader("application/json");
  Serial.print(String("Kết nối tới "));
  Serial.println(host); //Hiển thị thông tin kết nối

  //Lấy thông tin trạng thái kết nối với server của google
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true; //nếu kết nối thành công sẽ trả về trạng thái
      break;
    } else {
      Serial.println("Kết nối thất bại. Đang thử kết nối lại ....");
    }
  }

  Serial.println("Trạng thái kết nối là: " + String(client.connected()));//1 là thành công, 0 là thất bại
  if (!flag) {
    Serial.print("Không thể kết nối tới server");
    Serial.println(host);
    Serial.println("Thoát....");
    Serial.flush();
    return;
  }
}

//Khai báo hàm getData
String getData() {
  String str = "GET: ";
  client.GET(url, host); //Thực hiện gửi lệnh GET
  str = str + client.getResponseBody(); //Nhận giá trị trả về từ hàm GET
  return str;
}

//Khai báo hàm postData
void postData(String value1, String value2) {
  if (!client.connected()) { //Thực hiện kết nối lại google server khi mất kết nối
    Serial.println("Kết nối tới server...");
    client.connect(host, httpsPort);
  }
  //Tạo giá trị cần gửi vào URL (?value1=giatri&value2=giatri2)
  String urlFinal = url + "value1=" + value1 + "&value2=" + value2;
  //Thực hiện lệnh POST
  client.POST(urlFinal, host, "");
  Serial.println("POST: Đã gửi dữ liệu value1 =" + value1 + " value2= " + value2);
}


void loop() {
  // put your main code here, to run repeatedly:
  //Do mình sử dụng biến đếm thời gian của arduino để cho gửi chính xác hơn hàm delay()
  unsigned long currentMillis = millis();
  if (currentMillis - preMillis >= interval) {
    preMillis = currentMillis;
    postData("Dữ%20liệu%201", "DulieuTest"); //Thực hiện gửi dữ liệu vào google sheet
    dataget = getData();
    if (dataget != "")
    {
      for (int lct =0; lct<6;lct++)
      {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(100);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(100);
      }
    }
    Serial.println(dataget);     //Đồng thời lấy dữ liệu từ google sheet.
  }
}
