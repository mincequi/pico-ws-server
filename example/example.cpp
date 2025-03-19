#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "pico_ws_server/web_socket_server.h"

void on_connect(HttpServer& server, uint32_t conn_id) {
  printf("WebSocket opened\n");
  server.sendMessage(conn_id, "hello");
}

void on_disconnect(HttpServer& server, uint32_t conn_id) {
  printf("WebSocket closed\n");
}

void on_message(HttpServer& server, uint32_t conn_id, const void* data, size_t len) {
  printf("WebSocket message %d\n%s\n", (int)len, (char*)data);
}

int main() {
  stdio_init_all();

  if (cyw43_arch_init() != 0) {
    printf("cyw43_arch_init failed\n");
    while (1) tight_loop_contents();
  }

  cyw43_arch_enable_sta_mode();

  printf("Connecting to Wi-Fi...\n");
  if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
    printf("failed to connect.\n");
    while (1) tight_loop_contents();
  }
  printf("Connected.\n");

  HttpServer server;
  server.setConnectCallback(on_connect);
  server.setCloseCallback(on_disconnect);
  server.setMessageCallback(on_message);

  bool server_ok = server.startListening(80);
  if (!server_ok) {
    printf("Failed to start WebSocket server\n");
    while (1) tight_loop_contents();
  }
  printf("WebSocket server started\n");

  while (1) {
    cyw43_arch_poll();
  }
}
