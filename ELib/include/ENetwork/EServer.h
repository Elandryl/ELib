#pragma once

#include <vector>
#include <queue>
#include "ESocket.h"
#include "EPacket.h"

#define MAX_SOCKETS_PER_SET 64

namespace		      ELib
{
  class			      EServer
  {
  public:

    /* Selector */

    class		      Selector
    {
    public:
      Selector(ESocket *socket);
      ~Selector();
      EErrorCode	      launch();
      void		      stop();
      EErrorCode	      select();
      EErrorCode	      broadcast(EPacket *packet, uint8 flags = 0);
      bool		      addSocket(ESocket *socket);
      bool		      empty() const;
    
    private:
      fd_set		      m_set;
      ESocket		      m_socketControll;
      std::vector<ESocket*>   m_socketsClients;
      unsigned int	      m_maxFd;
      HANDLE		      m_threadSelect;
      bool		      m_isRunning;
    };
    
    /* Server */

    EServer();
    ~EServer();
    void		      definePacketGenerator(EPacketGenerator *packetGenerator);
    EErrorCode		      connect(const char *hostname, uint16 port);
    EErrorCode		      run();
    void		      stop();
    EErrorCode		      accept();
    EErrorCode		      broadcast(EPacket *packet, uint8 flags = 0);
    EPacket*		      getPacket();
  
  private:
    EErrorCode		      addSocket(ESocket *client);
    EErrorCode		      recvPacket(ESocket *client);

    ESocket		      m_socketServer;
    HANDLE		      m_threadAccept;
    std::vector<Selector*>    m_selectors;
    bool		      m_isRunning;
    EPacketGenerator	      *m_packetGenerator;
    std::queue<EPacket*>      m_packets;
  };
}