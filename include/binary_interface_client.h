/*
 * File: BinaryInterfaceClient.h
 * Created On: 2019-09-14
 * Copyright © 2019 Bosch Rexroth AG
 */

#ifndef BINARYINTERFACECLIENT_H
#define BINARYINTERFACECLIENT_H

/*
 * C++ Standard lib
 */
#include <assert.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

/*
 * Linux Netinet header and types
 */
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>

class BinaryInterfaceClient {
public:
  /*
   * Constructor: Connect immediately. Throws runtime_error on any error
   */
  BinaryInterfaceClient(const std::string &address, const uint16_t port)
      : Address(address), Port(port) {
    Connect();
  }

  /*
   * Destructor: Release OS Handle
   */
  ~BinaryInterfaceClient() { close(Handle); }

  /*
   * OS - read function
   */
  int32_t read(void *const target, const int32_t size) {
    // return ::read(Handle, target, size);
    int bytesRead = 0;
    int result;
    while (bytesRead < size) {
      void *const targetBufferPtr{
          static_cast<void *>(static_cast<char *>(target) + bytesRead)};
      result = ::read(Handle, targetBufferPtr, size - bytesRead);
      if (result < 1) {
        return result;
      }

      bytesRead += result;
    }
    return bytesRead;
  }

  /*
   * OS - write function
   * Not needed since Clients ALWAYS read
   */

private:
  std::string Address;
  uint16_t Port;
  int Handle{-1};

  /*
   * Connect procedure -> resolve address and connect
   */
  void Connect() {
    int32_t rc;
    int32_t Index;
    int32_t Error;

    constexpr int32_t retryAttempts = 100;

    //==
    // Resolve Address and Portstring to addrinfo saved at res (alloc is done by
    // linux)
    struct addrinfo *res;
    {
      char portString[16];
      snprintf(portString, 16, "%u", Port);

      struct addrinfo hints {};
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;

      rc = getaddrinfo(Address.c_str(), portString, &hints, &res);
      Error = errno;

      if (rc != 0) {
        throw std::runtime_error(
            "BinaryInterfaceClient::Connect: Error resolving the given address "
            "and port as stream-socket. Errno: " +
            std::to_string(Error) + ": " + std::string(strerror(Error)));
      }

      assert(res != nullptr);
    }

    Index = 0;
    do {
      for (struct addrinfo *walk = res; walk != nullptr; walk = walk->ai_next) {
        if (Index > 0) {
          sleep(1);
        }
        char addressString[128];

        //==
        // Reinterpret address from addrinfo element as sockaddr_in and set ipv4
        // or v6 address
        if (walk->ai_family == AF_INET) {
          struct sockaddr_in *addr = (struct sockaddr_in *)walk->ai_addr;
          inet_ntop(walk->ai_family, &(addr->sin_addr), addressString, 128);
        } else {
          struct sockaddr_in6 *addr = (struct sockaddr_in6 *)walk->ai_addr;
          inet_ntop(walk->ai_family, &(addr->sin6_addr), addressString, 128);
        }

        std::cout << "Connect to address: " << addressString
                  << " Attempt: " << Index << " Of: " << retryAttempts
                  << std::endl;
        //==
        // Create Socket and save handle!!
        std::cout << "Create Socket and save handle" << std::endl;
        Handle = socket(walk->ai_family, walk->ai_socktype, walk->ai_protocol);
        if (Handle < 0) {
          std::cout
              << "BinaryInterfaceClient::Connect: Attempt failed. Errno was: "
              << std::to_string(Error) << ": " << std::string(strerror(Error));
          continue;
        }

        //==
        // Connect to prev. constructed address

        std::cout << "Connect to prev. constructed address" << std::endl;
        rc = connect(Handle, walk->ai_addr, walk->ai_addrlen);
        Error = errno;
        if (rc != 0) {
          close(Handle);
          Handle = -1;

          std::cout
              << "BinaryInterfaceClient::Connect: Attempt failed. Errno was: "
              << std::to_string(Error) << ": " << std::string(strerror(Error));
        } else {
          break;
        }
      }
    } while ((rc < 0) &&
             ((Error == ETIMEDOUT) || (Error == ENETUNREACH) ||
              (Error == ECONNREFUSED)) &&
             (Index++ < retryAttempts));

    freeaddrinfo(res);
    if (Handle == -1) {
      throw std::runtime_error("BinaryInterfaceClient::Connect: Error during "
                               "connection. Last errno was: " +
                               std::to_string(Error) + ": " +
                               std::string(strerror(Error)));
    }

    int32_t Flag = 0;

    // Set TCP_NODELAY: Deactive Nagle
    rc = setsockopt(Handle, IPPROTO_TCP, TCP_NODELAY, &Flag, sizeof(Flag));
    if (rc < 0) {
      throw std::runtime_error("BinaryInterfaceClient::Connect: Error during "
                               "setsockopt: TCP_NODELAY");
    }

    std::cout << "Connected!" << std::endl;
  }
};

#endif // BINARYINTERFACECLIENT_H
