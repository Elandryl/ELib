/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetSelector Class.
*/

#pragma once

#include <vector>
#include "EGlobals/EGlobal.h"
#include "ENetwork/ENetPacketHandler.h"

/**
  @brief General scope for ELib components.
*/
namespace                     ELib
{
  
  /**
    @brief ELib object for ENetSocket receives automation.
    @details This class call select on a set of ENetSocket in its own thread.
    @details It is automatically stopped when no ENetSocket are contained.
  */
  class                       ENetSelector
  {
  public:
    ENetSelector(ENetPacketHandler &p_packetHandler, ENetSocket *p_client);
    ~ENetSelector();
    void                      start();
    void                      stop();
    void                      select();
    bool                      addClient(ENetSocket *p_client);
    void                      broadcast(ENetPacket *p_packet);
    bool                      isEmpty() const;
    bool                      isRunning() const;
    const std::string         toString() const;

  private:
    ENetPacketHandler         &m_packetHandler; /**< ENetPacketHandler that hold the received ENetPackets. */
    std::vector<ENetSocket*>  m_socketClients;  /**< List of ENetSocket that are handled by this ENetSelector. */
    HANDLE                    m_threadSelect;   /**< Handle for select thread. */
    HANDLE                    m_mutexClients;   /**< Semaphore for m_socketsClients thread safety. */
    bool                      m_isRunning;      /**< Indicate if ENetSelector is running. */
  };

}
