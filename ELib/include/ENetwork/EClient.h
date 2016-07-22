#pragma once

#include <queue>
#include "ESocket.h"
#include "EPacket.h"

namespace		  ELib
{
  class			  EClient
  {
  public:
    EClient();
    ~EClient();
    void		  definePacketGenerator(EPacketGenerator *packetGenerator);
    EErrorCode		  connect(const char *hostname, uint16 port);
    EErrorCode		  run();
    void		  stop();
    EErrorCode		  recvPacket();
    EErrorCode		  sendPacket(EPacket *packet);
    EPacket*		  getPacket();
  
  private:
    ESocket		  m_socket;
    HANDLE		  m_threadRecv;
    bool		  m_isRunning;
    EPacketGenerator	  *m_packetGenerator;
    std::queue<EPacket*>  m_packets;
  };
}