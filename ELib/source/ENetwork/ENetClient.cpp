/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetClient Class.
*/

#include "ENetwork/ENetClient.h"

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief Functor for ENetClient::recvfrom thread.
    @param p_unused Unused.
    @return Unused.
  */
  DWORD WINAPI          ClientRecvfromFunctor(LPVOID p_unused)
  {
    mEERROR_R();
    if (nullptr != ENetClient::getInstance())
    {
      ENetClient::getInstance()->recvfrom();
    }
    else
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    return (0);
  }

  /**
    @brief Functor for ENetClient::recv thread.
    @param p_unused Unused.
    @return Unused.
  */
  DWORD WINAPI          ClientRecvFunctor(LPVOID p_unused)
  {
    mEERROR_R();
    if (nullptr != ENetClient::getInstance())
    {
      ENetClient::getInstance()->recv();
    }
    else
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    return (0);
  }

  /**
    @brief Constructor for ENetClient.
  */
  ENetClient::ENetClient() :
    m_socketRecvfrom(),
    m_threadRecvfrom(nullptr),
    m_socketRecv(),
    m_threadRecv(nullptr),
    m_isRunning(false)
  {
  }

  /**
    @brief Destructor for ENetClient.
    @details Terminate its threads, close its ENetSockets and call WSACleanup().
  */
  ENetClient::~ENetClient()
  {
    m_socketRecvfrom.close();
    TerminateThread(m_threadRecvfrom, 0);
    CloseHandle(m_threadRecvfrom);
    m_socketRecv.close();
    TerminateThread(m_threadRecv, 0);
    CloseHandle(m_threadRecv);
    WSACleanup();
  }

  /**
    @brief Singleton for ENetClient. /!\ EError.
    @details Call WSAStartup().
    @return ENetClient unique instance on success.
    @return nullptr on failure.
  */
  ENetClient            *ENetClient::getInstance()
  {
    static ENetClient   *l_instance = nullptr;

    mEERROR_R();
    if (nullptr == l_instance)
    {
      WSADATA           WSAData = { 0 };

      if (0 == WSAStartup(MAKEWORD(2, 2), &WSAData))
      {
        l_instance = new ENetClient();
        if (nullptr == l_instance)
        {
          mEERROR_S(EERROR_MEMORY);
        }
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
      }
    }

    return (l_instance);
  }

  /**
    @brief Initialize ENetClient. /!\ EError.
    @details Prepare UDP ENetSocket for ENetClient::recvfrom().
    @details Prepare TCP ENetSocket for ENetClient::recv().
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
  */
  void                  ENetClient::init(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_NET_CLIENT_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_socketRecvfrom.socket(ENETSOCKET_FLAGS_PROTOCOL_UDP);
      if (EERROR_NONE == mEERROR)
      {
        m_socketRecvfrom.bind("0.0.0.0", 0);
        if (EERROR_NONE == mEERROR)
        {
          mEPRINT_STD("ENetClient: UDP client ready on 0.0.0.0:0.");
        }
        else
        {
          mEERROR_SH(EERROR_NET_SOCKET_ERR);
        }
      }
      else
      {
        mEERROR_SH(EERROR_NET_SOCKET_ERR);
      }
    }
    if (EERROR_NONE == mEERROR)
    {
      m_socketRecv.socket(ENETSOCKET_FLAGS_PROTOCOL_TCP);
      if (EERROR_NONE == mEERROR)
      {
        m_socketRecv.connect(p_hostname, p_port);
        if (EERROR_NONE == mEERROR)
        {
          mEPRINT_STD("ENetClient: TCP client connected to " + p_hostname + ":" + std::to_string(p_port) + ".");
        }
        else
        {
          mEERROR_SH(EERROR_NET_SOCKET_ERR);
        }
      }
      else
      {
        mEERROR_SH(EERROR_NET_SOCKET_ERR);
      }
    }
  }

  /**
    @brief Start ENetClient automation. /!\ EError.
    @details Create threads for ENetClient::recvfrom() and ENetClient:recv().
    @details ENetPacketHandler Singleton need to be valid.
  */
  void                  ENetClient::start()
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_NET_CLIENT_STATE);
    }
    if (nullptr == ENetPacketHandler::getInstance())
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_threadRecvfrom = CreateThread(nullptr, 0, ClientRecvfromFunctor, nullptr, 0, nullptr);
      if (nullptr != m_threadRecvfrom)
      {
        m_threadRecv = CreateThread(nullptr, 0, ClientRecvFunctor, nullptr, 0, nullptr);
        if (nullptr != m_threadRecv)
        {
          m_isRunning = true;
          mEPRINT_STD("ENetClient: Started successfully.");
        }
        else
        {
          TerminateThread(m_threadRecvfrom, 0);
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(GetLastError()));
        }
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(GetLastError()));
      }
    }
  }

  /**
    @brief Stop ENetClient automation. /!\ EError.
    @details Terminate its threads.
  */
  void                  ENetClient::stop()
  {
    mEERROR_R();
    if (false == isRunning())
    {
      mEERROR_S(EERROR_NET_SERVER_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_isRunning = false;
      TerminateThread(m_threadRecvfrom, 0);
      TerminateThread(m_threadRecv, 0);
      mEPRINT_STD("ENetClient: Stopped successfully.");
    }
  }

  /**
    @brief Receive connectionless datas to ENetClient. /!\ Blocking. /!\ Mutex. /!\ EError.
    @details Receive datas on connectionless ENetSocket and send them to ENetPacketHandler::read().
    @details ENetPacketHandler Singleton need to be valid.
  */
  void                  ENetClient::recvfrom()
  {
    while (true == isRunning())
    {
      mEERROR_R();
      if (nullptr != ENetPacketHandler::getInstance())
      {
        mEERROR_SH(EERROR_NULL_PTR);
        stop();
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SH(EERROR_NET_CLIENT_ERR);
        }
      }

      if (EERROR_NONE == mEERROR)
      {
        ENetSocket      *l_client = nullptr;

        l_client = new ENetSocket();
        if (nullptr != l_client)
        {
          char          l_datas[ENETSOCKET_UDP_MAX];
          int32         l_len = -1;

          l_len = m_socketRecvfrom.recvfrom(l_datas, ENETSOCKET_UDP_MAX, l_client);
          /* TODO: Check that source correspond to ENetServer. */
          if (0 < l_len)
          {
            ENetPacketHandler::getInstance()->read(l_datas, l_len, l_client);
            if (EERROR_NONE != mEERROR)
            {
              mEERROR_SH(EERROR_NET_PACKETHANDLER_ERR);
              delete (l_client);
            }
          }
          else
          {
            mEERROR_SH(EERROR_NET_SOCKET_ERR);
          }
        }
        else
        {
          mEERROR_S(EERROR_MEMORY);
        }
      }
    }
  }

  /**
    @brief Receive connected datas to ENetClient. /!\ Blocking. /!\ Mutex. /!\ EError.
    @details Call ENetPacketHandler::recvPacket().
    @details ENetPacketHandler Singleton need to be valid.
    @details On error, m_socketRecv is closed and ENetClient::stop() is called.
  */
  void                  ENetClient::recv()
  {
    while (true == m_isRunning)
    {
      mEERROR_R();
      if (nullptr != ENetPacketHandler::getInstance())
      {
        mEERROR_SH(EERROR_NULL_PTR);
        stop();
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SH(EERROR_NET_CLIENT_ERR);
        }
      }

      if (EERROR_NONE == mEERROR)
      {
        ENetPacketHandler::getInstance()->recvPacket(&m_socketRecv);
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SH(EERROR_NET_PACKETHANDLER_ERR);
          m_socketRecv.close();
          if (EERROR_NONE == mEERROR)
          {
            stop();
            if (EERROR_NONE != mEERROR)
            {
              mEERROR_SH(EERROR_NET_CLIENT_ERR);
            }
          }
          else
          {
            mEERROR_SH(EERROR_NET_SOCKET_ERR);
          }
        }
      }
    }
  }

  /**
    @brief Send ENetPacket to connected ENetSocket.
    @details ENetClient need to be running.
    @param p_packet Packet to be send.
  */
  void                  ENetClient::send(ENetPacket *p_packet)
  {
    mEERROR_R();
    if (false == m_isRunning)
    {
      mEERROR_S(EERROR_NET_CLIENT_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      p_packet->setSource(&m_socketRecv);
      p_packet->send();
      if (EERROR_NONE != mEERROR)
      {
        mEERROR_SH(EERROR_NET_PACKET_ERR);
      }
    }
  }

  /**
    @brief Get state of ENetClient.
    @return State.
  */
  bool                  ENetClient::isRunning()
  {
    return (m_isRunning);
  }

}