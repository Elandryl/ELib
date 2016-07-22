#pragma once

#include <Windows.h>
#include "Global.h"

namespace		ELib
{
  class			ESocket
  {
  public:
    ESocket();
    ESocket(SOCKET&, SOCKADDR_IN&);
    ~ESocket();
    SOCKET		getSocket() const;
    EErrorCode		socket();
    EErrorCode		connect(const char *hostname, uint16);
    EErrorCode		bind(const char *hostname, uint16 port);
    EErrorCode		listen();
    ESocket		*accept();
    EErrorCode		send(char *data, uint16 length, uint8 flags = 0);
    int32		recv(char *data, uint16 length, uint8 flags = 0);
    EErrorCode		close();
    EErrorCode		shutdown(uint8 flag = 2);
  
  private:
    SOCKET		m_socket;
    SOCKADDR_IN		m_info;
  };
}