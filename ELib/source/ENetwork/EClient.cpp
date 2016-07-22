#include "ENetwork/EClient.h"
#include <iostream>

namespace	ELib
{
  DWORD WINAPI	RecvFunctor(LPVOID lpParam)
  {
    static_cast<EClient*>(lpParam)->recvPacket();
    return (0);
  }

  EClient::EClient() :
    m_isRunning(false),
    m_packetGenerator(nullptr)
  {
    WSADATA	WSAData;
    
    if (WSAStartup(MAKEWORD(2, 2), &WSAData))
      EThrowIfFailed(EERROR_WSA_STARTUP);
  }

  EClient::~EClient()
  {
    while (!m_packets.empty())
    {
      delete(m_packets.front());
      m_packets.pop();
    }
    TerminateThread(m_threadRecv, 0);
    m_socket.shutdown();
    WSACleanup();
  }

  void		EClient::definePacketGenerator(EPacketGenerator *packetGenerator)
  {
    m_packetGenerator = packetGenerator;
  }

  EErrorCode	EClient::connect(const char *hostname, uint16 port)
  {
    if (m_socket.connect(hostname, port) != EERROR_NONE)
      return (EERROR_SOCKET_CONNECT);
    std::cout << "Client connected : " << hostname << ":" << port << std::endl;
    return (EERROR_NONE);
  }

  EErrorCode	EClient::run()
  {
    if (!m_packetGenerator)
      EReturn(EERROR_PACKET_GENERATOR);
    m_isRunning = true;
    m_threadRecv = CreateThread(nullptr, 0, RecvFunctor, this, 0, nullptr);
    return (EERROR_NONE);
  }

  void		EClient::stop()
  {
    m_isRunning = false;
    //m_socket.shutdown();
    TerminateThread(m_threadRecv, 0);
  }

  EErrorCode	EClient::recvPacket()
  {
    while (m_isRunning)
    {
      int32	ret;
      uint16	opcode = 0;

      ret = m_socket.recv(reinterpret_cast<char*>(&opcode), sizeof(uint16));
      if (ret > 0)
      {
	EPacket	*packet;

	if (!(packet = m_packetGenerator->generatePacket(opcode)) || packet->recv(&m_socket) != EERROR_NONE)
	  EReturn(EERROR_INVALID_PACKET);
	m_packets.push(packet);
      }
      else
      {
	m_isRunning = false;
	if (ret < 0)
  	  EReturn(EERROR_CLIENT_RECEIVE);
      }
    }
    return (EERROR_NONE);
  }

  EErrorCode	EClient::sendPacket(EPacket *packet)
  {
    return (packet->send(&m_socket));
  }

  EPacket	*EClient::getPacket()
  {
    EPacket	*packet;

    if (m_packets.empty())
      return (nullptr);
    packet = m_packets.front();
    m_packets.pop();
    return (packet);
  }
}