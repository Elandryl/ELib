/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetSocket Class.
*/

#pragma once

#include "EGlobals/EGlobal.h"

#define ENETSOCKET_MAX_CLIENTS  (64)      /**< Define the max number of client in the accept queue. */
#define ENETSOCKET_FAMILY       (AF_INET) /**< Define the socket family used in ELib. (AF_INET for IPv4 only) */
#define ENETSOCKET_UDP_MAX      (65507)   /**< Define the lengh max of a UDP EPacket */


/**
  @brief General scope for ELib components.
*/
namespace                       ELib
{

  /**
    @brief Flags for state and protocol of a ENetSocket.
  */
  enum                          ENetSocketFlags
  {
    ENETSOCKET_FLAGS_STATE_UNINITIALIZED   = 0x00,
    ENETSOCKET_FLAGS_STATE_INITIALIZED     = 0x01,
    ENETSOCKET_FLAGS_STATE_BOUND           = 0x02,
    ENETSOCKET_FLAGS_STATE_LISTENING       = 0x04,
    ENETSOCKET_FLAGS_STATE_CONNECTED       = 0x08,
    ENETSOCKET_FLAGS_STATES                = 0x0F, /**< Range of Status. */

    ENETSOCKET_FLAGS_PROTOCOL_UNDEFINED    = 0x00,
    ENETSOCKET_FLAGS_PROTOCOL_TCP          = 0x10,
    ENETSOCKET_FLAGS_PROTOCOL_UDP          = 0x20,
    ENETSOCKET_FLAGS_PROTOCOLS             = 0x30 /**< Range of Protocols. */
  };

  /**
    @brief Services for shutdown call.
  */
  enum                          ENetSocketService
  {
    ENETSOCKET_SERVICE_RECEIVE  = 0,
    ENETSOCKET_SERVICE_SEND     = 1,
    ENETSOCKET_SERVICE_BOTH     = 2
  };

  /**
    @brief ELib object for socket handling.
    @details Class for Socket functionalities and management.
    @details It can use multiple protocols and keep track of ENetSocket state.
  */
  class                         ENetSocket
  {
  public:
    ENetSocket();
    ~ENetSocket();
    void                        socket(ENetSocketFlags p_protocol);
    void                        bind(const std::string &p_hostname, uint16 p_port);
    void                        listen();
    ENetSocket                  *accept();
    void                        connect(const std::string &p_hostname, uint16 p_port);
    int32                       recv(char *p_datas, uint16 p_len);
    int32                       recvfrom(char *p_datas, uint16 p_len, ENetSocket *p_src);
    int32                       send(const char *p_datas, uint16 p_len);
    int32                       sendto(const char *p_datas, uint16 p_len, const ENetSocket *p_dst);
    void                        shutdown(ENetSocketService p_service = ENETSOCKET_SERVICE_BOTH);
    void                        close();
    const std::string           &getHostname() const;
    uint16                      getPort() const;
    ENetSocketFlags             getFlags() const;
    operator                    uint64() const;
    const std::string           toString() const;

  private:
    SOCKET                      m_socket;   /**< Unique identifier. */
    std::string                 m_hostname; /**< Internet host address in number-and-dots notation. */
    uint16                      m_port;     /**< Internet host port. */
    ENetSocketFlags             m_flags;    /**< Flags for state and protocol. */
  };

}
