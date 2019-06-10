/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetSocket Class.
*/

#pragma once

#include "EGlobals/EGlobal.h"

#define ENETSOCKET_FAMILY       (AF_INET) /**< Socket family used in ELib (IPv4 only). */
#define ENETSOCKET_MAX_CLIENTS  (64)      /**< Max number of client in the accept queue. */
#define ENETSOCKET_UDP_MAX      (65507)   /**< Lengh max of UDP EPacket */


/**
  @brief General scope for ELib components.
*/
namespace                       ELib
{

  /**
    @brief Flags for states and protocols of ENetSocket.
  */
  enum                          ENetSocketFlags
  {
    ENETSOCKET_FLAGS_STATE_UNINITIALIZED   = 0x0000,
    ENETSOCKET_FLAGS_STATE_INITIALIZED     = 0x0001,
    ENETSOCKET_FLAGS_STATE_BOUND           = 0x0002,
    ENETSOCKET_FLAGS_STATE_LISTENING       = 0x0004,
    ENETSOCKET_FLAGS_STATE_CONNECTED       = 0x0008,
    ENETSOCKET_FLAGS_STATES                = 0x000F,  /**< States range. */

    ENETSOCKET_FLAGS_PROTOCOL_TCP          = 0x0010,
    ENETSOCKET_FLAGS_PROTOCOL_UDP          = 0x0020,
    ENETSOCKET_FLAGS_PROTOCOLS             = 0x0030   /**< Protocols range. */
  };

  /**
    @brief Services for ENetSocket::shutdown call.
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
    ENetSocket();                                                                                   /**< /!\ .... */
    ~ENetSocket();                                                                                  /**< /!\ ..E. */
    void                        socket(ENetSocketFlags p_protocol);                                 /**< /!\ ..E. */
    void                        bind(const std::string &p_hostname, uint16 p_port);                 /**< /!\ ..E. */
    void                        listen();                                                           /**< /!\ ..E. */
    ENetSocket                  *accept();                                                          /**< /!\ B.E. */
    void                        connect(const std::string &p_hostname, uint16 p_port);              /**< /!\ ..E. */
    int32                       recv(char *p_datas, uint16 p_len);                                  /**< /!\ B.E. */
    int32                       recvfrom(char *p_datas, uint16 p_len, ENetSocket *p_src);           /**< /!\ B.E. */
    int32                       send(const char *p_datas, uint16 p_len);                            /**< /!\ ..E. */
    int32                       sendto(const char *p_datas, uint16 p_len, const ENetSocket *p_dst); /**< /!\ ..E. */
    void                        shutdown(ENetSocketService p_service = ENETSOCKET_SERVICE_BOTH);    /**< /!\ ..E. */
    void                        close();                                                            /**< /!\ ..E. */
    const std::string           &getHostname() const;                                               /**< /!\ .... */
    uint16                      getPort() const;                                                    /**< /!\ .... */
    ENetSocketFlags             getFlags() const;                                                   /**< /!\ .... */
    operator                    uint64() const;                                                     /**< /!\ .... */
    const std::string           toString() const;                                                   /**< /!\ .... */

  private:
    SOCKET                      m_socket;   /**< Unique identifier. */
    std::string                 m_hostname; /**< Internet host address in number-and-dots notation. */
    uint16                      m_port;     /**< Internet host port. */
    ENetSocketFlags             m_flags;    /**< Flags for state and protocol. */
  };

}
