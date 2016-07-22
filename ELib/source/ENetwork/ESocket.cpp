#include "ENetwork/ESocket.h"

namespace		  ELib
{
  ESocket::ESocket()
  {
  }
  
  ESocket::ESocket(SOCKET &socket, SOCKADDR_IN &info) :
    m_socket(socket),
    m_info(info)
  {
  }
  
  ESocket::~ESocket()
  {
    if (m_socket != INVALID_SOCKET)
      close();
  }
  
  SOCKET		  ESocket::getSocket() const
  {
    return (m_socket);
  }

  EErrorCode		  ESocket::socket()
  {
    if ((m_socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
      EReturn(EERROR_INVALID_SOCKET);
    return (EERROR_NONE);
  }

  
  EErrorCode		  ESocket::connect(const char *hostname, uint16 port)
  {
    hostent		  *host;
    
    if (socket() != EERROR_NONE)
      EReturn(EERROR_INVALID_SOCKET);
    if (!(host = gethostbyname(hostname)))
      EReturn(EERROR_REACH_HOSTNAME);
    m_info.sin_addr = *reinterpret_cast<IN_ADDR*>(host->h_addr);
    m_info.sin_port = htons(port);
    m_info.sin_family = AF_INET;
    if (::connect(m_socket, reinterpret_cast<SOCKADDR*>(&m_info), sizeof(SOCKADDR)) == SOCKET_ERROR)
      EReturn(EERROR_SOCKET_CONNECT);
    return (EERROR_NONE);
  }
  
  EErrorCode		  ESocket::bind(const char *hostname, uint16 port)
  {
    if (socket() != EERROR_NONE)
      EReturn(EERROR_INVALID_SOCKET);
    m_info.sin_addr.s_addr = inet_addr(hostname) /*INADDR_ANY*/;
    m_info.sin_port = htons(port);
    m_info.sin_family = AF_INET;
    if (::bind(m_socket, reinterpret_cast<SOCKADDR*>(&m_info), sizeof(SOCKADDR)) == SOCKET_ERROR)
      EReturn(EERROR_SOCKET_BIND);
    return (EERROR_NONE);
  }
  
  EErrorCode		  ESocket::listen()
  {
    if (::listen(m_socket, 64) == SOCKET_ERROR)
      EReturn(EERROR_SOCKET_LISTEN);
    return (EERROR_NONE);
  }
  
  ESocket		  *ESocket::accept()
  {
    SOCKET		  client;
    SOCKADDR_IN		  info;
    int32		  size = sizeof(SOCKADDR);
    
    if ((client = ::accept(m_socket, reinterpret_cast<SOCKADDR*>(&info), &size)) == INVALID_SOCKET)
      EReturnValue(EERROR_SOCKET_ACCEPT, nullptr);
    return (new ESocket(client, info));
  }
  
  EErrorCode		  ESocket::send(char *data, uint16 length, uint8 flags)
  {
    if (::send(m_socket, data, length, flags) == SOCKET_ERROR)
      EReturn(EERROR_SOCKET_SEND);
    return (EERROR_NONE);
  }
  
  int32			  ESocket::recv(char *data, uint16 length, uint8 flags)
  {
    return (::recv(m_socket, data, length, flags));
  }
  
  EErrorCode		  ESocket::close()
  {
    if (closesocket(m_socket))
      EReturn(EERROR_SOCKET_CLOSE);
    return (EERROR_NONE);
  }

  EErrorCode		  ESocket::shutdown(uint8 flag)
  {
    if (::shutdown(m_socket, flag))
      EReturn(EERROR_SOCKET_SHUTDOWN);
    return (EERROR_NONE);
  }
}