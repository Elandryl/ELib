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
    @brief ENetServer automatize the handling of ENetSockets for receiving ENetPackets.
    @details This class call accept for incoming connections in its own thread.
    @details It will generate new ENetSelector every MAX_CLIENTS to dispatch the load.
    @details Every received ENetPackets are contained in the ENetPacketHandler in FIFO order.
  */
  class                         ENetServer
  {
  public:
    ~ENetServer();
    static ENetServer           *getInstance();
    void                        init(const std::string &p_hostname, uint16 p_port);
    void                        start();
    void                        stop();
    void                        accept();
    void                        addClient(ENetSocket *p_client);
    void                        broadcast(ENetPacket *p_packet);
    void                        clearSelectors();
    ENetPacketHandler           &getPacketHandler();
    bool                        isRunning() const;
    const std::string           toString() const;

  private:
    ENetServer();

    ENetSocket                  m_socketAccept;   /**< ENetSocket for accept incoming connection. */
    HANDLE                      m_threadAccept;   /**< Handle for accept thread. */
    std::vector<ENetSelector*>  m_selectors;      /**< List of ENetSelector that contains and handles the ENetSocket clients. */
    HANDLE                      m_mutexSelectors; /**< Semaphore for m_selectors thread safety. */
    ENetPacketHandler           m_packetHandler;  /**< ENetPacketHandler that contains the received ENetPackets. */
    bool                        m_isRunning;      /**< Indicate if ENetServer is running. */
  };

}
