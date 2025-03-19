#pragma once

#include <cstddef>
#include <memory>
#include <unordered_map>

#include "lwip/tcp.h"

#include "http/HttpServer.h"
#include "client_connection.h"

// Not multicore safe
class WebSocketServerInternal {
 public:
  WebSocketServerInternal(HttpServer& server, uint32_t max_connections)
      : server(server), max_connections(max_connections) {}

  void setConnectCallback(HttpServer::ConnectCallback cb) { connect_cb = cb; }
  void setCloseCallback(HttpServer::CloseCallback cb) { close_cb = cb; }
  void setMessageCallback(HttpServer::MessageCallback cb) { message_cb = cb; }
  void setUserCallback(HttpServer::UserCallback cb, void* context) {
    user_cb = cb;
    user_cb_context = context;
  }

  bool startListening(uint16_t port);
  void popMessages();

  bool sendMessage(uint32_t conn_id, const char* payload);
  bool sendMessage(uint32_t conn_id, const void* payload, size_t payload_size);
  bool broadcastMessage(const char* payload);
  bool broadcastMessage(const void* payload, size_t payload_size);

  bool sendRaw(uint32_t conn_id, const char* payload, size_t payload_size);

  bool close(uint32_t conn_id);

  ClientConnection* onConnect(struct tcp_pcb* pcb);
  void onUpgrade(ClientConnection* connection);
  void onClose(ClientConnection* connection, bool is_upgraded);

  void onMessage(ClientConnection* connection, const void* payload, size_t size);

  bool onUser(ClientConnection* connection, struct pbuf* pb);

 private:
  HttpServer& server;

  uint32_t max_connections;
  HttpServer::ConnectCallback connect_cb = nullptr;
  HttpServer::MessageCallback message_cb = nullptr;
  HttpServer::CloseCallback close_cb = nullptr;
  HttpServer::UserCallback user_cb = nullptr;
  void* user_cb_context = nullptr;

  struct tcp_pcb* listen_pcb = nullptr;
  std::unordered_map<uint32_t, std::unique_ptr<ClientConnection>> connection_by_id;

  uint32_t getConnectionId(ClientConnection* connection);
  ClientConnection* getConnectionById(uint32_t conn_id);
};
