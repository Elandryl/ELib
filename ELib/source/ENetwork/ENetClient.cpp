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
    @brief Functor for recvPacket thread.
    @param p_unused Unused.
    @return Unused.
  */
  DWORD WINAPI          RecvPacketFunctor(LPVOID p_unused)
  {
    ENetClient::getInstance()->recvPacket();
    return (0);
  }

  /**
    @brief Singleton: Instantiate a ENetClient.
  */
  ENetClient::ENetClient() :
    m_socketServer(),
    m_threadRecvPacket(nullptr),
    m_packetHandler(),
    m_isRunning(false)
  {
  }

  /**
    @brief Delete a ENetClient.
    @details Terminate its thread, close the server ENetSocket and close WSA.
  */
  ENetClient::~ENetClient()
  {
    m_socketServer.close();
    TerminateThread(m_threadRecvPacket, 0);
    CloseHandle(m_threadRecvPacket);
    WSACleanup();
  }

  /**
    @brief Retrieve Singleton of ENetClient.
    @details Call WSAStartup at first call.
    @return Unique instance of ENetClient.
    @eerror EERROR_NET_WSA_STARTUP if WSAStartup() fail.
  */
  ENetClient            *ENetClient::getInstance()
  {
    static ENetClient   *l_instance = nullptr;

    if (nullptr == l_instance)
    {
      WSADATA           WSAData = { 0 };

      if (0 != WSAStartup(MAKEWORD(2, 2), &WSAData))
      {
        mEERROR_S(EERROR_NET_WSA_STARTUP);
      }
      l_instance = new ENetClient();
    }

    return (l_instance);
  }

  /**
    @brief Initialize the ENetClient.
    @details Prepare the ENetSocket server to receive incoming datas on specific hostname.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_CLIENT_RUNNING if ENetClient is already running.
    @eerror EERROR_NET_CLIENT_INIT if socket() or connect() fail.
  */
  void                  ENetClient::init(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_NET_CLIENT_RUNNING);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_socketServer.socket(ENETSOCKET_FLAGS_PROTOCOL_TCP);
      if (EERROR_NONE == mEERROR)
      {
        m_socketServer.connect(p_hostname, p_port);
        if (EERROR_NONE == mEERROR)
        {
          mEPRINT_STD("ENetClient successfully connected to EServer at " + p_hostname + ":" + std::to_string(p_port));
        }
        else
        {
          mEERROR_SA(EERROR_NET_CLIENT_INIT, mEERROR_G.toString());
        }
      }
      else
      {
        mEERROR_SA(EERROR_NET_CLIENT_INIT, mEERROR_G.toString());
      }
    }
  }

  /**
    @brief Start the automation of the ENetClient.
    @details Create the thread of the ENetClient and call its recvPacket function.
  */
  void                  ENetClient::start()
  {
    if (false == isRunning())
    {
      m_threadRecvPacket = CreateThread(nullptr, 0, RecvPacketFunctor, nullptr, 0, nullptr);
      m_isRunning = true;
      mEPRINT_STD("ENetClient started");
    }
  }

  /**
    @brief Stop the ENetClient automation.
    @details Terminate the recvPacket function thread.
  */
  void                  ENetClient::stop()
  {
    if (true == isRunning())
    {
      m_isRunning = false;
      TerminateThread(m_threadRecvPacket, 0);
      mEPRINT_STD("ENetClient stopped");
    }
  }

  /**
    @brief Handle the reception of ENetPacket for the ENetSocket server.
    @details Automatically receive the ENetPacket of the ENetSocket server of the ENetClient.
    @details Store the received ENetPackets in the ENetPacketHandler of the ENetClient.
    @return EERROR_NONE in success.
    @return EERROR_NET_CLIENT_STOP if the ENetClient couldn't be stopped properly.
  */
  void                  ENetClient::recvPacket()
  {
    while (true == m_isRunning)
    {
      mEERROR_R();

      m_packetHandler.recvPacket(&m_socketServer);
      if (EERROR_NONE != mEERROR)
      {
        m_socketServer.close();
        stop();
        mEERROR_SA(EERROR_NET_SELECTOR_RECV, mEERROR_G.toString());
      }
    }
  }

  /**
    @brief Handle the sending of ENetPacket to the ENetSocket server.
    @param p_packet Packet to be send.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SELECTOR_SEND if send() failed.
  */
  void                  ENetClient::sendPacket(ENetPacket *p_packet)
  {
    mEERROR_R();
    p_packet->setSource(&m_socketServer);
    p_packet->send();
    if (EERROR_NONE != mEERROR)
    {
      mEERROR_SA(EERROR_NET_SELECTOR_SEND, mEERROR_G.toString());
    }
  }

  /**
    @brief Get the ENetPacketHandler of the ENetClient.
    @return ENetPacketHandler of the ENetClient.
  */
  ENetPacketHandler     &ENetClient::getPacketHandler()
  {
    return (m_packetHandler);
  }

  /**
    @brief Determine if the ENetClient is running.
    @return bool indicating if the ENetClient is running.
  */
  bool                  ENetClient::isRunning()
  {
    return (m_isRunning);
  }

}