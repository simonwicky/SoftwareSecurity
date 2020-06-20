#include "inc/services/socket.hpp"
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <inc/services/logging.hpp>

Socket::Socket(Domain domain, CommunicationType type) {
  this->socketHandle = socket(static_cast<int>(domain), static_cast<int >(type), 0);
}

Socket::Socket(int socketHandle) {
  this->socketHandle = socketHandle;
}

int Socket::setSocketOption(Level level, const std::vector<Option> &options, const void *optionValue,
                            socketlen optionLength) {
  return setsockopt(this->socketHandle, static_cast<int>(level), makeOption(options), optionValue, optionLength);
}

int Socket::socketBind(SocketAddress *addr, socketlen addressLen) {
  return bind(this->socketHandle, addr, addressLen);
}

void Socket::socketListen(int queueSize) {
  assert(queueSize > 0);
  listen(this->socketHandle, queueSize);
}

Socket *Socket::acceptConnection(SocketAddress *address, socketlen *addressLen) {
  assert((address == nullptr && addressLen == nullptr) || address != nullptr);
  auto result = accept(this->socketHandle, address, addressLen);
  if (result > 0)
    return new Socket(result);
  if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
    return nullptr;
  return nullptr;
}

ssize_t Socket::socketSend(const std::string &buffer, const std::vector<MessageOption> &options) {
  return send(this->socketHandle, buffer.c_str(), buffer.length(), makeOption(options));
}

ssize_t Socket::socketReceive(std::string &buffer, const std::vector<MessageOption> &flags) {
  char buf[MAX_SIZE];
  auto read = recv(this->socketHandle, buf, MAX_SIZE, makeOption(flags));
  if (read > 0) {
    buffer.append(buf, static_cast<unsigned long>(read));
    return read;
  }

  if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
    return 0;

  return -1;
}

int Socket::makeOption(const std::vector<Option> &options) {
  int value = 0;
  for (auto &option: options) {
    value |= static_cast<int>(option);
  }
  return value;
}

int Socket::makeOption(const std::vector<MessageOption> &options) {
  int value = 0;
  for (auto &option: options) {
    value |= static_cast<int>(option);
  }
  return value;
}

void Socket::shutdownSocket() {
  if (!shutdownFlag)
    shutdown(this->socketHandle, SHUT_RDWR);
  shutdownFlag = true;
}

void Socket::closeSocket() {
  if (!closedFlag)
    close(this->socketHandle);
  closedFlag = true;
}

int Socket::portNumber() {
  struct sockaddr_in sin{};
  socklen_t len = sizeof(sin);
  if (getsockname(this->socketHandle, (struct sockaddr *) &sin, &len) == -1)
    return -1;
  else
    return ntohs(sin.sin_port);
}

Socket::~Socket() {
  shutdownSocket();
  closeSocket();
}

bool Socket::socketConnect(std::string &serverAddress, size_t port) {
  struct sockaddr_in servAddress{};

  memset(&servAddress, 0, sizeof(servAddress));

  servAddress.sin_family = AF_INET;
  servAddress.sin_port = htons(port);

  if (inet_pton(AF_INET, serverAddress.c_str(), &servAddress.sin_addr) <= 0) {
    LOG_INFO("invalid server address");
    return false;
  }

  if (connect(socketHandle, (struct sockaddr *) &servAddress, sizeof(servAddress)) < 0) {
    LOG_INFO("connection failed");
    return false;
  }
  return true;
}

ssize_t Socket::socketSend(char *buff, int size, const std::vector<MessageOption> &options) {
  return send(this->socketHandle, buff, size, makeOption(options));
}
