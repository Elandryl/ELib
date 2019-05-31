/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for EClient Class.
*/

#pragma once

#include "EPacketHandler.h"

/**
  @brief General scope for ELib components.
*/
namespace           ELib
{

  /**
    @brief EClient automatize the handling of ESocket for receiving EPackets.
    @details This class call recvPacket for incoming EPackets in its own thread.
    @details Every received EPackets are contained in the EPacketHandler in FIFO order.
  */
  class             EClient
  {
  public:
    EClient();
    ~EClient();
    void            init(const std::string &p_hostname, uint16 p_port);
    void            start();
    void            stop();
    void            recvPacket();
    void            sendPacket(EPacket *p_packet);
    EPacketHandler  &getPacketHandler();
    EPrinter        &getPrinter();
    bool            isRunning();

  private:
    ESocket         m_socketServer;     /**< ESocket for communication to EServer. */
    HANDLE          m_threadRecvPacket; /**< Handle for recvPacket thread. */
    EPacketHandler  m_packetHandler;    /**< EPacketHandlet that contains the received EPackets. */
    EPrinter        m_printer;          /**< EPrinter for console printing. */
    bool            m_isRunning;        /**< Indicate if EServer is running. */
  };

}
