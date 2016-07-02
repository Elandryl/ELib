#include "ENetwork/ESocket.h"

namespace		  ELib
{
  ESocket::ESocket() :
    m_status(STATUS_NONE)
  {
  }
  
  ESocket::ESocket(SOCKET &socket, SOCKADDR_IN &info) :
    m_socket(socket),
    m_info(info),
    m_status(STATUS_CONNECTED)
  {
  }
  
  ESocket::~ESocket()
  {
    if (!STATUS_NONE)
      closesocket(m_socket);
  }
  
  SOCKET		  ESocket::getSocket() const
  {
    return (m_socket);
  }
  
  ESocket::SocketStatus	  ESocket::getStatus() const
  {
    return (m_status);
  }

  bool			  ESocket::socket()
  {
    return ((m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET);
  }
  
  bool			  ESocket::connect(const char *hostname, uint16 port)
  {
    hostent		  *host;
    
    if ((m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
      m_status = STATUS_ERROR;
    if (!(host = gethostbyname(hostname)))
      return (false);
    m_info.sin_addr = *reinterpret_cast<IN_ADDR*>(host->h_addr);
    m_info.sin_port = htons(port);
    m_info.sin_family = AF_INET;
    if (::connect(m_socket, reinterpret_cast<SOCKADDR*>(&m_info), sizeof(SOCKADDR)) == SOCKET_ERROR)
      return (false);
    m_status = STATUS_CONNECTED;
    return (true);
  }
  
  bool			  ESocket::bind(const char *hostname, uint16 port)
  {
    if ((m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
      m_status = STATUS_ERROR;
    m_info.sin_addr.s_addr = /*inet_addr(hostname)*/ INADDR_ANY;
    m_info.sin_port = htons(port);
    m_info.sin_family = AF_INET;
    if (::bind(m_socket, reinterpret_cast<SOCKADDR*>(&m_info), sizeof(SOCKADDR)) == SOCKET_ERROR)
      return (false);
    m_status = STATUS_CONNECTED;
    return (true);
  }
  
  bool			  ESocket::listen()
  {
    if (::listen(m_socket, 64) == SOCKET_ERROR)
      return (false);
    return (true);
  }
  
  ESocket		  *ESocket::accept()
  {
    SOCKET		  client;
    SOCKADDR_IN		  info;
    int32		  size = sizeof(SOCKADDR);
    
    if ((client = ::accept(m_socket, reinterpret_cast<SOCKADDR*>(&info), &size)) == INVALID_SOCKET)
      return (nullptr);
    return (new ESocket(client, info));
  }
  
  bool			  ESocket::send(char *data, uint16 length, uint8 flags)
  {
    if (::send(m_socket, data, length, flags) == SOCKET_ERROR)
      return (false);
    return (true);
  }
  
  int32			  ESocket::recv(char *data, uint16 length, uint8 flags)
  {
    return (::recv(m_socket, data, length, flags));
  }
  
  bool			  ESocket::close()
  {
    closesocket(m_socket);
    m_status = STATUS_CREATED;
    return (true);
  }
}