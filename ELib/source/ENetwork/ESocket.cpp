/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ESocket Class.
*/

//TODO => Check that close error reset doesn't erase usefull error logs.

#include "ENetwork/ESocket.h"

/**
  @brief General scope for ELib components.
*/
namespace             ELib
{

  /**
    @brief Instantiate a uninitialized ESocket.
    @details ESocket default state is ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @details ESocket default protocol is ESOCKET_FLAGS_PROTOCOL_UNDEFINED.
  */
  ESocket::ESocket() :
    m_socket(INVALID_SOCKET),
    m_hostname("0.0.0.0"),
    m_port(0),
    m_flags(static_cast<ESocketFlags>(ESOCKET_FLAGS_STATE_UNINITIALIZED | ESOCKET_FLAGS_PROTOCOL_UNDEFINED))
  {
  }
  
  /**
    @brief Delete a ESocket.
    @details ESocket is closed if not yet done.
  */
  ESocket::~ESocket()
  {
    if (INVALID_SOCKET != m_socket)
    {
      close();
    }
  }

  /**
    @brief Initialize the ESocket depending on p_protocol from parameter.
    @details ESocket must be in state of ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @details p_protocol must be a flag in range of ESOCKET_FLAGS_PROTOCOLS.
    @param p_protocol Protocol to be used to initialize the ESocket.
    @eerror EERROR_NONE in success. State is set ESOCKET_FLAGS_STATE_INITIALIZED.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_SOCKET_PROTOCOL if parameter p_protocol is not in range of ESOCKET_FLAGS_PROTOCOL.
    @eerror EERROR_SOCKET_INVALID if socket() fail.
  */
  void                ESocket::socket(ESocketFlags p_protocol)
  {
    mEERROR_R();
    p_protocol = static_cast<ESocketFlags>(p_protocol & ESOCKET_FLAGS_PROTOCOLS);
    if (ESOCKET_FLAGS_STATE_UNINITIALIZED != (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }
    if (ESOCKET_FLAGS_PROTOCOL_UNDEFINED == p_protocol)
    {
      mEERROR_S(EERROR_SOCKET_PROTOCOL);
    }
    
    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      switch (p_protocol)
      {
        case ESOCKET_FLAGS_PROTOCOL_TCP:
        {
          m_socket = ::socket(ESOCKET_FAMILY, SOCK_STREAM, IPPROTO_TCP);
        }
          break;
        case ESOCKET_FLAGS_PROTOCOL_UDP:
        {
          m_socket = ::socket(ESOCKET_FAMILY, SOCK_DGRAM, IPPROTO_UDP);
        }
          break;
        default:
          break;
      }
      if (INVALID_SOCKET != m_socket)
      {
        m_flags = static_cast<ESocketFlags>(ESOCKET_FLAGS_STATE_INITIALIZED | p_protocol);
      }
      else
      {
        mEERROR_SA(EERROR_SOCKET_INVALID, getWSAErrString());
      }
    }
  }

