#include "ENetwork/EServer.h"
#include <iostream>

namespace	ELib
{
  EServer	*gServer = NULL;
  /* Functors */
  
  DWORD WINAPI	AcceptFunctor(LPVOID lpParam)
  {
    static_cast<EServer*>(lpParam)->accept();
    return (0);
  }
  
  DWORD WINAPI	SelectFunctor(LPVOID lpParam)
  {
    static_cast<EServer::Selector*>(lpParam)->select();
    return (0);
  }
  
  /* Server */
  
  EServer::EServer()
  {
    WSADATA	WSAData;
    
    if (WSAStartup(MAKEWORD(2, 2), &WSAData))
    {
      perror("error WSAStartup()");
      exit(EXIT_FAILURE);
    }
    gServer = this;
  }
  
  EServer::~EServer()
  {
    m_socketServer.close();
    while (!m_selectors.empty())
    {
      delete(m_selectors.back());
      m_selectors.pop_back();
    }
    WSACleanup();
  }

  bool		EServer::launch(const char *hostname, uint16 port)
  {
    m_socketServer.bind(hostname, port);
    m_socketServer.listen();
    m_isRunning = true;
    m_threadAccept = CreateThread(NULL, 0, AcceptFunctor, this, 0, NULL);
    std::cout << "Server launched : " << hostname << ":" << port << std::endl;
    return (true);
  }

  bool		EServer::accept()
  {
    ESocket	*client;
    
    while (m_isRunning)
      if (!(client = m_socketServer.accept())	|| !addSocket(client))
      {
	perror("error Accept()");
	exit(EXIT_FAILURE);
      }
    return (true);
  }
  
  bool		EServer::broadcast(char *data, uint32 length, uint8 flags)
  {
    for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end(); ++it)
      (*it)->broadcast(data, length, flags);
    return (true);
  }

  bool		EServer::addSocket(ESocket *client)
  {
    bool	added = false;
    
    for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end();)
      if (!(*it)->isRunning())
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
      m_selectors.push_back(new Selector(client));
    return (true);
  }
  
  bool		EServer::recv(ESocket *client)
  {
    int32	ret;
    char	*test = new char[18];

    ret = client->recv(test, 18);
    if (ret > 0)
    {
      std::cout << "RECV PACKET " << ret << std::endl;
      EPacket	*packet = new EPacket();

      packet->size = ret;
      packet->data = test;
      m_packets.push(packet);
    }
    else
    {
      if (ret == 0)
	perror("disconnected");
      else
	std::cout << ret << " error " << WSAGetLastError() << std::endl;
      delete(client);
      delete(test);
    }
    return (true);
  }
  
  bool		EServer::getPacket(char *data, uint32 size)
  {
    if (m_packets.empty())
      return (false);
    memcpy(data, m_packets.front()->data, m_packets.front()->size);
    delete(m_packets.front()->data);
    delete(m_packets.front());
    m_packets.pop();
    return (true);
  }

  /* Selector */
  
  EServer::Selector::Selector(ESocket *socket) :
    m_socketsClients{socket},
    m_maxFd(static_cast<uint32>(socket->getSocket()) + 1),
    m_running(true)
  {
    m_socketControll.socket();
    if (m_socketControll.getSocket() >= m_maxFd)
      m_maxFd = static_cast<uint32>(m_socketControll.getSocket()) + 1;
    m_threadSelect = CreateThread(NULL, 0, SelectFunctor, this, 0, NULL);
  }
  
  EServer::Selector::~Selector()
  {
    m_socketControll.close();
    for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end();)
    {
      (*it)->close();
      delete(*it);
      it = m_socketsClients.erase(it);
    }
  }

  bool		EServer::Selector::select()
  {
    while (m_socketsClients.size())
    {
      FD_ZERO(&m_set);
      FD_SET(m_socketControll.getSocket(), &m_set);
      for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
	FD_SET((*it)->getSocket(), &m_set);
      if (::select(m_maxFd, &m_set, NULL, NULL, NULL) < 0)
      {
	std::cout << "error Select" << WSAGetLastError() << std::endl;
	system("pause");
	exit(EXIT_FAILURE);
      }
      if (FD_ISSET(m_socketControll.getSocket(), &m_set))
	m_socketControll.socket();
      for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
	if (FD_ISSET((*it)->getSocket(), &m_set))
	  if (!gServer->recv(*it))
	    it = m_socketsClients.erase(it)--;
    }
    m_running = false;
    return (false);
  }
  
  bool		EServer::Selector::broadcast(char *data, uint32 length, uint8 flags)
  {
    for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
      (*it)->send(data, length, flags);
    return (true);
  }

  bool		EServer::Selector::addSocket(ESocket *socket)
  {
    if (m_socketsClients.size() >= MAX_SOCKETS_PER_SET)
      return (false);
    m_socketsClients.push_back(socket);
    if (socket->getSocket() >= m_maxFd)
      m_maxFd = static_cast<uint32>(socket->getSocket()) + 1;
    m_socketControll.close();
    return (true);
  }
  
  bool		EServer::Selector::isRunning() const
  {
    return (m_running);
  }
}