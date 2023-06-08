#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <WebSocketsClient.h>

/*====== Defining API Endpoints ====== */
static String HOST = "192.168.172.114"; // hosted on a linux machine with all ports open.
static int PORT = 3000;
static boolean useSSL = false;
static String socketRoute = "/socket.io/?EIO=3";
// Transport = Polling made it connect disconnect again and again, but that was working on Unity or Other Angular Applications.

/*====== Defining Variables ====== */
#define LED 2
WebSocketsClient webSocket;
uint32_t chipId = 0;


/*====== Helper Functions ====== */
void APModeCallback(WiFiManager* manager) {
  Serial.println("Entered config mode, Please login with your WiFi/AP Credentials..");
  Serial.println(WiFi.softAPIP());
  Serial.println(manager->getConfigPortalSSID());
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for(uint32_t i = 0; i < len; i++) {
    if(i % cols == 0) {
      Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    Serial.printf("%02X ", *src);
    src++;
  }
  Serial.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
       webSocket.sendTXT("Connected");
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
       hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }

}


/*====== Socket Functions ====== */
void socket_Connected(const char * payload, size_t length) {
  Serial.println("Socket.IO Connected!");
}


void socket_event(const char * payload, size_t length) {
  Serial.print("got message: ");
  Serial.println(payload);
}

void setup() {
  // Set pin mode
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  /*======= Get ESP Chip ID =======*/
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  /*======= WiFi Manager Setup =======*/
  WiFiManager wifiManager;
  wifiManager.setAPCallback(APModeCallback);
  // Let it sleep into the eternal world of darkness after 5 minutes
  wifiManager.setTimeout(300);
  //On connection Failure
  if (!wifiManager.autoConnect("Webmind", "1223334444")) {
    Serial.println("Failed Connection to remote Access Point");
    ESP.restart();
    delay(5000);
  }

  /*======= Socket Setup =======*/
  
  webSocket.beginSocketIO(HOST, PORT, socketRoute);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(3000);
//  webSocket.setExtraHeaders("Authorization: Bearer ");
}

void loop() {
  webSocket.loop();
//  webSocket.sendTXT("['hello','some data']"); 
//  
//
//        // create JSON message for Socket.IO (event)
//        DynamicJsonDocument doc(15000);
//        JsonArray array = doc.to<JsonArray>();
//        
//        // add event name
//        // Hint: socket.on('event_name', ....
//        array.add("hello");
//
//        // add payload (parameters) for the event
//        JsonObject param1 = array.createNestedObject();
//        param1["hostname"] = hostname;
//        param1["picture"] = String((char *)fb->buf);
//
//        // JSON to String (serializion)
//        String output;
//        serializeJson(doc, output);
//
//        // Send event        
//        webSocket.sendEVENT(output);
//        Serial.println("Image sent");
//        Serial.println(output);
//        
}
