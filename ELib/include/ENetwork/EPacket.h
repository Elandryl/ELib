#pragma once

#include "ESocket.h"

namespace		    ELib
{
  class			    EPacket
  {
  public:
    EPacket(uint16 opcode) : m_opcode(opcode) {}
    virtual ~EPacket() {}
    virtual EErrorCode      recv(ESocket *socket, uint8 flags = 0) = 0;
    virtual EErrorCode      send(ESocket *socket, uint8 flags = 0)
    {
      return (socket->send(reinterpret_cast<char*>(&m_opcode), sizeof(uint16)));
    }

  protected:
    uint16		    m_opcode;
  };

  class			    EPacketGenerator
  {
  public:
    virtual ~EPacketGenerator() {}
    virtual EPacket	    *generatePacket(uint16 opcode) = 0;
  };
}