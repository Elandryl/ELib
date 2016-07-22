#include "ENetwork/EServer.h"
#include <iostream>

namespace	    ELib
{
  EServer	    *gServer = nullptr;
  /* Functors */
  
  DWORD WINAPI	    AcceptFunctor(LPVOID lpParam)
  {
    static_cast<EServer*>(lpParam)->accept();
    return (0);
  }
  
  DWORD WINAPI	    SelectFunctor(LPVOID lpParam)
  {
    static_cast<EServer::Selector*>(lpParam)->select();
    return (0);
  }
  
  /* Server */
  
  EServer::EServer() :
    m_isRunning(false),
    m_packetGenerator(nullptr)
  {
    WSADATA	    WSAData;
    
    if (WSAStartup(MAKEWORD(2, 2), &WSAData))
      EThrowIfFailed(EERROR_WSA_STARTUP);
    gServer = this;
  }
  
  EServer::~EServer()
  {
    m_socketServer.close();
    for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end(); ++it)
      delete(*it);
    while (!m_packets.empty())
    {
      delete(m_packets.front());
      m_packets.pop();
    }
    WSACleanup();
  }

  void		    EServer::definePacketGenerator(EPacketGenerator *packetGenerator)
  {
    m_packetGenerator = packetGenerator;
  }

  EErrorCode	    EServer::connect(const char *hostname, uint16 port)
  {
    m_socketServer.bind(hostname, port);
    m_socketServer.listen();
    std::cout << "Server launched : " << hostname << ":" << port << std::endl;
    return (EERROR_NONE);
  }

  EErrorCode	    EServer::run()
  {
    if (!m_packetGenerator)
      EReturn(EERROR_PACKET_GENERATOR);
    m_isRunning = true;
    m_threadAccept = CreateThread(nullptr, 0, AcceptFunctor, this, 0, nullptr);
    for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end(); ++it)
      (*it)->launch();
    return (EERROR_NONE);
  }

  void		    EServer::stop()
  {
    m_isRunning = false;
    //m_socketServer.shutdown();
    TerminateThread(m_threadAccept, 0);
    for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end(); ++it)
      (*it)->stop();
  }
  
  EErrorCode	    EServer::accept()
  {
    ESocket	    *client;
    
    while (m_isRunning)
    {
      if (!(client = m_socketServer.accept()))
	return (EERROR_SOCKET_ACCEPT);
      if (!addSocket(client))
	EReturn(EERROR_SERVER_ADD);
    }
    return (EERROR_NONE);
  }
  
  EErrorCode	    EServer::broadcast(EPacket *packet, uint8 flags)
  {
    bool	    error = false;

    for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end(); ++it)
      if ((*it)->broadcast(packet, flags) != EERROR_NONE)
	error = true;
    return (error ? EERROR_SOCKET_SEND : EERROR_NONE);
  }

  EErrorCode	    EServer::addSocket(ESocket *client)
  {
    bool	    added = false;
    
    for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end();)
      if ((*it)->empty())
      {
	delete(*it);
	it = m_selectors.erase(it);
      }
      else
      {
	if (!added && (*it)->addSocket(client))
	  added = true;
	++it;
      }
    if (!added)
    {
      m_selectors.push_back(new Selector(client));
      if (m_selectors.back()->launch() != EERROR_NONE)
	return (EERROR_SELECTOR_LAUNCH);
    }
    return (EERROR_NONE);
  }

  EErrorCode	    EServer::recvPacket(ESocket *client)
  {
    int32	    ret;
    uint16	    opcode = 0;

    ret = client->recv(reinterpret_cast<char*>(&opcode), sizeof(uint16));
    if (ret > 0)
    {
      EPacket	    *packet;

      if (!(packet = m_packetGenerator->generatePacket(opcode)) || packet->recv(client) != EERROR_NONE)
	EReturn(EERROR_INVALID_PACKET);
      m_packets.push(packet);
    }
    if (ret < 0)
      EReturn(EERROR_CLIENT_RECEIVE);
    return (EERROR_NONE);
  }

  EPacket	    *EServer::getPacket()
  {
    EPacket	    *packet;

    if (m_packets.empty())
      return (nullptr);
    packet = m_packets.front();
    m_packets.pop();
    return (packet);
  }

  /* Selector */
  
  EServer::Selector::Selector(ESocket *socket) :
    m_socketsClients{socket},
    m_maxFd(static_cast<uint32>(socket->getSocket()) + 1),
    m_isRunning(false)
  {
  }
  
  EServer::Selector::~Selector()
  {
    m_socketControll.close();
    for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
    {
      (*it)->close();
      delete(*it);
    }
  }

  EErrorCode	    EServer::Selector::launch()
  {
    if (m_socketControll.socket() != EERROR_NONE)
      EReturn(EERROR_SELECTOR_LAUNCH);
    m_isRunning = true;
    if (m_socketControll.getSocket() >= m_maxFd)
      m_maxFd = static_cast<uint32>(m_socketControll.getSocket()) + 1;
    m_threadSelect = CreateThread(nullptr, 0, SelectFunctor, this, 0, nullptr);
    return (EERROR_NONE);
  }

  void		    EServer::Selector::stop()
  {
    m_isRunning = false;
    //m_socketControll.shutdown();
    TerminateThread(m_threadSelect, 0);
  }

  EErrorCode	    EServer::Selector::select()
  {
    while (m_isRunning)
    {
      FD_ZERO(&m_set);
      FD_SET(m_socketControll.getSocket(), &m_set);
      for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
    	FD_SET((*it)->getSocket(), &m_set);
      if (::select(m_maxFd, &m_set, nullptr, nullptr, nullptr) < 0)
	EReturn(EERROR_SERVER_SELECT);
      if (FD_ISSET(m_socketControll.getSocket(), &m_set))
	if (m_socketControll.socket() != EERROR_NONE)
	  return (EERROR_INVALID_SOCKET);
      for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end();)
      {
	EErrorCode  error;

	if (FD_ISSET((*it)->getSocket(), &m_set) && !(error = gServer->recvPacket(*it)))
	{
	  (*it)->close();
	  delete(*it);
	  it = m_socketsClients.erase(it);
	  if (m_socketsClients.empty())
	    stop();
	  return (error);
	}
	else
	  ++it;
      }
    }
    return (EERROR_NONE);
  }
  
  EErrorCode	    EServer::Selector::broadcast(EPacket *packet, uint8 flags)
  {
    bool	    error = false;

    for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
      if (packet->send(*it, flags) != EERROR_NONE)
	error = true;
    return (error ? EERROR_SOCKET_SEND : EERROR_NONE);
  }

  bool		    EServer::Selector::addSocket(ESocket *socket)
  {
    if (m_socketsClients.size() >= MAX_SOCKETS_PER_SET - 1)
      return (false);
    m_socketsClients.push_back(socket);
    if (socket->getSocket() >= m_maxFd)
      m_maxFd = static_cast<uint32>(socket->getSocket()) + 1;
    m_socketControll.close();
    return (true);
  }
  
  bool		    EServer::Selector::empty() const
  {
    return (m_socketsClients.empty());
  }
}