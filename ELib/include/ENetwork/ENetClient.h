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
    @brief ENetClient automatize the handling of ENetSocket for receiving ENetPackets.
    @details This class call recvPacket for incoming ENetPackets in its own thread.
    @details Every received ENetPackets are contained in the ENetPacketHandler in FIFO order.
  */
  class                 ENetClient
  {
  public:
    ENetClient();
    ~ENetClient();
    void                init(const std::string &p_hostname, uint16 p_port);
    void                start();
    void                stop();
    void                recvPacket();
    void                sendPacket(ENetPacket *p_packet);
    ENetPacketHandler   &getPacketHandler();
    bool                isRunning();

  private:
    ENetSocket          m_socketServer;     /**< ENetSocket for communication to ENetServer. */
    HANDLE              m_threadRecvPacket; /**< Handle for recvPacket thread. */
    ENetPacketHandler   m_packetHandler;    /**< ENetPacketHandlet that contains the received ENetPackets. */
    bool                m_isRunning;        /**< Indicate if ENetServer is running. */
  };

}
