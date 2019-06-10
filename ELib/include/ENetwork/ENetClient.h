/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetClient Class.
*/

#pragma once

#include "EGlobals/EGlobal.h"
#include "ENetwork/ENetPacketHandler.h"

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief Elib object for network client side automation (Singleton).
    @details Call ENetClient::recvfrom() for incoming connectionless datas in its own thread.
    @details Call ENetClient::recv() for incoming connected datas in its own thread.
    @details Use ENetPacketHandler for ENetPacket storage.
  */
  class                 ENetClient
  {
  public:
    ~ENetClient();                                                            /**< .... */
    static ENetClient   *getInstance();                                       /**< ..E. */
    void                init(const std::string &p_hostname, uint16 p_port);   /**< ..E. */
    void                start();                                              /**< ..E. */
    void                stop();                                               /**< ..E. */
    void                recvfrom();                                           /**< BME. */
    void                recv();                                               /**< BME. */
    void                send(ENetPacket *p_packet);                           /**< ..E. */
    bool                isRunning();                                          /**< .... */

  private:
    ENetClient();

    ENetSocket          m_socketRecvfrom;   /**< recvfrom() ENetSocket. */
    HANDLE              m_threadRecvfrom;   /**< recvfrom() thread. */
    ENetSocket          m_socketRecv;       /**< recv() ENetSocket. */
    HANDLE              m_threadRecv;       /**< recv() thread. */
    bool                m_isRunning;        /**< State. */
  };

}
