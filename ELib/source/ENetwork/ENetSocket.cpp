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
    @brief Instantiate a uninitialized ENetSocket.
    @details ENetSocket default state is ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @details ENetSocket default protocol is ENETSOCKET_FLAGS_PROTOCOL_UNDEFINED.
  */
  ENetSocket::ENetSocket() :
    m_socket(INVALID_SOCKET),
    m_hostname("0.0.0.0"),
    m_port(0),
    m_flags(static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_UNINITIALIZED | ENETSOCKET_FLAGS_PROTOCOL_UNDEFINED))
  {
  }
  
  /**
    @brief Delete a ENetSocket.
    @details ENetSocket is closed if not yet done.
  */
  ENetSocket::~ENetSocket()
  {
    if ((INVALID_SOCKET != m_socket)
      && (ENETSOCKET_FLAGS_STATE_UNINITIALIZED != (m_flags & ENETSOCKET_FLAGS_STATES)))
    {
      close();
    }
  }

  /**
    @brief Initialize the ENetSocket depending on p_protocol from parameter.
    @details ENetSocket must be in state of ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @details p_protocol must be a flag in range of ENETSOCKET_FLAGS_PROTOCOLS.
    @param p_protocol Protocol to be used to initialize the ENetSocket.
    @eerror EERROR_NONE in success. State is set ENETSOCKET_FLAGS_STATE_INITIALIZED.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_NET_SOCKET_PROTOCOL if parameter p_protocol is not in range of ENETSOCKET_FLAGS_PROTOCOL.
    @eerror EERROR_NET_SOCKET_INVALID if socket() fail.
  */
  void                  ENetSocket::socket(ENetSocketFlags p_protocol)
  {
    mEERROR_R();
    p_protocol = static_cast<ENetSocketFlags>(p_protocol & ENETSOCKET_FLAGS_PROTOCOLS);
    if (ENETSOCKET_FLAGS_STATE_UNINITIALIZED != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_UNDEFINED == p_protocol)
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
        mEERROR_SA(EERROR_NET_SOCKET_INVALID, getWSAErrString());
      }
    }
  }

  /**
    @brief Bind the ENetSocket to a local internet address.
    @details ENetSocket must be in state ENETSOCKET_FLAGS_STATE_INITIALIZED.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success. State is set ENETSOCKET_FLAGS_STATE_BOUND.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATE_INITIALIZED.
    @eerror EERROR_NET_SOCKET_BIND if bind() fail.
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

      m_hostname = p_hostname;
      m_port = p_port;
      l_infos.sin_addr.s_addr = inet_addr(m_hostname.c_str());
      l_infos.sin_port = htons(m_port);
      l_infos.sin_family = ENETSOCKET_FAMILY;
      if (SOCKET_ERROR != ::bind(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR)))
      {
        m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_BOUND | (m_flags & ENETSOCKET_FLAGS_PROTOCOLS));
      }
      else
      {
        mEERROR_SA(EERROR_NET_SOCKET_BIND, getWSAErrString());
      }
    }
  }

  /**
    @brief Put the ENetSocket in listening state.
    @details ENetSocket must be in state of ENETSOCKET_FLAGS_STATE_BOUND.
    @details ENetSocket protocol must be ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @eerror EERROR_NONE in success. State is set ENETSOCKET_FLAGS_STATE_LISTENING.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATE_BOUND.
    @eerror EERROR_NET_SOCKET_PROTOCOL if ENetSocket protocol is not ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @eerror EERROR_NET_SOCKET_LISTEN if listen() fail.
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
        m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_LISTENING | (m_flags & ENETSOCKET_FLAGS_PROTOCOLS));
      }
      else
      {
        mEERROR_SA(EERROR_NET_SOCKET_LISTEN, getWSAErrString());
      }
    }
  }

  /**
    @brief Accept incoming connection to the ENetSocket. This is a blocking function.
    @details ENetSocket must be in state ENETSOCKET_FLAGS_STATE_LISTENING.
    @return Pointer to ENetSocket client connected to this ENetSocket.
    @return nullptr in case of error.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state of ENETSOCKET_FLAGS_STATE_LISTENING.
    @eerror EERROR_OOM if ENetSocket client allocation fail.
    @eerror EERROR_NET_SOCKET_ACCEPT if accept() fail.
  */
  ENetSocket            *ENetSocket::accept()
  {
    ENetSocket          *l_client = nullptr;
    int32               l_infosLen = sizeof(SOCKADDR_IN);

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

        l_client->m_socket = ::accept(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), &l_infosLen);
        if (INVALID_SOCKET != l_client->m_socket)
        {
          l_client->m_hostname = inet_ntoa(l_infos.sin_addr);
          l_client->m_port = l_infos.sin_port;
          l_client->m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_CONNECTED | (m_flags & ENETSOCKET_FLAGS_PROTOCOLS));
        }
        else
        {
          delete (l_client);
          l_client = nullptr;
          mEERROR_SA(EERROR_NET_SOCKET_ACCEPT, getWSAErrString());
        }
      }
      else
      {
        mEERROR_S(EERROR_OOM);
      }
    }

    return (l_client);
  }

  /**
    @brief Connect the ENetSocket to a internet address.
    @details The ENetSocket must be in state ENETSOCKET_FLAGS_STATE_INITIALIZED.
    @details The ENetSocket protocol must be ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success. State is set ENETSOCKET_FLAGS_STATE_CONNECTED.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATE_INITIALIZED.
    @eerror EERROR_NET_SOCKET_PROTOCOL if ENetSocket protocol is not ENETSOCKET_FLAGS_PROTOCOL_TCP.
    @eerror EERROR_NET_SOCKET_CONNECT if connect() fail.
  */
  void                  ENetSocket::connect(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_INITIALIZED != (m_flags & ENETSOCKET_FLAGS_STATES))
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

      m_hostname = p_hostname;
      m_port = p_port;
      l_infos.sin_addr.s_addr = inet_addr(m_hostname.c_str());
      l_infos.sin_port = htons(m_port);
      l_infos.sin_family = ENETSOCKET_FAMILY;
      if (SOCKET_ERROR != ::connect(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR)))
      {
        m_flags = static_cast<ENetSocketFlags>(ENETSOCKET_FLAGS_STATE_CONNECTED | (m_flags & ENETSOCKET_FLAGS_PROTOCOLS));
      }
      else
      {
        mEERROR_SA(EERROR_NET_SOCKET_CONNECT, getWSAErrString());
      }
    }
  }
  
  /**
    @brief Receive datas from a connected ENetSocket.
    @details ENetSocket must be in state ENETSOCKET_FLAGS_STATUS_CONNECTED.
    @param p_datas Pointer to buffer to receive the incoming datas.
    @param p_len Length of p_datas buffer.
    @return Length of datas received in success.
    @return 0 if ENetSocket disconnected. ENetSocket is automatically closed.
    @return -1 in failure.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATUS_CONNECTED.
    @eerror EERROR_NET_SOCKET_RECV if recv() fail.
    @eerror EERROR_NULL_PTR is p_datas is null.
  */
  int32                 ENetSocket::recv(char *p_datas, uint16 p_len)
  {
    int32               l_len = SOCKET_ERROR;

    if (0 != p_len)
    {
      mEERROR_R();
      if (ENETSOCKET_FLAGS_STATE_CONNECTED != (m_flags & ENETSOCKET_FLAGS_STATES))
      {
        mEERROR_S(EERROR_NET_SOCKET_STATE);
      }
      if (nullptr == p_datas)
      {
        mEERROR_S(EERROR_NULL_PTR);
      }

      if (EERROR_NONE == mEERROR)
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
          mEERROR_SA(EERROR_NET_SOCKET_RECV, getWSAErrString());
        }
      }
    }
    else
    {
      l_len = 0;
    }

    return (l_len);
  }

  /**
    @brief Receive datas from a connectionless ENetSocket.
    @details The ENetSocket must be in state ENETSOCKET_FLAGS_STATUS_BOUND.
    @details Receive is ignored if ENetSocket source is null or if ENetSocket source state is not ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @param p_datas Pointer to buffer to receive the incoming datas.
    @param p_len Length of p_datas buffer.
    @param p_src Pointer to ENetSocket that will hold the informations of the source.
    @return Length of datas received in success.
    @return -1 in failure.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATUS_BOUND.
    @eerror EERROR_NET_SOCKET_INVALID if p_src is null or if its state is not ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_NET_SOCKET_RECVFROM if recvfrom() fail.
    @eerror EERROR_NULL_PTR is p_datas is null.
  */
  int32                 ENetSocket::recvfrom(char *p_datas, uint16 p_len, ENetSocket *p_src)
  {
    int32               l_infosLen = sizeof(SOCKADDR_IN);
    int32               l_len = SOCKET_ERROR;

    if (0 != p_len)
    {
      mEERROR_R();
      if (ENETSOCKET_FLAGS_STATE_BOUND != (m_flags & ENETSOCKET_FLAGS_STATES))
      {
        mEERROR_S(EERROR_NET_SOCKET_STATE);
      }
      if ((nullptr == p_src)
        || (ENETSOCKET_FLAGS_STATE_UNINITIALIZED != (p_src->m_flags & ENETSOCKET_FLAGS_STATES)))
      {
        mEERROR_S(EERROR_NET_SOCKET_INVALID);
      }
      if (nullptr == p_datas)
      {
        mEERROR_S(EERROR_NULL_PTR);
      }

      if (EERROR_NONE == mEERROR)
      {
        SOCKADDR_IN       l_infos = { 0 };

        l_len = ::recvfrom(m_socket, p_datas, p_len, 0, reinterpret_cast<SOCKADDR*>(&l_infos), &l_infosLen);
        if (SOCKET_ERROR == l_len)
        {
          p_src->m_hostname = inet_ntoa(l_infos.sin_addr);
          p_src->m_port = l_infos.sin_port;
        }
        else
        {
          mEERROR_SA(EERROR_NET_SOCKET_RECVFROM, getWSAErrString());
        }
      }
    }
    else
    {
      l_len = 0;
    }

    return (l_len);
  }

  /**
    @brief Send datas to a connected ENetSocket.
    @details The ENetSocket must be in state ENETSOCKET_FLAGS_STATUS_CONNECTED.
    @param p_datas Pointer to buffer of datas to be send.
    @param p_len Length of p_datas buffer.
    @return Length of datas sent in success.
    @return -1 in failure.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATUS_CONNECTED.
    @eerror EERROR_NET_SOCKET_SEND if send() fail.
    @eerror EERROR_NULL_PTR is p_datas is null.
  */
  int32                 ENetSocket::send(const char *p_datas, uint16 p_len)
  {
    int32               l_infosLen = sizeof(SOCKADDR_IN);
    int32               l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_CONNECTED != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_len = ::send(m_socket, p_datas, p_len, 0);
      if (SOCKET_ERROR == l_len)
      {
        mEERROR_SA(EERROR_NET_SOCKET_SEND, getWSAErrString());
      }
    }

    return (l_len);
  }

  /**
    @brief Send datas to a connectionless ENetSocket.
    @details The ENetSocket must be in state ENETSOCKET_FLAGS_STATUS_BOUND.
    @details Send is ignored if ENetSocket destination is null.
    @param p_datas Pointer to buffer of datas to be send.
    @param p_len Length of p_datas buffer.
    @param p_src Pointer to ENetSocket that hold the informations of the destination.
    @return Length of datas sent in success.
    @return -1 in failure.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is not in state ENETSOCKET_FLAGS_STATUS_BOUND.
    @eerror EERROR_NET_SOCKET_INVALID if p_dst is null.
    @eerror EERROR_NET_SOCKET_SENDTO if sendto() fail.
    @eerror EERROR_NULL_PTR is p_datas is null.
  */
  int32                 ENetSocket::sendto(const char *p_datas, uint16 p_len, const ENetSocket *p_dst)
  {
    int32               l_infosLen = sizeof(SOCKADDR_IN);
    int32               l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ENETSOCKET_FLAGS_STATE_BOUND != (m_flags & ENETSOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if (nullptr == p_dst)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      SOCKADDR_IN       l_infos = { 0 };

      l_infos.sin_addr.s_addr = inet_addr(p_dst->m_hostname.c_str());
      l_infos.sin_port = htons(p_dst->m_port);
      l_infos.sin_family = ENETSOCKET_FAMILY;
      l_len = ::sendto(m_socket, p_datas, p_len, 0, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR_IN));
      if (SOCKET_ERROR == l_len)
      {
        mEERROR_SA(EERROR_NET_SOCKET_SENDTO, getWSAErrString());
      }
    }

    return (l_len);
  }

  /**
    @brief Shutdown a service of the ENetSocket.
    @details The ENetSocket must not be in state ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @param p_service Describes the types of service to be shutdown.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is in state ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_NET_SOCKET_SHUTDOWN if shutdown() fail.
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
        mEERROR_SA(EERROR_NET_SOCKET_SHUTDOWN, getWSAErrString());
      }
    }
  }
  
  /**
    @brief Close the ENetSocket.
    @details ENetSocket must not be in state of ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_NONE in success. State is set to ENETSOCKET_FLAGS_STATUS_UNINITIALIZED.
    @eerror EERROR_NET_SOCKET_STATE if ENetSocket is in state of ENETSOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_NET_SOCKET_CLOSE if closesocket() fail.
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
      if (SOCKET_ERROR == ::closesocket(m_socket))
      {
        mEERROR_SA(EERROR_NET_SOCKET_CLOSE, getWSAErrString());
      }
      m_flags = ENETSOCKET_FLAGS_STATE_UNINITIALIZED;
    }
  }

  /**
    @brief Get Internet host address in number-and-dots notation.
    @return Internet host address in number-and-dots notation of the ENetSocket.
  */
  const std::string     &ENetSocket::getHostname() const
  {
    return (m_hostname);
  }

  /**
    @brief Get Internet host port.
    @return Port of the ENetSocket.
  */
  uint16                ENetSocket::getPort() const
  {
    return (m_port);
  }

  /**
    @brief Get flags of the ENetSocket.
    @return Flags of the ENetSocket.
  */
  ENetSocketFlags       ENetSocket::getFlags() const
  {
    return (m_flags);
  }

  /**
    @brief Convert a ENetSocket to a unique identifier.
    @return Identifier of the ENetSocket.
  */
  ENetSocket::operator  uint64() const
  {
    return (static_cast<uint64>(m_socket));
  }

  /**
    @brief Retrieve ENetSocket informations.
    @return Informations of the ENetSocket.
  */
  const std::string     ENetSocket::toString() const
  {
    std::string         str = "";

    str = "ENetSocket: " + std::to_string(m_socket);

    return (str);
  }

}
