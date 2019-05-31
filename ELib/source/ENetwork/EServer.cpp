/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EServer Class.
*/

#include "ENetwork/EServer.h"

/**
  @brief General scope for ELib components.
*/
namespace                         ELib
{

  /**
    @brief Functor for accept thread.
    @param p_param Pointer to param from CreateThread caller.
    @return Unused.
  */
  DWORD WINAPI                    AcceptFunctor(LPVOID p_param)
  {
    static_cast<EServer*>(p_param)->accept();
    return (0);
  }
  
  /**
    @brief Instantiate a EServer.
    @details Initialize WSA and its mutex.
    @details It should be unique in an application.
    @ethrow EERROR_WSA_STARTUP if WSAStartup() fail.
  */
  EServer::EServer() :
    m_socketAccept(),
    m_selectors({}),
    m_threadAccept(nullptr),
    m_mutexSelectors(nullptr),
    m_packetHandler(),
    m_printer(),
    m_isRunning(false)
  {
    WSADATA                       WSAData = { 0 };
    
    if (0 != WSAStartup(MAKEWORD(2, 2), &WSAData))
    {
      mETHROW_S(EERROR_WSA_STARTUP);
    }
    m_mutexSelectors = CreateMutex(nullptr, false, nullptr);
  }
  
  /**
    @brief Delete a EServer.
    @details Release its mutex, terminate its thread, close the accept ESocket, delete its ESelectors and close WSA.
  */
  EServer::~EServer()
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
    @brief Initialize the EServer.
    @details Prepare the ESocket accept to receive incoming connection on specific hostname.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SERVER_RUNNING if EServer is already running.
    @eerror EERROR_SERVER_INIT if socket(), bind() or listen() failed.
  */
  void                      EServer::init(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_SERVER_RUNNING);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      m_socketAccept.socket(ESOCKET_FLAGS_PROTOCOL_TCP);
      if (EERROR_NONE == mEERROR_G.m_errorCode)
      {
        m_socketAccept.bind(p_hostname, p_port);
        if (EERROR_NONE == mEERROR_G.m_errorCode)
        {
          m_socketAccept.listen();
          if (EERROR_NONE == mEERROR_G.m_errorCode)
          {
            m_printer.print(EPRINT_TYPE_PRIORITY_STD, "EServer successfully connected to " + p_hostname + ":" + std::to_string(p_port));
          }
          else
          {
            mEERROR_S(EERROR_SERVER_INIT);
          }
        }
        else
        {
          mEERROR_S(EERROR_SERVER_INIT);
        }
      }
      else
      {
        mEERROR_S(EERROR_SERVER_INIT);
      }
    }
  }

  /**
    @brief Start the automation of the EServer.
    @details Create the thread of the EServer and call its accept function.
    @details Start every ESelector it contains by calling their run function.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SERVER_START if start() fail.
  */
  void                      EServer::start()
  {
    mEERROR_R();
    if (false == isRunning())
    {
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      clearSelectors();
      for (std::vector<ESelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        (*l_it)->start();
        if (EERROR_NONE != mEERROR_G.m_errorCode)
        {
          mEERROR_SA(EERROR_SERVER_START, mEERROR_G.toString());
        }
      }
      ReleaseMutex(m_mutexSelectors);
      m_threadAccept = CreateThread(nullptr, 0, AcceptFunctor, this, 0, nullptr);
      m_isRunning = true;
      m_printer.print(EPRINT_TYPE_PRIORITY_STD, "EServer started");
    }
  }

  /**
    @brief Stop the EServer automation.
    @details Terminate the accept function thread, and stop every ESelectors.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SERVER_STOP if stop() fail.
  */
  void                      EServer::stop()
  {
    mEERROR_R();
    if (true == isRunning())
    {
      m_isRunning = false;
      TerminateThread(m_threadAccept, 0);
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      for (std::vector<ESelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        (*l_it)->stop();
        if (EERROR_NONE != mEERROR_G.m_errorCode)
        {
          mEERROR_SA(EERROR_SERVER_STOP, mEERROR_G.toString());
        }
      }
      ReleaseMutex(m_mutexSelectors);
      m_printer.print(EPRINT_TYPE_PRIORITY_STD, "EServer stopped");
    }
  }

  /**
    @brief Accept incoming connection to the EServer.
    @details Accept incoming connection to the EServer and add the ESocket created to a ESelector handling.
    @details Automatically delete empty and not running ESelector at each new connection.
  */
  void                      EServer::accept()
  {
    while (true == isRunning())
    {
      ESocket               *l_client = nullptr;

      l_client = m_socketAccept.accept();
      if (nullptr != l_client)
      {
        addClient(l_client);
        if (EERROR_NONE != mEERROR_G.m_errorCode)
        {
          m_printer.print(EPRINT_TYPE_PRIORITY_ERROR, "Failed to connect EClient" + std::to_string(*l_client));
          delete (l_client);
        }
      }
    }
  }
  
  /**
    @brief Add ESocket client to a ESelector for automation.
    @details Add the ESocket client to a running ESelector with room.
    @details Automatically create a new ESelector and run it if needed.
    @param p_client ESocket of a newly connected client.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SOCKET_INVALID is p_client is null.
    @eerror EERROR_SERVER_ADD if p_client couldn't be add or a newly created ESelector couldn't be start.
  */
  void                      EServer::addClient(ESocket *p_client)
  {
    bool                    l_added = false;

    mEERROR_R();
    if (nullptr == p_client)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      clearSelectors();
      for (std::vector<ESelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        if (false == l_added)
        {
          l_added = (*l_it)->addClient(p_client);
          if (EERROR_NONE != mEERROR_G.m_errorCode)
          {
            mEERROR_SA(EERROR_SERVER_ADD, mEERROR_G.toString());
          }
        }
      }
      ReleaseMutex(m_mutexSelectors);
      if ((EERROR_NONE == mEERROR_G.m_errorCode)
        && (false == l_added))
      {
        ESelector           *l_selector = nullptr;

        l_selector = new ESelector(m_packetHandler, p_client);
        if (nullptr != l_selector)
        {
          l_selector->start();
          if (EERROR_NONE == mEERROR_G.m_errorCode)
          {
            WaitForSingleObject(m_mutexSelectors, INFINITE);
            m_selectors.push_back(l_selector);
            ReleaseMutex(m_mutexSelectors);
            l_added = true;
          }
          else
          {
            delete (l_selector);
            mEERROR_SA(EERROR_SERVER_ADD, mEERROR_G.toString());
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
    @brief Send EPacket to every ESocket connected to the EServer.
    @details Use ESelector broadcast command to send the EPacket.
    @details EErrors during sends are ignored.
    @param p_packet EPacket to be send.
    @eerror EERROR_NONE in success.
    @eerror EERROR_PACKET_INVALID is p_packet is null.
  */
  void                      EServer::broadcast(EPacket *p_packet)
  {
    if (nullptr == p_packet)
    {
      mEERROR_S(EERROR_PACKET_INVALID);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      for (std::vector<ESelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        (*l_it)->broadcast(p_packet);
      }
      ReleaseMutex(m_mutexSelectors);
    }
  }

  /**
    @brief Clear the ESelector list unused.
    @details Delete every ESelectors that are empty and not running.
  */
  void                      EServer::clearSelectors()
  {
    WaitForSingleObject(m_mutexSelectors, INFINITE);
    for (std::vector<ESelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); )
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
    @brief Get the EPacketHandler of the EServer.
    @return Reference to the EPacketHandler of the EServer.
  */
  EPacketHandler                  &EServer::getPacketHandler()
  {
    return (m_packetHandler);
  }

  /**
    @brief Get the EPrinter of the EServer.
    @return EPrinter of the EServer.
  */
  EPrinter                        &EServer::getPrinter()
  {
    return (m_printer);
  }

  /**
    @brief Determine if the EServer is running.
    @return bool indicating if the EServer is running.
  */
  bool                            EServer::isRunning() const
  {
    return (m_isRunning);
  }

  /**
    @brief Retreive the EServer informations as string.
    @return String containing informations of EServer.
  */
  const std::string               EServer::toString() const
  {
    std::string                   l_str;

    l_str = "EServer ";
    l_str += isRunning() ? "running\n" : "stopped\n";
    WaitForSingleObject(m_mutexSelectors, INFINITE);
    for (std::vector<ESelector*>::const_iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
    {
      l_str += "  " + (*l_it)->toString() + "\n";
    }
    ReleaseMutex(m_mutexSelectors);

    return (l_str);
  }

 }
