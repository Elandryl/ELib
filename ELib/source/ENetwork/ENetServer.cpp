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
    @brief Functor for ENetServer::recvfrom(). /!\ EError.
    @param p_unused Unused.
    @return Unused.
  */
  DWORD WINAPI          ServerRecvfromFunctor(LPVOID p_unused)
  {
    mEERROR_R();
    if (nullptr != ENetServer::getInstance())
    {
      ENetServer::getInstance()->recvfrom();
    }
    else
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    return (0);
  }

  /**
    @brief Functor for ENetServer::accept(). /!\ EError.
    @param p_unused Unused.
    @return Unused.
  */
  DWORD WINAPI          ServerAcceptFunctor(LPVOID p_unused)
  {
    mEERROR_R();
    if (nullptr != ENetServer::getInstance())
    {
      ENetServer::getInstance()->accept();
    }
    else
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    return (0);
  }
  
  /**
    @brief Constructor for ENetServer.
  */
  ENetServer::ENetServer() :
    m_socketRecvfrom(),
    m_threadRecvfrom(nullptr),
    m_socketAccept(),
    m_threadAccept(nullptr),
    m_selectors({}),
    m_mutexSelectors(nullptr),
    m_isRunning(false)
  {
  }
  
  /**
    @brief Destructor for ENetServer.
    @details Release its mutex, terminate its threads, close its ENetSockets, delete its ENetSelectors and call WSACleanup().
  */
  ENetServer::~ENetServer()
  {
    ReleaseMutex(m_mutexSelectors);
    CloseHandle(m_mutexSelectors);
    m_socketRecvfrom.close();
    TerminateThread(m_threadRecvfrom, 0);
    CloseHandle(m_threadRecvfrom);
    m_socketAccept.close();
    TerminateThread(m_threadAccept, 0);
    CloseHandle(m_threadAccept);
    while (m_selectors.empty() != true)
    {
      delete (m_selectors.front());
      m_selectors.erase(m_selectors.begin());
    }
    WSACleanup();
  }

  /**
    @brief Singleton for ENetServer. /!\ EError.
    @details Call WSAStartup() and initialize its mutex.
    @return ENetServer unique instance on success.
    @return nullptr on failure.
  */
  ENetServer            *ENetServer::getInstance()
  {
    static ENetServer   *l_instance = nullptr;

    mEERROR_R();
    if (nullptr == l_instance)
    {
      WSADATA           WSAData = { 0 };

      if (0 == WSAStartup(MAKEWORD(2, 2), &WSAData))
      {
        HANDLE          l_mutex = nullptr;

        l_mutex = CreateMutex(nullptr, false, nullptr);
        if (nullptr != l_mutex)
        {
          l_instance = new ENetServer();
          if (nullptr == l_instance)
          {
            l_instance->m_mutexSelectors = l_mutex;
          }
          else
          {
            CloseHandle(l_mutex);
            mEERROR_S(EERROR_MEMORY);
          }
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(GetLastError()));
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
    @brief Initialize ENetServer. /!\ EError.
    @details Prepare UDP ENetSocket for ENetServer::recvfrom().
    @details Prepare TCP ENetSocket for ENetServer::accept().
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Port of the host.
  */
  void                  ENetServer::init(const std::string &p_hostname, uint16 p_port)
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_NET_SERVER_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_socketRecvfrom.socket(ENETSOCKET_FLAGS_PROTOCOL_UDP);
      if (EERROR_NONE == mEERROR)
      {
        m_socketRecvfrom.bind(p_hostname, p_port);
        if (EERROR_NONE == mEERROR)
        {
          mEPRINT_STD("ENetServer: UDP server ready on " + p_hostname + ":" + std::to_string(p_port) + ".");
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
      m_socketAccept.socket(ENETSOCKET_FLAGS_PROTOCOL_TCP);
      if (EERROR_NONE == mEERROR)
      {
        m_socketAccept.bind(p_hostname, p_port);
        if (EERROR_NONE == mEERROR)
        {
          m_socketAccept.listen();
          if (EERROR_NONE == mEERROR)
          {
            mEPRINT_STD("ENetServer: TCP server ready on " + p_hostname + ":" + std::to_string(p_port) + ".");
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
      else
      {
        mEERROR_SH(EERROR_NET_SOCKET_ERR);
      }
    }
  }

  /**
    @brief Start ENetServer automation. /!\ Mutex. /!\ EError.
    @details Create threads for ENetServer::recvfrom() and ENetServer:accept().
    @details Call ENetSelector::start() on each ENetSelector (failures ignored).
    @details ENetPacketHandler Singleton need to be valid.
  */
  void                  ENetServer::start()
  {
    mEERROR_R();
    if (true == isRunning())
    {
      mEERROR_S(EERROR_NET_SERVER_STATE);
    }
    if (nullptr == ENetPacketHandler::getInstance())
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }


    if (EERROR_NONE == mEERROR)
    {
      m_threadRecvfrom = CreateThread(nullptr, 0, ServerRecvfromFunctor, nullptr, 0, nullptr);
      if (nullptr != m_threadRecvfrom)
      {
        m_threadAccept = CreateThread(nullptr, 0, ServerAcceptFunctor, nullptr, 0, nullptr);
        if (nullptr != m_threadAccept)
        {
          WaitForSingleObject(m_mutexSelectors, INFINITE);
          clearSelectors();
          for (std::vector<ENetSelector*>::iterator l_selector = m_selectors.begin(); l_selector != m_selectors.end(); ++l_selector)
          {
            (*l_selector)->start();
            if (EERROR_NONE != mEERROR)
            {
              mEERROR_SH(EERROR_NET_SELECTOR_ERR);
            }
          }
          ReleaseMutex(m_mutexSelectors);
          m_isRunning = true;
          mEPRINT_STD("ENetServer: Started successfully.");
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
    @brief Stop ENetServer automation. /!\ Mutex. /!\ EError.
    @details Terminate its threads.
    @details Call ENetSelector::stop() on each ENetSelectors (failures ignored).
  */
  void                  ENetServer::stop()
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
      TerminateThread(m_threadAccept, 0);
      WaitForSingleObject(m_mutexSelectors, INFINITE);
      for (std::vector<ENetSelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
      {
        (*l_it)->stop();
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SH(EERROR_NET_SELECTOR_ERR);
        }
      }
      ReleaseMutex(m_mutexSelectors);
      mEPRINT_STD("ENetServer: Stopped successfully.");
    }
  }

  /**
    @brief Receive connectionless datas to ENetServer. /!\ Blocking. /!\ Mutex. /!\ EError.
    @details Receive datas on connectionless ENetSocket and send them to ENetPacketHandler::read().
    @details ENetPacketHandler Singleton need to be valid.
  */
  void                  ENetServer::recvfrom()
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
          mEERROR_SH(EERROR_NET_SERVER_ERR);
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
    @brief Accept incoming connections to ENetServer. /!\ Blocking. /!\ Mutex. /!\ EError.
    @details Accept connect ENetServer and send them to ENetServer::addClient().
  */
  void                  ENetServer::accept()
  {
    while (true == isRunning())
    {
      mEERROR_R();
      ENetSocket      *l_client = nullptr;

      l_client = m_socketAccept.accept();
      if (nullptr != l_client)
      {
        addClient(l_client);
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SH(EERROR_NET_SERVER_ERR);
          mEPRINT_ERR("ENetServer: Failed to connect EClient " + std::to_string(*l_client) + ".");
          delete (l_client);
        }
      }
      else
      {
        mEERROR_SH(EERROR_NET_SOCKET_ERR);
      }
    }
  }
  
  /**
    @brief Add ENetSocket client to ENetSelector automation. /!\ Mutex. /!\ EError.
    @details Call ENetServer::clearSelectors() at start.
    @details Call ENetSelector::addClient() on each ENetSelector untill a success.
    @details Create a new ENetSelector if addition failed with no error.
    @details Discard ENetSocket client in case of EError.
    @param p_client ENetSocket client.
  */
  void                  ENetServer::addClient(ENetSocket *p_client)
  {
    mEERROR_R();
    if (nullptr == p_client)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      bool              l_stop = false;

      WaitForSingleObject(m_mutexSelectors, INFINITE);
      clearSelectors();
      for (std::vector<ENetSelector*>::iterator l_selector = m_selectors.begin(); l_selector != m_selectors.end(); ++l_selector)
      {
        if (false == l_stop)
        {
          l_stop = (*l_selector)->addClient(p_client);
          if (EERROR_NONE != mEERROR)
          {
            mEERROR_SH(EERROR_NET_SELECTOR_ERR);
            l_stop = true;
          }
        }
      }
      ReleaseMutex(m_mutexSelectors);
      if ((EERROR_NONE == mEERROR)
        && (false == l_stop))
      {
        ENetSelector    *l_selector = nullptr;

        l_selector = new ENetSelector();
        if (nullptr != l_selector)
        {
          l_selector->addClient(p_client);
          if (EERROR_NONE == mEERROR)
          {
            l_selector->start();
            if (EERROR_NONE == mEERROR)
            {
              WaitForSingleObject(m_mutexSelectors, INFINITE);
              m_selectors.push_back(l_selector);
              ReleaseMutex(m_mutexSelectors);
            }
            else
            {
              mEERROR_SH(EERROR_NET_SELECTOR_ERR);
              delete (l_selector);
            }
          }
          else
          {
            mEERROR_SH(EERROR_NET_SELECTOR_ERR);
            delete (l_selector);
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
    @brief Send ENetPacket to every ENetServer clients. /!\ Mutex. /!\ EError.
    @details Call ENetSelector::broadcast() on each ENetSelector.
    @param p_packet ENetPacket to be send.
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
        if (EERROR_NONE == mEERROR)
        {
          mEERROR_SH(EERROR_NET_SELECTOR_ERR);
        }
      }
      ReleaseMutex(m_mutexSelectors);
    }
  }

  /**
    @brief Clear the ENetSelector list unused. /!\ Mutex.
    @details Delete every ENetSelectors that are empty.
  */
  void                  ENetServer::clearSelectors()
  {
    WaitForSingleObject(m_mutexSelectors, INFINITE);
    for (std::vector<ENetSelector*>::iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); )
    {
      if (0 == (*l_it)->getSize())
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
    @brief Get state of ENetServer.
    @return State.
  */
  bool                  ENetServer::isRunning() const
  {
    return (m_isRunning);
  }

  /**
    @brief Get ENetServer informations as string. /!\ Mutex.
    @return Informations of ENetServer.
  */
  const std::string     ENetServer::toString() const
  {
    std::string         l_str;

    l_str = "ENetServer ";
    l_str += isRunning() ? "running " : "stopped ";
    l_str += "(" + std::to_string(m_selectors.size()) + " selectors).\n";
    WaitForSingleObject(m_mutexSelectors, INFINITE);
    for (std::vector<ENetSelector*>::const_iterator l_it = m_selectors.begin(); l_it != m_selectors.end(); ++l_it)
    {
      l_str += "  " + (*l_it)->toString() + ".\n";
    }
    ReleaseMutex(m_mutexSelectors);

    return (l_str);
  }

 }
