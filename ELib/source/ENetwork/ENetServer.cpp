/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetServer Class.
*/

#include "ENetwork/ENetServer.h"

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief Functor for accept thread.
    @param p_unused Unused.
    @return Unused.
  */
  DWORD WINAPI          AcceptFunctor(LPVOID p_unused)
  {
    ENetServer::getInstance()->accept();
    return (0);
  }
  
  /**
    @brief Singleton: Instantiate a ENetServer.
    @details Initialize its mutex.
  */
  ENetServer::ENetServer() :
    m_socketAccept(),
    m_threadAccept(nullptr),
    m_selectors({}),
    m_mutexSelectors(nullptr),
    m_packetHandler(),
    m_isRunning(false)
  {
    m_mutexSelectors = CreateMutex(nullptr, false, nullptr);
  }
  
  /**
    @brief Delete a ENetServer.
    @details Release its mutex, terminate its thread, close the accept ENetSocket, delete its ENetSelectors and close WSA.
  */
  ENetServer::~ENetServer()
  {
    ReleaseMutex(m_mutexSelectors);
    CloseHandle(m_mutexSelectors);
    m_socketAccept.close();
    TerminateThread(m_threadAccept, 0);
    CloseHandle(m_threadAccept);
    while (m_selectors.empty() != true)
    {
      m_selectors.front()->stop();
      m_selectors.erase(m_selectors.begin());
    }
    WSACleanup();
  }

  /**
    @brief Retrieve Singleton of ENetServer.
    @details Call WSAStartup at first call.
    @return Unique instance of ENetServer.
    @eerror EERROR_NET_WSA_STARTUP if WSAStartup() fail.
  */
  ENetServer            *ENetServer::getInstance()
  {
    static ENetServer   *l_instance = nullptr;

    if (nullptr == l_instance)
    {
      WSADATA           WSAData = { 0 };

      if (0 != WSAStartup(MAKEWORD(2, 2), &WSAData))
      {
        mEERROR_S(EERROR_NET_WSA_STARTUP);
      }
      l_instance = new ENetServer();
    }

    return (l_instance);
  }

  /**
    @brief Initialize the ENetServer.
    @details Prepare the ENetSocket accept to receive incoming connection on specific hostname.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SERVER_RUNNING if ENetServer is already running.
    @eerror EERROR_NET_SERVER_INIT if socket(), bind() or listen() failed.
  */
  void                  ENetServer::init(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_NET_SERVER_RUNNING);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_socketAccept.socket(ENETSOCKET_FLAGS_PROTOCOL_TCP);
      if (EERROR_NONE == mEERROR)
      {
        m_socketAccept.bind(p_hostname, p_port);
        if (EERROR_NONE == mEERROR)
        {
          m_socketAccept.listen();
          if (EERROR_NONE == mEERROR)
          {
            mEPRINT_STD("ENetServer successfully connected to " + p_hostname + ":" + std::to_string(p_port));
          }
          else
          {
            mEERROR_S(EERROR_NET_SERVER_INIT);
          }
        }
        else
        {
          mEERROR_S(EERROR_NET_SERVER_INIT);
        }
      }
      else
      {
        mEERROR_S(EERROR_NET_SERVER_INIT);
      }
    }
  }

  /**
    @brief Start the automation of the ENetServer.
    @details Create the thread of the ENetServer and call its accept function.
    @details Start every ENetSelector it contains by calling their run function.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SERVER_START if start() fail.
  */
  void                  ENetServer::start()
  {
    mEERROR_R();
    if (false == isRunning())
    {
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      clearSelectors();
      for (std::vector<ENetSelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        (*l_it)->start();
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SA(EERROR_NET_SERVER_START, mEERROR_G.toString());
        }
      }
      ReleaseMutex(m_mutexSelectors);
      m_threadAccept = CreateThread(nullptr, 0, AcceptFunctor, nullptr, 0, nullptr);
      m_isRunning = true;
      mEPRINT_STD("ENetServer started");
    }
  }

  /**
    @brief Stop the ENetServer automation.
    @details Terminate the accept function thread, and stop every ENetSelectors.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SERVER_STOP if stop() fail.
  */
  void                  ENetServer::stop()
  {
    mEERROR_R();
    if (true == isRunning())
    {
      m_isRunning = false;
      TerminateThread(m_threadAccept, 0);
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      for (std::vector<ENetSelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        (*l_it)->stop();
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SA(EERROR_NET_SERVER_STOP, mEERROR_G.toString());
        }
      }
      ReleaseMutex(m_mutexSelectors);
      mEPRINT_STD("ENetServer stopped");
    }
  }

  /**
    @brief Accept incoming connection to the ENetServer.
    @details Accept incoming connection to the ENetServer and add the ENetSocket created to a ENetSelector handling.
    @details Automatically delete empty and not running ENetSelector at each new connection.
  */
  void                  ENetServer::accept()
  {
    while (true == isRunning())
    {
      ENetSocket        *l_client = nullptr;

      l_client = m_socketAccept.accept();
      if (nullptr != l_client)
      {
        addClient(l_client);
        if (EERROR_NONE != mEERROR)
        {
          mEPRINT_ERR("Failed to connect EClient" + std::to_string(*l_client));
          delete (l_client);
        }
      }
      else
      {
        mEERROR_SA(EERROR_NET_SOCKET_INVALID, mEERROR_G.toString());
      }
    }
  }
  
  /**
    @brief Add ENetSocket client to a ENetSelector for automation.
    @details Add the ENetSocket client to a running ENetSelector with room.
    @details Automatically create a new ENetSelector and run it if needed.
    @param p_client ENetSocket of a newly connected client.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SOCKET_INVALID is p_client is null.
    @eerror EERROR_NET_SERVER_ADD if p_client couldn't be add or a newly created ENetSelector couldn't be start.
  */
  void                  ENetServer::addClient(ENetSocket *p_client)
  {
    bool                l_added = false;

    mEERROR_R();
    if (nullptr == p_client)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      clearSelectors();
      for (std::vector<ENetSelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        if (false == l_added)
        {
          l_added = (*l_it)->addClient(p_client);
          if (EERROR_NONE != mEERROR)
          {
            mEERROR_SA(EERROR_NET_SERVER_ADD, mEERROR_G.toString());
          }
        }
      }
      ReleaseMutex(m_mutexSelectors);
      if ((EERROR_NONE == mEERROR)
        && (false == l_added))
      {
        ENetSelector    *l_selector = nullptr;

        l_selector = new ENetSelector(m_packetHandler, p_client);
        if (nullptr != l_selector)
        {
          l_selector->start();
          if (EERROR_NONE == mEERROR)
          {
            WaitForSingleObject(m_mutexSelectors, INFINITE);
            m_selectors.push_back(l_selector);
            ReleaseMutex(m_mutexSelectors);
            l_added = true;
          }
          else
          {
            delete (l_selector);
            mEERROR_SA(EERROR_NET_SERVER_ADD, mEERROR_G.toString());
          }
        }
        else
        {
          mEERROR_S(EERROR_OOM);
        }
      }
    }
  }

  /**
    @brief Send ENetPacket to every ENetSocket connected to the ENetServer.
    @details Use ENetSelector broadcast command to send the ENetPacket.
    @details EErrors during sends are ignored.
    @param p_packet ENetPacket to be send.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NULL_PTR is p_packet is null.
  */
  void                  ENetServer::broadcast(ENetPacket *p_packet)
  {
    if (nullptr == p_packet)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      for (std::vector<ENetSelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        (*l_it)->broadcast(p_packet);
      }
      ReleaseMutex(m_mutexSelectors);
    }
  }

  /**
    @brief Clear the ENetSelector list unused.
    @details Delete every ENetSelectors that are empty and not running.
  */
  void                  ENetServer::clearSelectors()
  {
    WaitForSingleObject(m_mutexSelectors, INFINITE);
    for (std::vector<ENetSelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); )
    {
      if ((true == (*l_it)->isEmpty())
        && (false == (*l_it)->isRunning()))
      {
        delete (*l_it);
        l_it = m_selectors.erase(l_it);
      }
      else
      {
        ++l_it;
      }
    }
    ReleaseMutex(m_mutexSelectors);
  }

  /**
    @brief Get the ENetPacketHandler of the ENetServer.
    @return Reference to the ENetPacketHandler of the ENetServer.
  */
  ENetPacketHandler     &ENetServer::getPacketHandler()
  {
    return (m_packetHandler);
  }

  /**
    @brief Determine if the ENetServer is running.
    @return bool indicating if the ENetServer is running.
  */
  bool                  ENetServer::isRunning() const
  {
    return (m_isRunning);
  }

  /**
    @brief Retreive the ENetServer informations as string.
    @return String containing informations of ENetServer.
  */
  const std::string     ENetServer::toString() const
  {
    std::string         l_str;

    l_str = "ENetServer ";
    l_str += isRunning() ? "running\n" : "stopped\n";
    WaitForSingleObject(m_mutexSelectors, INFINITE);
    for (std::vector<ENetSelector*>::const_iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
    {
      l_str += "  " + (*l_it)->toString() + "\n";
    }
    ReleaseMutex(m_mutexSelectors);

    return (l_str);
  }

 }