  /**
    @brief Bind the ESocket to a local internet address.
    @details ESocket must be in state ESOCKET_FLAGS_STATE_INITIALIZED.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success. State is set ESOCKET_FLAGS_STATE_BOUND.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATE_INITIALIZED.
    @eerror EERROR_SOCKET_BIND if bind() fail.
  */
  void                ESocket::bind(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_INITIALIZED != (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      SOCKADDR_IN     l_infos = { 0 };

      m_hostname = p_hostname;
      m_port = p_port;
      l_infos.sin_addr.s_addr = inet_addr(m_hostname.c_str());
      l_infos.sin_port = htons(m_port);
      l_infos.sin_family = ESOCKET_FAMILY;
      if (SOCKET_ERROR != ::bind(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR)))
      {
        m_flags = static_cast<ESocketFlags>(ESOCKET_FLAGS_STATE_BOUND | (m_flags & ESOCKET_FLAGS_PROTOCOLS));
      }
      else
      {
        mEERROR_SA(EERROR_SOCKET_BIND, getWSAErrString());
      }
    }
  }

  /**
    @brief Put the ESocket in listening state.
    @details ESocket must be in state of ESOCKET_FLAGS_STATE_BOUND.
    @details ESocket protocol must be ESOCKET_FLAGS_PROTOCOL_TCP.
    @eerror EERROR_NONE in success. State is set ESOCKET_FLAGS_STATE_LISTENING.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATE_BOUND.
    @eerror EERROR_SOCKET_PROTOCOL if ESocket protocol is not ESOCKET_FLAGS_PROTOCOL_TCP.
    @eerror EERROR_SOCKET_LISTEN if listen() fail.
  */
  void                ESocket::listen()
  {
    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_BOUND != (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }
    if (ESOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ESOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_SOCKET_PROTOCOL);
    }
    
    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      if (SOCKET_ERROR != ::listen(m_socket, MAX_CLIENTS))
      {
        m_flags = static_cast<ESocketFlags>(ESOCKET_FLAGS_STATE_LISTENING | (m_flags & ESOCKET_FLAGS_PROTOCOLS));
      }
      else
      {
        mEERROR_SA(EERROR_SOCKET_LISTEN, getWSAErrString());
      }
    }
  }

  /**
    @brief Accept incoming connection to the ESocket. This is a blocking function.
    @details ESocket must be in state ESOCKET_FLAGS_STATE_LISTENING.
    @return Pointer to ESocket client connected to this ESocket.
    @return nullptr in case of error.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state of ESOCKET_FLAGS_STATE_LISTENING.
    @eerror EERROR_OOM if ESocket client allocation fail.
    @eerror EERROR_SOCKET_ACCEPT if accept() fail.
  */
  ESocket             *ESocket::accept()
  {
    ESocket           *l_client = nullptr;
    int32             l_infosLen = sizeof(SOCKADDR_IN);

    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_LISTENING != (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }
    if (ESOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ESOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_SOCKET_PROTOCOL);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      l_client = new ESocket();
      if (nullptr != l_client)
      {
        SOCKADDR_IN   l_infos = { 0 };

        l_client->m_socket = ::accept(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), &l_infosLen);
        if (INVALID_SOCKET != l_client->m_socket)
        {
          l_client->m_hostname = inet_ntoa(l_infos.sin_addr);
          l_client->m_port = l_infos.sin_port;
          l_client->m_flags = static_cast<ESocketFlags>(ESOCKET_FLAGS_STATE_CONNECTED | (m_flags & ESOCKET_FLAGS_PROTOCOLS));
        }
        else
        {
          delete (l_client);
          l_client = nullptr;
          mEERROR_SA(EERROR_SOCKET_ACCEPT, getWSAErrString());
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
    @brief Connect the ESocket to a internet address.
    @details The ESocket must be in state ESOCKET_FLAGS_STATE_INITIALIZED.
    @details The ESocket protocol must be ESOCKET_FLAGS_PROTOCOL_TCP.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success. State is set ESOCKET_FLAGS_STATE_CONNECTED.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATE_INITIALIZED.
    @eerror EERROR_SOCKET_PROTOCOL if ESocket protocol is not ESOCKET_FLAGS_PROTOCOL_TCP.
    @eerror EERROR_SOCKET_CONNECT if connect() fail.
  */
  void                ESocket::connect(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_INITIALIZED != (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }
    if (ESOCKET_FLAGS_PROTOCOL_TCP != (m_flags & ESOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_SOCKET_PROTOCOL);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      SOCKADDR_IN     l_infos = { 0 };

      m_hostname = p_hostname;
      m_port = p_port;
      l_infos.sin_addr.s_addr = inet_addr(m_hostname.c_str());
      l_infos.sin_port = htons(m_port);
      l_infos.sin_family = ESOCKET_FAMILY;
      if (SOCKET_ERROR != ::connect(m_socket, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR)))
      {
        m_flags = static_cast<ESocketFlags>(ESOCKET_FLAGS_STATE_CONNECTED | (m_flags & ESOCKET_FLAGS_PROTOCOLS));
      }
      else
      {
        mEERROR_SA(EERROR_SOCKET_CONNECT, getWSAErrString());
      }
    }
  }
  
  /**
    @brief Receive datas from a connected ESocket.
    @details ESocket must be in state ESOCKET_FLAGS_STATUS_CONNECTED.
    @param p_datas Pointer to buffer to receive the incoming datas.
    @param p_len Length of p_datas buffer.
    @return Length of datas received in success.
    @return 0 if ESocket disconnected. ESocket is automatically closed.
    @return -1 in failure.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATUS_CONNECTED.
    @eerror EERROR_SOCKET_RECV if p_datas is null or recv() fail.
  */
  int32               ESocket::recv(char *p_datas, uint16 p_len)
  {
    int32             l_len = SOCKET_ERROR;

    if (0 != p_len)
    {
      mEERROR_R();
      if (ESOCKET_FLAGS_STATE_CONNECTED != (m_flags & ESOCKET_FLAGS_STATES))
      {
        mEERROR_S(EERROR_SOCKET_STATE);
      }
      if (nullptr == p_datas)
      {
        mEERROR_S(EERROR_SOCKET_RECV);
      }

      if (EERROR_NONE == mEERROR_G.m_errorCode)
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
          mEERROR_SA(EERROR_SOCKET_RECV, getWSAErrString());
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
    @brief Receive datas from a connectionless ESocket.
    @details The ESocket must be in state ESOCKET_FLAGS_STATUS_BOUND.
    @details Receive is ignored if ESocket source is null or if ESocket source state is not ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @param p_datas Pointer to buffer to receive the incoming datas.
    @param p_len Length of p_datas buffer.
    @param p_src Pointer to ESocket that will hold the informations of the source.
    @return Length of datas received in success.
    @return -1 in failure.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATUS_BOUND.
    @eerror EERROR_SOCKET_INVALID if p_src is null or if its state is not ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_SOCKET_RECVFROM if p_datas is null or recvfrom() fail.
  */
  int32               ESocket::recvfrom(char *p_datas, uint16 p_len, ESocket *p_src)
  {
    int32             l_infosLen = sizeof(SOCKADDR_IN);
    int32             l_len = SOCKET_ERROR;

    if (0 != p_len)
    {
      mEERROR_R();
      if (ESOCKET_FLAGS_STATE_BOUND != (m_flags & ESOCKET_FLAGS_STATES))
      {
        mEERROR_S(EERROR_SOCKET_STATE);
      }
      if ((nullptr == p_src)
        || (ESOCKET_FLAGS_STATE_UNINITIALIZED != (p_src->m_flags & ESOCKET_FLAGS_STATES)))
      {
        mEERROR_S(EERROR_SOCKET_INVALID);
      }
      if (nullptr == p_datas)
      {
        mEERROR_S(EERROR_SOCKET_RECVFROM);
      }

      if (EERROR_NONE == mEERROR_G.m_errorCode)
      {
        SOCKADDR_IN     l_infos = { 0 };

        l_len = ::recvfrom(m_socket, p_datas, p_len, 0, reinterpret_cast<SOCKADDR*>(&l_infos), &l_infosLen);
        if (SOCKET_ERROR == l_len)
        {
          p_src->m_hostname = inet_ntoa(l_infos.sin_addr);
          p_src->m_port = l_infos.sin_port;
        }
        else
        {
          mEERROR_SA(EERROR_SOCKET_RECVFROM, getWSAErrString());
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
    @brief Send datas to a connected ESocket.
    @details The ESocket must be in state ESOCKET_FLAGS_STATUS_CONNECTED.
    @param p_datas Pointer to buffer of datas to be send.
    @param p_len Length of p_datas buffer.
    @return Length of datas sent in success.
    @return -1 in failure.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATUS_CONNECTED.
    @eerror EERROR_SOCKET_SEND if p_datas is null or send() fail.
  */
  int32               ESocket::send(const char *p_datas, uint16 p_len)
  {
    int32             l_infosLen = sizeof(SOCKADDR_IN);
    int32             l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_CONNECTED != (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_SOCKET_SEND);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      l_len = ::send(m_socket, p_datas, p_len, 0);
      if (SOCKET_ERROR == l_len)
      {
        mEERROR_SA(EERROR_SOCKET_SEND, getWSAErrString());
      }
    }

    return (l_len);
  }

  /**
    @brief Send datas to a connectionless ESocket.
    @details The ESocket must be in state ESOCKET_FLAGS_STATUS_BOUND.
    @details Send is ignored if ESocket destination is null.
    @param p_datas Pointer to buffer of datas to be send.
    @param p_len Length of p_datas buffer.
    @param p_src Pointer to ESocket that hold the informations of the destination.
    @return Length of datas sent in success.
    @return -1 in failure.
    @eerror EERROR_SOCKET_STATE if ESocket is not in state ESOCKET_FLAGS_STATUS_BOUND.
    @eerror EERROR_SOCKET_INVALID if p_dst is null.
    @eerror EERROR_SOCKET_SENDTO if p_datas is null or sendto() fail.
  */
  int32               ESocket::sendto(const char *p_datas, uint16 p_len, const ESocket *p_dst)
  {
    int32             l_infosLen = sizeof(SOCKADDR_IN);
    int32             l_len = SOCKET_ERROR;

    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_BOUND != (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }
    if (nullptr == p_dst)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_SOCKET_SENDTO);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      SOCKADDR_IN     l_infos = { 0 };

      l_infos.sin_addr.s_addr = inet_addr(p_dst->m_hostname.c_str());
      l_infos.sin_port = htons(p_dst->m_port);
      l_infos.sin_family = ESOCKET_FAMILY;
      l_len = ::sendto(m_socket, p_datas, p_len, 0, reinterpret_cast<SOCKADDR*>(&l_infos), sizeof(SOCKADDR_IN));
      if (SOCKET_ERROR == l_len)
      {
        mEERROR_SA(EERROR_SOCKET_SENDTO, getWSAErrString());
      }
    }

    return (l_len);
  }

  /**
    @brief Shutdown a service of the ESocket.
    @details The ESocket must not be in state ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @param p_service Describes the types of service to be shutdown.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SOCKET_STATE if ESocket is in state ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_SOCKET_SHUTDOWN if shutdown() fail.
  */
  void                ESocket::shutdown(ESocketService p_service)
  {
    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_UNINITIALIZED == (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      if (SOCKET_ERROR == ::shutdown(m_socket, p_service))
      {
        mEERROR_SA(EERROR_SOCKET_SHUTDOWN, getWSAErrString());
      }
    }
  }
  
  /**
    @brief Close the ESocket.
    @details ESocket must not be in state of ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_NONE in success. State is set to ESOCKET_FLAGS_STATUS_UNINITIALIZED.
    @eerror EERROR_SOCKET_STATE if ESocket is in state of ESOCKET_FLAGS_STATE_UNINITIALIZED.
    @eerror EERROR_SOCKET_CLOSE if closesocket() fail.
  */
  void                ESocket::close()
  {
    mEERROR_R();
    if (ESOCKET_FLAGS_STATE_UNINITIALIZED == (m_flags & ESOCKET_FLAGS_STATES))
    {
      mEERROR_S(EERROR_SOCKET_STATE);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      if (SOCKET_ERROR == ::closesocket(m_socket))
      {
        mEERROR_SA(EERROR_SOCKET_CLOSE, getWSAErrString());
      }
      m_flags = ESOCKET_FLAGS_STATE_UNINITIALIZED;
    }
  }

  /**
    @brief Get Internet host address in number-and-dots notation.
    @return Internet host address in number-and-dots notation of the ESocket.
  */
  const std::string   &ESocket::getHostname() const
  {
    return (m_hostname);
  }

  /**
    @brief Get Internet host port.
    @return Port of the ESocket.
  */
  uint16              ESocket::getPort() const
  {
    return (m_port);
  }

  /**
    @brief Get flags of the ESocket.
    @return Flags of the ESocket.
  */
  ESocketFlags        ESocket::getFlags() const
  {
    return (m_flags);
  }

  /**
    @brief Convert a ESocket to a unique identifier.
    @return Identifier of the ESocket.
  */
  ESocket::operator   uint64() const
  {
    return (static_cast<uint64>(m_socket));
  }

  /**
    @brief Retrieve ESocket informations.
    @return Informations of the ESocket.
  */
  const std::string   ESocket::toString() const
  {
    std::string       str = "";

    str = "ESocket: " + std::to_string(m_socket);

    return (str);
  }

}
