/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for EServer Class.
*/

#pragma once

#include "ESelector.h"

/**
  @brief General scope for ELib components.
*/
namespace                     ELib
{

  /**
    @brief EServer automatize the handling of ESockets for receiving EPackets.
    @details This class call accept for incoming connections in its own thread.
    @details It will generate new ESelector every MAX_CLIENTS to dispatch the load.
    @details Every received EPackets are contained in the EPacketHandler in FIFO order.
  */
  class                       EServer
  {
  public:
    EServer();
    ~EServer();
    void                      init(const std::string &p_hostname, uint16 p_port);
    void                      start();
    void                      stop();
    void                      accept();
    void                      addClient(ESocket *p_client);
    void                      broadcast(EPacket *p_packet);
    void                      clearSelectors();
    EPacketHandler            &getPacketHandler();
    bool                      isRunning() const;
    EPrinter                  &getPrinter();
    const std::string         toString() const;

  private:
    ESocket                   m_socketAccept;   /**< ESocket for accept incoming connection. */
    std::vector<ESelector*>   m_selectors;      /**< List of ESelector that contains and handles the ESocket clients. */
    HANDLE                    m_threadAccept;   /**< Handle for accept thread. */
    HANDLE                    m_mutexSelectors; /**< Semaphore for m_selectors thread safety. */
    EPacketHandler            m_packetHandler;  /**< EPacketHandler that contains the received EPackets. */
    EPrinter                  m_printer;        /**< EPrinter for console printing. */
    bool                      m_isRunning;      /**< Indicate if EServer is running. */
  };

}
