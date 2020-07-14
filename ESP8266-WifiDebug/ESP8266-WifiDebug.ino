#include <ESP8266WiFi.h> // Include the Wi-Fi library

// The SSID (name) of the Wi-Fi network you want to connect to
const char* ssid = "SSID";
// The password of the Wi-Fi network
const char* password = "WIFI-PSK";

WiFiEventHandler disconnectedEventHandler;

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}

void WiFiEvent(WiFiEvent_t event) {

  switch(event) {

    case WIFI_EVENT_STAMODE_CONNECTED:
      Serial.println("[WiFi-event] Connected");
      break;

    case WIFI_EVENT_STAMODE_DISCONNECTED:
      Serial.println("[WiFi-event] Disconnected");
      break;

    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      Serial.println("[WiFi-event] Authmode change");
      break;

    case WIFI_EVENT_STAMODE_GOT_IP:
      Serial.println("[WiFi-event] Connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;

    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
      Serial.println("[WiFi-event] DHCP timeout");
      break;

    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      Serial.println("[WiFi-event] AP connected");
      break;

    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      Serial.println("[WiFi-event] AP disconnected");
      break;

    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
      Serial.println("[WiFi-event] AP probe request received");
      break;

    case WIFI_EVENT_MODE_CHANGE:
      Serial.println("[WiFi-event] Mode changed");
      break;

    case WIFI_EVENT_SOFTAPMODE_DISTRIBUTE_STA_IP:
      Serial.println("[WiFi-event] AP distribute IP");
      break;

    default:
      Serial.printf("[WiFi-event] Event: %d\n", event);
      break;
  }
}

void onDisconnect(const WiFiEventStationModeDisconnected& event) {
  switch (event.reason) {
    case WIFI_DISCONNECT_REASON_UNSPECIFIED:                Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Unspecified\n");              break;
    case WIFI_DISCONNECT_REASON_AUTH_EXPIRE:                Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Auth expire\n");              break;
    case WIFI_DISCONNECT_REASON_AUTH_LEAVE:                 Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Auth leave\n");               break;
    case WIFI_DISCONNECT_REASON_ASSOC_EXPIRE:               Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Assoc expire\n");             break;
    case WIFI_DISCONNECT_REASON_ASSOC_TOOMANY:              Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Assoc toomany\n");            break;
    case WIFI_DISCONNECT_REASON_NOT_AUTHED:                 Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Not authed\n");               break;
    case WIFI_DISCONNECT_REASON_NOT_ASSOCED:                Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Not assoced\n");              break;
    case WIFI_DISCONNECT_REASON_ASSOC_LEAVE:                Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Assoc leave\n");              break;
    case WIFI_DISCONNECT_REASON_ASSOC_NOT_AUTHED:           Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Assoc not authed\n");         break;
    case WIFI_DISCONNECT_REASON_DISASSOC_PWRCAP_BAD:        Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Disassoc pwrcap bad\n");      break;
    case WIFI_DISCONNECT_REASON_DISASSOC_SUPCHAN_BAD:       Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Disassoc supchan bad\n");     break;
    case WIFI_DISCONNECT_REASON_IE_INVALID:                 Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected IE invalid\n");               break;
    case WIFI_DISCONNECT_REASON_MIC_FAILURE:                Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Mic failure\n");              break;
    case WIFI_DISCONNECT_REASON_4WAY_HANDSHAKE_TIMEOUT:     Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected 4way handshake timeout\n");   break;
    case WIFI_DISCONNECT_REASON_GROUP_KEY_UPDATE_TIMEOUT:   Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Group key update timeout\n"); break;
    case WIFI_DISCONNECT_REASON_IE_IN_4WAY_DIFFERS:         Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected IE in 4way differs\n");       break;
    case WIFI_DISCONNECT_REASON_GROUP_CIPHER_INVALID:       Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Group cipher invalid\n");     break;
    case WIFI_DISCONNECT_REASON_PAIRWISE_CIPHER_INVALID:    Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Pairwise cipher invalid\n");  break;
    case WIFI_DISCONNECT_REASON_AKMP_INVALID:               Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected AKMP invalid\n");             break;
    case WIFI_DISCONNECT_REASON_UNSUPP_RSN_IE_VERSION:      Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Unsupp RSN IE version\n");    break;
    case WIFI_DISCONNECT_REASON_INVALID_RSN_IE_CAP:         Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Invalid RSN IE cap\n");       break;
    case WIFI_DISCONNECT_REASON_802_1X_AUTH_FAILED:         Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected 802 1X auth failed\n");       break;
    case WIFI_DISCONNECT_REASON_CIPHER_SUITE_REJECTED:      Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Cipher suite rejected\n");    break;
    case WIFI_DISCONNECT_REASON_BEACON_TIMEOUT:             Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Beacon timeout\n");           break;
    case WIFI_DISCONNECT_REASON_NO_AP_FOUND:                Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected No AP found\n");              break;
    case WIFI_DISCONNECT_REASON_AUTH_FAIL:                  Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Auth fail\n");                break;
    case WIFI_DISCONNECT_REASON_ASSOC_FAIL:                 Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Assoc fail\n");               break;
    case WIFI_DISCONNECT_REASON_HANDSHAKE_TIMEOUT:          Serial.printf("[WiFi-event] WiFiEventStationModeDisconnected Handshake timeout\n");        break;
  }
}

void setup() {
  WiFi.disconnect();
  Serial.begin(115200);
  delay(10);
  Serial.println();

  // Debug
  Serial.println(wl_status_to_string(WiFi.status()));
  WiFi.onEvent(WiFiEvent);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(onDisconnect);

  // Connect to the network
  WiFi.setOutputPower(20.5);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");
  
  int i = 0;
  // Wait for the Wi-Fi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
    Serial.println(wl_status_to_string(WiFi.status()));
    if (i > 60) {
      break;
    }
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    // IP address of the ESP8266
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect!");
    WiFi.mode(WIFI_OFF);
    WiFi.disconnect(true);
  }
  Serial.println();
}

void loop() {
}
