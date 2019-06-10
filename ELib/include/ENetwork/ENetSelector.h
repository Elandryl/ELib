/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetSelector Class.
*/

#pragma once

#include <vector>
#include "EGlobals/EGlobal.h"
#include "ENetPacketHandler.h"

/**
  @brief General scope for ELib components.
*/
namespace                     ELib
{
  
  /**
    @brief ELib object for connected ENetSocket automation in ENetServer.
    @details Call ENetSelector::select() on its clients in its own thread.
    @details Automatically stopped when no client are contained.
  */
  class                       ENetSelector
  {
  public:
    ENetSelector();                                             /**< .... */
    ~ENetSelector();                                            /**< .... */
    void                      start();                          /**< ..E. */
    void                      stop();                           /**< ..E. */
    void                      select();                         /**< BME. */
    bool                      addClient(ENetSocket *p_client);  /**< .ME. */
    void                      broadcast(ENetPacket *p_packet);  /**< .ME. */
    uint32                    getSize() const;                  /**< .... */
    bool                      isRunning() const;                /**< .... */
    const std::string         toString() const;                 /**< .M.. */

  private:
    std::vector<ENetSocket*>  m_clients;        /**< ENetSocket list. */
    HANDLE                    m_threadSelect;   /**< select() thread. */
    HANDLE                    m_mutexClients;   /**< m_client semaphore. */
    bool                      m_isRunning;      /**< State. */
  };

}
