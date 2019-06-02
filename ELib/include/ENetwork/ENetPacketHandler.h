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
    @brief Pointer to Function for auto-generation of ENetPacket.
    @param p_src Pointer to ENetSocket source of the ENetPacket.
    @return Pointer to new ENetPacket. Type depends on ENetPacketGenerator used.
  */
  typedef ENetPacket *(*ENetPacketGenerator)(ENetSocket *p_src);

  /**
    @brief ELib object for ENetPacket handling.
    @details Automatically receive and contain every ENetPacket of an application.
    @details Basic generator are provided. More can be provided with custom ENetPacketType.
  */
  class                       ENetPacketHandler
  {
  public:
    ENetPacketHandler();
    ~ENetPacketHandler();
    ENetPacket                *getPacket();
    void                      recvPacket(ENetSocket *p_src);
    void                      generate(ENetSocket *p_src, char *p_datas, int32 p_len);
    void                      setGenerator(ENetPacketType p_opCode, ENetPacketGenerator p_generator);
    void                      cleanSocket(const ENetSocket *p_socket);

  private:
    std::map<ENetPacketType,
      ENetPacketGenerator>    m_generators;   /**< Map of ENetPacketGenerator linked to a ENetPacketType. */
    std::queue<ENetPacket*>   m_packets;      /**< Queue of shared_ptr to received ENetPacket. */
    HANDLE                    m_mutexPackets; /**< Semaphore for m_packets thread safety. */
  };

}
