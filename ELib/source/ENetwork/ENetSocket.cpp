/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetSocket Class.
*/

#include "ENetwork/ENetSocket.h"

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief Constructor for ENetSocket.
    @details State is ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
  */
  ENetSocket::ENetSocket() :
    m_socket(INVALID_SOCKET),
    m_hostname("0.0.0.0"),
    m_port(0),
    m_flags(ENETSOCKET_FLAGS_STATE_UNINITIALIZED)
  {
  }
  
  /**
    @brief Destructor for ENetSocket. /!\ EError.
    @details Call ENetSocket::close().
  */
  ENetSocket::~ENetSocket()
  {
    mEERROR_R();
    if ((INVALID_SOCKET != m_socket)
      && (ENETSOCKET_FLAGS_STATE_UNINITIALIZED != (m_flags & ENETSOCKET_FLAGS_STATES)))
    {
      close();
      if (EERROR_NONE != mEERROR)
      {
        mEERROR_SH(EERROR_NET_SOCKET_ERR);
      }
    }
  }

  /**
    @brief Initialize ENetSocket depending on protocol. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @details Protocol must be in range of ENETSOCKET_FLAGS_PROTOCOLS.
    @details On success, state is set to ENETSOCKET_FLAGS_STATE_INITIALIZED.
    @param p_protocol Protocol to be used.
  */
  void                  ENetSocket::socket(ENetSocketFlags p_protocol)
  {
    mEERROR_R();
    p_protocol = static_cast<ENetSocketFlags>(p_protocol & ENETSOCKET_FLAGS_PROTOCOLS);
    if (ENETSOCKET_FLAGS_STATE_UNINITIALIZED != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (0 != (p_protocol & (~ENETSOCKET_FLAGS_PROTOCOLS)))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    
    if (EERROR_NONE == mEERROR)
    {
      switch (p_protocol)
      {
        case ENETSOCKET_FLAGS_PROTOCOL_TCP:
        {
          m_socket = ::socket(ENETSOCKET_FAMILY, SOCK_STREAM, IPPROTO_TCP);
        }
          break;
        case ENETSOCKET_FLAGS_PROTOCOL_UDP:
        {
          m_socket = ::socket(ENETSOCKET_FAMILY, SOCK_DGRAM, IPPROTO_UDP);
        }
          break;
        default:
          break;
      }
      if (INVALID_SOCKET != m_socket)
      {
        m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_INITIALIZED | p_protocol);
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
      }
    }
  }

  /**
    @brief Bind ENetSocket to a local internet address. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATE_INITIALIZED.
    @details On success, state is set to ENETSOCKET_FLAGS_STATE_BOUND.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
  */
  void                  ENetSocket::bind(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_INITIALIZED != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      SOCKADDR_IN       l_infos = { 0 };

      l_infos.sin_addr.s_addr = inet_addr(m_hostname.c_str());
      if (INADDR_NONE != l_infos.sin_addr.s_addr)
      {
        m_hostname = p_hostname;
        m_port = p_port;
        l_infos.sin_port = htons(m_port);
        l_infos.sin_family = ENETSOCKET_FAMILY;
        if (SOCKET_ERROR != ::bind(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR)))
        {
          m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_BOUND | (m_flags & ENETSOCKET_FLAGS_PROTOCOLS));
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, "Not a legitimate Internet address");
        }
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
      }
    }
  }

  /**
    @brief Put ENetSocket in listening state. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATE_BOUND.
    @details Protocol must be ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @details On success, state is set to ENETSOCKET_FLAGS_STATE_LISTENING.
  */
  void                  ENetSocket::listen()
  {
    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_BOUND != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    
    if (EERROR_NONE == mEERROR)
    {
      if (SOCKET_ERROR != ::listen(m_socket, ENETSOCKET_MAX_CLIENTS))
      {
        m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_LISTENING | ENETSOCKET_FLAGS_PROTOCOL_TCP);
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
      }
    }
  }

  /**
    @brief Accept incoming connection to ENetSocket. /!\ Blocking. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATE_LISTENING.
    @details Protocol must be ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @return ENetSocket of newly connected client on success.
    @return nullptr on failure.
  */
  ENetSocket            *ENetSocket::accept()
  {
    ENetSocket          *l_client = nullptr;

    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_LISTENING != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_client = new ENetSocket();
      if (nullptr != l_client)
      {
        SOCKADDR_IN     l_infos = { 0 };
        int32           l_infosLen = sizeof(SOCKADDR_IN);

        l_client->m_socket = ::accept(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), &l_infosLen);
        if (INVALID_SOCKET != l_client->m_socket)
        {
          l_client->m_hostname = inet_ntoa(l_infos.sin_addr);
          l_client->m_port = l_infos.sin_port;
          l_client->m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_CONNECTED | ENETSOCKET_FLAGS_PROTOCOL_TCP);
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
          delete (l_client);
          l_client = nullptr;
        }
      }
      else
      {
        mEERROR_S(EERROR_MEMORY);
      }
    }

    return (l_client);
  }

  /**
    @brief Connect ENetSocket to a host internet address. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATE_BOUND.
    @details Protocol must be ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @details On success, state is set to ENETSOCKET_FLAGS_STATE_CONNECTED.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
  */
  void                  ENetSocket::connect(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_BOUND != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }

    if (EERROR_NONE == mEERROR)
    {
      SOCKADDR_IN       l_infos = { 0 };

      l_infos.sin_addr.s_addr = inet_addr(m_hostname.c_str());
      if (INADDR_NONE != l_infos.sin_addr.s_addr)
      {
        m_hostname = p_hostname;
        m_port = p_port;
        l_infos.sin_port = htons(m_port);
        l_infos.sin_family = ENETSOCKET_FAMILY;
        if (SOCKET_ERROR != ::connect(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR)))
        {
          m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_CONNECTED | ENETSOCKET_FLAGS_PROTOCOL_TCP);
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
        }
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, "Not a legitimate Internet address");
      }
    }
  }
  
  /**
    @brief Receive datas from connected ENetSocket. /!\ Blocking. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATUS_CONNECTED.
    @details Protocol must be ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @param p_datas Buffer to receive the incoming datas.
    @param p_len Length of buffer.
    @return Length of received datas on success.
    @return 0 on disconnection. ENetSocket is automatically closed.
    @return -1 on failure.
  */
  int32                 ENetSocket::recv(char *p_datas, uint16 p_len)
  {
    int32               l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_CONNECTED != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    if ((nullptr == p_datas)
      && (0 != p_len))
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (0 != p_len)
      {
        l_len = ::recv(m_socket, p_datas, p_len, 0);
        if (SOCKET_ERROR != l_len)
        {
          if (0 == l_len)
          {
            close();
          }
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
        }
      }
      else
      {
        p_len = 0;
      }
    }

    return (l_len);
  }

  /**
    @brief Receive datas from connectionless ENetSocket. /!\ Blocking. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATUS_BOUND.
    @details Protocol must be ENETSOCKET_FLAGS_PROTOCOL_UDP.
    @details ENetSocket source must be valid.
    @param p_datas Buffer to receive the incoming datas.
    @param p_len Length of buffer.
    @param p_src ENetSocket that will hold informations of the source.
    @return Length of received datas on success.
    @return -1 on failure.
  */
  int32                 ENetSocket::recvfrom(char *p_datas, uint16 p_len, ENetSocket *p_src)
  {
    int32               l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_BOUND != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_UDP != (m_flags & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    if (nullptr == p_src)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if ((nullptr == p_datas)
      && (0 != p_len))
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (0 != p_len)
      {
        SOCKADDR_IN     l_infos = { 0 };
        int32           l_infosLen = sizeof(SOCKADDR_IN);

        l_len = ::recvfrom(m_socket, p_datas, p_len, 0, reinterpret_cast<SOCKADDR*>(&l_infos), &l_infosLen);
        if (SOCKET_ERROR != l_len)
        {
          p_src->m_hostname = inet_ntoa(l_infos.sin_addr);
          p_src->m_port = l_infos.sin_port;
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
        }
      }
      else
      {
        p_len = 0;
      }
    }

    return (l_len);
  }

  /**
    @brief Send datas to connected ENetSocket. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATUS_CONNECTED.
    @details Protocol must be ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @param p_datas Buffer of datas to be send.
    @param p_len Length of buffer.
    @return Length of sent datas on success.
    @return -1 on failure.
  */
  int32                 ENetSocket::send(const char *p_datas, uint16 p_len)
  {
    int32               l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_CONNECTED != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    if ((nullptr == p_datas)
      && (0 != p_len))
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (0 != p_len)
      {
        l_len = ::send(m_socket, p_datas, p_len, 0);
        if (SOCKET_ERROR == l_len)
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
        }
      }
      else
      {
        l_len = 0;
      }
    }

    return (l_len);
  }

  /**
    @brief Send datas to connectionless ENetSocket. /!\ EError.
    @details State must be ENETSOCKET_FLAGS_STATUS_BOUND.
    @details Protocol must be ENETSOCKET_FLAGS_PROTOCOL_UDP.
    @details ENetSocket destination must be valid.
    @param p_datas Buffer of datas to be send.
    @param p_len Length of buffer.
    @param p_dst ENetSocket that hold informations of the destination.
    @return Length of sent datas on success.
    @return -1 on failure.
  */
  int32                 ENetSocket::sendto(const char *p_datas, uint16 p_len, const ENetSocket *p_dst)
  {
    int32               l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_BOUND != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_UDP != (m_flags & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    if (nullptr == p_dst)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if ((nullptr == p_datas)
      && (0 != p_len))
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (0 != p_len)
      {
        SOCKADDR_IN     l_infos = { 0 };

        l_infos.sin_addr.s_addr = inet_addr(m_hostname.c_str());
        if (INADDR_NONE != l_infos.sin_addr.s_addr)
        {
          l_infos.sin_port = htons(p_dst->m_port);
          l_infos.sin_family = ENETSOCKET_FAMILY;
          l_len = ::sendto(m_socket, p_datas, p_len, 0, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR_IN));
          if (SOCKET_ERROR == l_len)
          {
            mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
          }
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, "Not a legitimate Internet address");
        }
      }
      else
      {
        l_len = 0;
      }
    }

    return (l_len);
  }

  /**
    @brief Shutdown a service of ENetSocket. /!\ EError.
    @details State must not be ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @param p_service Types of service to be shutdown.
  */
  void                  ENetSocket::shutdown(ENetSocketService p_service)
  {
    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_UNINITIALIZED == (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (SOCKET_ERROR == ::shutdown(m_socket, p_service))
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
      }
    }
  }
  
  /**
    @brief Close ENetSocket. /!\ EError.
    @details State must not be ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @details On success, state is set to ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
  */
  void                  ENetSocket::close()
  {
    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_UNINITIALIZED == (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (SOCKET_ERROR != ::closesocket(m_socket))
      {
        m_flags = ENETSOCKET_FLAGS_STATE_UNINITIALIZED;
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
      }
    }
  }

  /**
    @brief Get hostname of ENetSocket.
    @return Internet host address in number-and-dots notation.
  */
  const std::string     &ENetSocket::getHostname() const
  {
    return (m_hostname);
  }

  /**
    @brief Get port of ENetSocket.
    @return Internet host port.
  */
  uint16                ENetSocket::getPort() const
  {
    return (m_port);
  }

  /**
    @brief Get flags of ENetSocket.
    @return Flags for state and protocol.
  */
  ENetSocketFlags       ENetSocket::getFlags() const
  {
    return (m_flags);
  }

  /**
    @brief Convert ENetSocket to unique identifier.
    @return Unique identifier.
  */
  ENetSocket::operator  uint64() const
  {
    return (static_cast<uint64>(m_socket));
  }

  /**
    @brief Retrieve ENetSocket informations.
    @return Informations as string.
  */
  const std::string     ENetSocket::toString() const
  {
    std::string         str = "";

    str += "ENetSocket " + std::to_string(m_socket) + ": ";
    str += "(" + m_hostname + ":" + std::to_string(m_port) + ")";

    return (str);
  }

}
