/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for EPacketHandler Class.
*/

#pragma once

#include <map>
#include <queue>
#include "EPacket.h"

/**
  @brief General scope for ELib components.
*/
namespace                   ELib
{

  /**
    @brief Pointer to Function for auto-generation of EPacket.
    @param p_src Pointer to ESocket source of the EPacket.
    @return Pointer to new EPacket. Type depends on EPacketGenerator used.
  */
  typedef EPacket *(*EPacketGenerator)(ESocket *p_src);

  /**
    @brief ELib object for EPacket handling.
    @details Automatically receive and contain every EPacket of an application.
    @details Basic generator are provided. More can be provided with custom EPacketType.
  */
  class                     EPacketHandler
  {
  public:
    EPacketHandler();
    ~EPacketHandler();
    EPacket                 *getPacket();
    void                    recvPacket(ESocket *p_src);
    void                    generate(ESocket *p_src, char *p_datas, int32 p_len);
    void                    setGenerator(EPacketType p_opCode, EPacketGenerator p_generator);
    void                    cleanSocket(const ESocket *p_socket);

  private:
    std::map<EPacketType,
      EPacketGenerator>     m_generators;   /**< Map of EPacketGenerator linked to a EPacketType. */
    std::queue<EPacket*>    m_packets;      /**< Queue of shared_ptr to received EPacket. */
    HANDLE                  m_mutexPackets; /**< Semaphore for m_packets thread safety. */
  };

}
