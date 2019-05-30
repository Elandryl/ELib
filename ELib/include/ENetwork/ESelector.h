/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ESelector Class.
*/

#pragma once

#include <vector>
#include "EPacketHandler.h"

/**
  @brief General scope for ELib components.
*/
namespace                   ELib
{
  
  /**
    @brief ELib object for ESocket receives automation.
    @details This class call select on a set of ESocket in its own thread.
    @details It is automatically stopped when no ESocket are contained.
  */
  class                     ESelector
  {
  public:
    ESelector(EPacketHandler &p_packetHandler, ESocket *p_client);
    ~ESelector();
    void                    start();
    void                    stop();
    void                    select();
    bool                    addClient(ESocket *p_client);
    void                    broadcast(EPacket *p_packet);
    bool                    isEmpty() const;
    bool                    isRunning() const;
    const std::string       toString() const;

  private:
    EPacketHandler          &m_packetHandler; /**< EPacketHandler that hold the received EPackets. */
    std::vector<ESocket*>   m_socketClients;  /**< List of ESocket that are handled by this ESelector. */
    HANDLE                  m_threadSelect;   /**< Handle for select thread. */
    HANDLE                  m_mutexClients;   /**< Semaphore for m_socketsClients thread safety. */
    bool                    m_isRunning;      /**< Indicate if ESelector is running. */
  };

}
