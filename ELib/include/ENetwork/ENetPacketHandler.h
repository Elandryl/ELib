/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetPacketHandler Class.
*/

#pragma once

#include <map>
#include <queue>
#include "EGlobals/EGlobal.h"
#include "ENetwork/ENetPacket.h"

/**
  @brief General scope for ELib components.
*/
namespace                     ELib
{

  /**
    @brief Function for auto-generation of ENetPacket.
    @param p_src ENetSocket source of ENetPacket.
    @return Generated ENetPacket.
  */
  typedef ENetPacket *(*ENetPacketGenerator)(ENetSocket *p_src);                                      /**< /!\ ..E. */

  /**
    @brief ELib object for ENetPacket automation (Singleton).
    @details Automatically generate and store every ENetPacket of an application.
    @details Basics generators are provided. More can be provide with custom ENetPacketType.
  */
  class                       ENetPacketHandler
  {
  public:
    ~ENetPacketHandler();
    static ENetPacketHandler  *getInstance();                                                       /**< /!\ ..E. */
    ENetPacket                *popPacket();                                                         /**< /!\ .M.. */
    void                      recvPacket(ENetSocket *p_src);                                        /**< /!\ BME. */
    void                      read(char *p_datas, int32 p_len, ENetSocket *p_src = nullptr);        /**< /!\ .ME. */
    void                      setGenerator(ENetPacketType p_type, ENetPacketGenerator p_generator); /**< /!\ ..E. */
    void                      cleanSocket(const ENetSocket *p_socket);                              /**< /!\ .M.. */

  private:
    ENetPacketHandler();

    std::map<ENetPacketType,
      ENetPacketGenerator>    m_generators;   /**< ENetPacketGenerator map. */
    std::queue<ENetPacket*>   m_packets;      /**< Received ENetPacket queue. */
    HANDLE                    m_mutexPackets; /**< m_packet semaphore. */
  };

}
