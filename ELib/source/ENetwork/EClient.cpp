/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EClient Class.
*/

#include <iostream>
#include "ENetwork/EClient.h"

/**
  @brief General scope for ELib components.
*/
namespace           ELib
{

  /**
    @brief Functor for recvPacket thread.
    @param p_param Pointer to param from CreateThread.
    @return Unused.
  */
  DWORD WINAPI      RecvPacketFunctor(LPVOID p_param)
  {
    static_cast<EClient*>(p_param)->recvPacket();
    return (0);
  }

  /**
    @brief Instantiate a EClient.
    @details Initialize WSA.
    @details It should be unique in an application.
    @ethrow EERROR_WSA_STARTUP if WSAStartup() fail.
  */
  EClient::EClient() :
    m_socketServer(),
    m_threadRecvPacket(nullptr),
    m_packetHandler(),
    m_isRunning(false)
  {
    WSADATA                       WSAData = { 0 };

    if (0 != WSAStartup(MAKEWORD(2, 2), &WSAData))
    {
      mETHROW_S(EERROR_WSA_STARTUP);
    }
  }

  /**
    @brief Delete a EClient.
    @details Terminate its thread, close the server ESocket and close WSA.
  */
  EClient::~EClient()
  {
    m_socketServer.close();
    TerminateThread(m_threadRecvPacket, 0);
    CloseHandle(m_threadRecvPacket);
    WSACleanup();
  }

  /**
    @brief Initialize the EClient.
    @details Prepare the ESocket server to receive incoming datas on specific hostname.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success.
    @eerror EERROR_CLIENT_RUNNING if EClient is already running.
    @eerror EERROR_CLIENT_INIT if socket() or connect() fail.
  */
  void              EClient::init(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_CLIENT_RUNNING);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      m_socketServer.socket(ESOCKET_FLAGS_PROTOCOL_TCP);
      if (EERROR_NONE == mEERROR_G.m_errorCode)
      {
        m_socketServer.connect(p_hostname, p_port);
        if (EERROR_NONE == mEERROR_G.m_errorCode)
        {
          std::cout << "EClient successfully connected to EServer at " << p_hostname << ":" << p_port << std::endl;
        }
        else
        {
          mEERROR_SA(EERROR_CLIENT_INIT, mEERROR_G.toString());
        }
      }
      else
      {
        mEERROR_SA(EERROR_CLIENT_INIT, mEERROR_G.toString());
      }
    }
  }

  /**
    @brief Start the automation of the EClient.
    @details Create the thread of the EClient and call its recvPacket function.
  */
  void              EClient::start()
  {
    if (false == isRunning())
    {
      m_threadRecvPacket = CreateThread(nullptr, 0, RecvPacketFunctor, this, 0, nullptr);
      m_isRunning = true;
      std::cout << "EClient started" << std::endl;
    }
  }

  /**
    @brief Stop the EClient automation.
    @details Terminate the recvPacket function thread.
  */
  void              EClient::stop()
  {
    if (true == isRunning())
    {
      m_isRunning = false;
      TerminateThread(m_threadRecvPacket, 0);
      std::cout << "EClient stopped" << std::endl;
    }
  }

  /**
    @brief Handle the reception of EPacket for the ESocket server.
    @details Automatically receive the EPacket of the ESocket server of the EClient.
    @details Store the received EPackets in the EPacketHandler of the EClient.
    @return EERROR_NONE in success.
    @return EERROR_CLIENT_STOP if the EClient couldn't be stopped properly.
  */
  void              EClient::recvPacket()
  {
    while (true == m_isRunning)
    {
      mEERROR_R();

      m_packetHandler.recvPacket(&m_socketServer);
      if (EERROR_NONE != mEERROR_G.m_errorCode)
      {
        m_socketServer.close();
        stop();
        mEERROR_SA(EERROR_SELECTOR_RECV, mEERROR_G.toString());
      }
    }
  }

  void              EClient::sendPacket(EPacket *p_packet)
  {
    p_packet->setSource(&m_socketServer);
    p_packet->send();
  }

  /**
    @brief Get the EPacketHandler of the EClient.
    @return EPacketHandler of the EClient.
  */
  EPacketHandler    &EClient::getPacketHandler()
  {
    return (m_packetHandler);
  }

  /**
    @brief Determine if the EClient is running.
    @return bool indicating if the EClient is running.
  */
  bool              EClient::isRunning()
  {
    return (m_isRunning);
  }

}