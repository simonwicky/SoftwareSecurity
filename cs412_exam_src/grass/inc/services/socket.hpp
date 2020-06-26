#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <cassert>
#include <string>

#define MAX_SIZE 2048

using socketlen = socklen_t;
using SocketAddress = sockaddr;
using SocketAddressIn = sockaddr_in;

/**
 * @brief custom deleter to be used for RAII
 */
struct socketAddressDeleter {
  void operator()(SocketAddress *address) {
    free(address);
  }
};

/**
 * @brief custom deleter to be used for RAII
 */
struct socketAddressInDeleter {
  void operator()(SocketAddressIn *socketAddressIn) {
    free(socketAddressIn);
  }
};

/**
 * Object wrapper for socket usage, methods map to their C counterparts
 */
class Socket {

public:

  enum class Domain : int {
    IPv4 = AF_INET, IPv6 = AF_INET6
  };

  enum class CommunicationType : int {
    TCP = SOCK_STREAM, UDP = SOCK_DGRAM
  };

  enum class Level {
    SOCKET = SOL_SOCKET, TCP = IPPROTO_TCP,
  };

  enum class Option {
    S_DEBUG = SO_DEBUG,
    BROADCAST = SO_DEBUG,
    REUSEADDR = SO_REUSEADDR,
    KEEPALIVE = SO_KEEPALIVE,
    REUSEPORT = SO_REUSEPORT,
    RCV_TIMEOUT = SO_RCVTIMEO
  };

  enum class MessageOption {
    // send option
        EOR = MSG_EOR,
    // send and recv option
        OOB = MSG_OOB,
    // recv options
        PEEK = MSG_PEEK, WAITALL = MSG_WAITALL
  };

  /**
   * @brief encapsulates a socket with the socketHandle handle
   * @param socketHandle
   */
  explicit Socket(int socketHandle);


  /**
   * @brief public constructor for creating a Socket
   * @param domain
   * @param type
   */
  Socket(Domain domain, CommunicationType type);

  Socket(const Socket &other) = delete;

  Socket(Socket &&other) = delete;

  ~Socket();

  /**
   * @brief set socket options.
   * @param level communication layer of the socket on which to set the options (might be smarter to split in multiple
   * methods for each layer)
   * @param options vector of options to be OR-ed and set
   * @param optionValue to be set for all supplied options
   * @param optionLength option value length
   * @return 0 on success and -1 on error
   */
  int setSocketOption(Level level, const std::vector<Option> &options, const void *optionValue, socketlen optionLength);

  /**
   * @brief binds a socket to a certain address
   * @param addr to be binned on
   * @param addressLen size of the address object
   * @return 0 on success and -1 on error
   */
  int socketBind(SocketAddress *addr, socketlen addressLen);


  /**
   * @brief sets the socket to a listen state
   * @param queueSize the size of the queue containing connection request
   */
  void socketListen(int queueSize);

  /**
    * @brief accepts a connection on a listening socket and returns the connection socket
    * @param address to be filled in with data representing the connected peer
    * @param addressLen size of the address to be filled
    * @return Socket wrapper to be used to communicate with a client
    */
  Socket *acceptConnection(SocketAddress *address, socketlen *addressLen);
  /**
   * @brief send a message through a socket
   * @param content content to be sent
   * @param size size of the content
   * @param options
   * @return number of sent bytes to the peer
   */
  ssize_t socketSend(const std::string &payload, const std::vector<MessageOption> &options);

  /**
   * @brief receive a message through a socket
   * @param buffer buffer to fill the received data with
   * @param buffer maximum size of data to receive
   * @param flags receive flags
   * @return the number of bytes received
   */
  ssize_t socketReceive(std::string &buffer, const std::vector<MessageOption> &flags);

  /**
   * @brief shuts down socket
   */
  void shutdownSocket();

  /**
   * @brief closes the socket
   */
  void closeSocket();

  /**
   * @brief returns the port number used by the socket
   * @return port number
   */
  int portNumber();

  /**
   * @brief connects the socket
   * @param serverAddress to which a socket should connect
   * @param port number of the server
   * @return true- success, false- failure
   */
  bool socketConnect(std::string &serverAddress, size_t port);

  /**
   * @brief send data using this socket
   * @param buff containing the data
   * @param size number of bytes to send
   * @param options message options
   * @return number of bytes sent
   */
  ssize_t socketSend(char *buff, int size, const std::vector<MessageOption> &options);

private:

  bool shutdownFlag = false;
  bool closedFlag = false;

  int socketHandle;

  /**
   * ORs socket options
   * @param options to be ORed
   * @return int representation of the requested options
   */
  int makeOption(const std::vector<Option> &options);
  /**
   * ORs message options
   * @param options to be ORed
   * @return int representation of the requested options
   */
  int makeOption(const std::vector<MessageOption> &options);
};