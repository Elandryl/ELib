/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetServer Class.
*/

#pragma once

#include "EGlobals/EGlobal.h"
#include "ENetwork/ENetSelector.h"

/**
  @brief General scope for ELib components.
*/
namespace                       ELib
{

  /**
    @brief Elib object for network server side automation (Singleton).
    @details Call ENetServer::recvfrom() for incoming connectionless datas in its own thread.
    @details Call ENetServer::accept() for incoming connections in its own thread.
    @details Automatically generate ENetSelector every MAX_CLIENTS to dispatch load.
    @details Use ENetPacketHandler for ENetPacket storage.
  */
  class                         ENetServer
  {
  public:
    ~ENetServer();                                                                    /**< .... */
    static ENetServer           *getInstance();                                       /**< ..E. */
    void                        init(const std::string &p_hostname, uint16 p_port);   /**< ..E. */
    void                        start();                                              /**< .ME. */
    void                        stop();                                               /**< .ME. */
    void                        recvfrom();                                           /**< BME. */
    void                        accept();                                             /**< BME. */
    void                        addClient(ENetSocket *p_client);                      /**< .ME. */
    void                        broadcast(ENetPacket *p_packet);                      /**< .ME. */
    void                        clearSelectors();                                     /**< .M.. */
    bool                        isRunning() const;                                    /**< .... */
    const std::string           toString() const;                                     /**< .M.. */

  private:
    ENetServer();

    ENetSocket                  m_socketRecvfrom; /**< recvfrom() ENetSocket. */
    HANDLE                      m_threadRecvfrom; /**< recvfrom() thread. */
    ENetSocket                  m_socketAccept;   /**< accept() ENetSocket. */
    HANDLE                      m_threadAccept;   /**< accept() thread. */
    std::vector<ENetSelector*>  m_selectors;      /**< ENetSelector list. */
    HANDLE                      m_mutexSelectors; /**< m_selectors semaphore. */
    bool                        m_isRunning;      /**< State. */
  };

}
