#include "http/HttpServer.h"

#include <memory>
#include <stdint.h>

#include "web_socket_server_internal.h"

HttpServer::HttpServer(uint32_t max_connections)
    : internal(std::make_unique<WebSocketServerInternal>(*this, max_connections)) {}
HttpServer::~HttpServer() {}

void HttpServer::setConnectCallback(ConnectCallback cb) {
  internal->setConnectCallback(cb);
}
void HttpServer::setMessageCallback(MessageCallback cb) {
  internal->setMessageCallback(cb);
}
void HttpServer::setCloseCallback(CloseCallback cb) {
  internal->setCloseCallback(cb);
}
void HttpServer::setCallbackExtra(void* arg) {
  callback_extra = arg;
}
void* HttpServer::getCallbackExtra() {
  return callback_extra;
}
void HttpServer::setUserCallback(UserCallback cb, void* context) {
  internal->setUserCallback(cb, context);
}

bool HttpServer::startListening(uint16_t port) {
  return internal->startListening(port);
}
void HttpServer::popMessages() {
  internal->popMessages();
}

bool HttpServer::sendMessage(uint32_t conn_id, const char* payload) {
  return internal->sendMessage(conn_id, payload);
}
bool HttpServer::sendMessage(uint32_t conn_id, const void* payload, size_t payload_size) {
  return internal->sendMessage(conn_id, payload, payload_size);
}

bool HttpServer::broadcastMessage(const char* payload) {
  return internal->broadcastMessage(payload);
}
bool HttpServer::broadcastMessage(const void* payload, size_t payload_size) {
  return internal->broadcastMessage(payload, payload_size);
}

bool HttpServer::sendRaw(uint32_t conn_id, const char* payload, size_t payload_size) {
  return internal->sendRaw(conn_id, payload, payload_size);
}

bool HttpServer::close(uint32_t conn_id) {
  return internal->close(conn_id);
}