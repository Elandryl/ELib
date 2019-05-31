/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ESocket Class.
*/

#pragma once

#include "EGlobal.h"

#define MAX_CLIENTS     (64)      /**< Define the max number of client in the accept queue. */
#define ESOCKET_FAMILY  (AF_INET) /**< Define the socket family used in ELib. (AF_INET for IPv4 only) */

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief Flags for state and protocol of a ESocket.
  */
  enum                  ESocketFlags
  {
    ESOCKET_FLAGS_STATE_UNINITIALIZED   = 0x00,
    ESOCKET_FLAGS_STATE_INITIALIZED     = 0x01,
    ESOCKET_FLAGS_STATE_BOUND           = 0x02,
    ESOCKET_FLAGS_STATE_LISTENING       = 0x04,
    ESOCKET_FLAGS_STATE_CONNECTED       = 0x08,
    ESOCKET_FLAGS_STATES                = 0x0F, /**< Range of Status. */

    ESOCKET_FLAGS_PROTOCOL_UNDEFINED    = 0x00,
    ESOCKET_FLAGS_PROTOCOL_TCP          = 0x10,
    ESOCKET_FLAGS_PROTOCOL_UDP          = 0x20,
    ESOCKET_FLAGS_PROTOCOLS             = 0x30 /**< Range of Protocols. */
  };

  /**
    @brief Services for shutdown call.
  */
  enum                  ESocketService
  {
    ESOCKET_SERVICE_RECEIVE             = 0,
    ESOCKET_SERVICE_SEND                = 1,
    ESOCKET_SERVICE_BOTH                = 2
  };

  /**
    @brief ELib object for socket handling.
    @details Class for Socket functionalities and management.
    @details It can use multiple protocols and keep track of ESocket state.
  */
  class                 ESocket
  {
  public:
    ESocket();
    ~ESocket();
    void                socket(ESocketFlags p_protocol);
    void                bind(const std::string &p_hostname, uint16 p_port);
    void                listen();
    ESocket             *accept();
    void                connect(const std::string &p_hostname, uint16 p_port);
    int32               recv(char *p_datas, uint16 p_len);
    int32               recvfrom(char *p_datas, uint16 p_len, ESocket *p_src);
    int32               send(const char *p_datas, uint16 p_len);
    int32               sendto(const char *p_datas, uint16 p_len, const ESocket *p_dst);
    void                shutdown(ESocketService p_service = ESOCKET_SERVICE_BOTH);
    void                close();
    const std::string   &getHostname() const;
    uint16              getPort() const;
    ESocketFlags        getFlags() const;
    operator            uint64() const;
    const std::string   toString() const;

  private:
    SOCKET              m_socket;   /**< Unique identifier. */
    std::string         m_hostname; /**< Internet host address in number-and-dots notation. */
    uint16              m_port;     /**< Internet host port. */
    ESocketFlags        m_flags;    /**< Flags for state and protocol. */
  };

}
