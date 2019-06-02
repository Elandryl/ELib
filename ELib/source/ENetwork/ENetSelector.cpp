/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetSelector Class.
*/

#include "ENetwork/ENetSelector.h"

/**
  @brief General scope for ELib components.
*/
namespace                     ELib
{

  /**
    @brief Functor for select thread.
    @param p_param Pointer to param from CreateThread caller.
    @return Unused.
  */
  DWORD WINAPI                SelectFunctor(LPVOID p_param)
  {
    static_cast<ENetSelector*>(p_param)->select();

    return (0);
  }

  /**
    @brief Instantiate a ENetSelector and store its client.
    @details Initialize its mutex and store the ENetPacketHandler that will be use for storage.
    @details ENetSelector need a least a ENetSocket client to run. If p_client is null, ENetSelector will automatically close itself.
    @param p_packetHandler ENetPacketHandler that will hold the received ENetPackets.
    @param p_client Pointer to ENetSocket client that need the creation.
  */
  ENetSelector::ENetSelector(ENetPacketHandler &p_packetHandler, ENetSocket *p_client) :
    m_packetHandler(p_packetHandler),
    m_socketClients({}),
    m_threadSelect(nullptr),
    m_mutexClients(nullptr),
    m_isRunning(false)
  {
    ENetPacketType            l_type = ENETPACKET_TYPE_CONNECT;

    m_mutexClients = CreateMutex(nullptr, false, nullptr);
    m_socketClients.push_back(p_client);
    m_packetHandler.generate(p_client, reinterpret_cast<char*>(&l_type), sizeof(ENetPacketType));

  }

  /**
    @brief Delete a ENetSelector.
    @details Release its mutex, terminate its thread.
  */
  ENetSelector::~ENetSelector()
  {
    ReleaseMutex(m_mutexClients);
    CloseHandle(m_mutexClients);
    TerminateThread(m_threadSelect, 0);
    CloseHandle(m_threadSelect);
  }

  /**
    @brief Start the ENetSelector automation.
    @details Create the thread of ENetSelector and call its select function.
    @details ENetSelector need to have at least one ENetSocket to handle.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SELECTOR_RUNNING if the ENetSelector is already running.
    @eerror EERROR_NET_SELECTOR_EMPTY it the ENetSelector is empty.
  */
  void                      ENetSelector::start()
  {
    mEERROR_R();
    if (true == m_isRunning)
    {
      mEERROR_S(EERROR_NET_SELECTOR_RUNNING);
    }
    if (true == isEmpty())
    {
      mEERROR_S(EERROR_NET_SELECTOR_EMPTY);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_threadSelect = CreateThread(nullptr, 0, SelectFunctor, this, 0, nullptr);
      m_isRunning = true;
    }
  }

  /**
    @brief Stop the ENetSelector automation.
    @details Terminate the thread of the ENetSelector, and release its mutex.
  */
  void                        ENetSelector::stop()
  {
    if (true == m_isRunning)
    {
      m_isRunning = false;
      ReleaseMutex(m_mutexClients);
      TerminateThread(m_threadSelect, 0);
    }
  }

  /**
    @brief Handle the reception of ENetPacket for the ENetSocket clients contained in the ENetSelector.
    @details Automatically receive the ENetPacket of ENetSockets in the list of clients handle by this ENetSelector.
    @details Store the received ENetPackets in the ENetPacketHandler linked at the creation of ENetSelector.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SELECTOR_SELECT if select() fail.
    @eerror EERROR_NET_SELECTOR_RECV if recvPacket() fail.
  */
  void                        ENetSelector::select()
  {
    while (true == m_isRunning)
    {
      fd_set                  l_set = { 0 };
      uint64                  l_maxFd = 0;
      timeval                 l_timeout = { 0, 100000 };
      int32                   l_ret = SOCKET_ERROR;

      mEERROR_R();
      WaitForSingleObject(m_mutexClients, INFINITE);
      FD_ZERO(&l_set);
      for (std::vector<ENetSocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
      {
        l_maxFd = max(l_maxFd, **l_it);
        FD_SET(**l_it, &l_set);
      }
      l_ret = ::select(static_cast<int>(l_maxFd), &l_set, nullptr, nullptr, &l_timeout);
      if (SOCKET_ERROR != l_ret)
      {
        for (std::vector<ENetSocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
        {
          if (0 != FD_ISSET(**l_it, &l_set))
          {
            m_packetHandler.recvPacket(*l_it);
            if (EERROR_NONE != mEERROR)
            {
              ENetPacketType  l_type = ENETPACKET_TYPE_DISCONNECT;

              mEERROR_SA(EERROR_NET_SELECTOR_RECV, mEERROR_G.toString());
              m_packetHandler.generate(*l_it, reinterpret_cast<char*>(&l_type), sizeof(ENetPacketType));
              (*l_it)->close();
            }
          }
        }
        for (std::vector<ENetSocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); )
        {
          if (ENETSOCKET_FLAGS_STATE_UNINITIALIZED == (*l_it)->getFlags())
          {
            l_it = m_socketClients.erase(l_it);
          }
          else
          {
            l_it++;
          }
        }
        if (true == isEmpty())
        {
          stop();
        }
      }
      else
      {
        mEERROR_SA(EERROR_NET_SELECTOR_SELECT, getWSAErrString());
      }
      ReleaseMutex(m_mutexClients);
    }
  }

  /**
    @brief Add a ENetSocket client to the set of this ENetSelector.
    @details Add the client only if there is enough space for it.
    @details The client is only added when the Semaphore is released. Might cause a delay.
    @details If an error occurs, ENetSocket client is invalid and can't be added to any ENetSelector. It should be discarded by user.
    @param p_client Pointer to ENetSocket to be add to ENetSelector handling.
    @return true in success.
    @return false if p_client is null, ENetSelector is already full or not running, or an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SOCKET_INVALID if p_client is null.
  */
  bool                        ENetSelector::addClient(ENetSocket *p_client)
  {
    bool                      l_ret = false;

    mEERROR_R();
    if (nullptr == p_client)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }

    if ((EERROR_NONE == mEERROR)
      && (ENETSOCKET_MAX_CLIENTS > m_socketClients.size()))
    {
      WaitForSingleObject(m_mutexClients, INFINITE);
      if (true == m_isRunning)
      {
        ENetPacketType        l_type = ENETPACKET_TYPE_CONNECT;

        m_packetHandler.generate(p_client, reinterpret_cast<char*>(&l_type), sizeof(ENetPacketType));
        m_socketClients.push_back(p_client);
        l_ret = true;
      }
      ReleaseMutex(m_mutexClients);
    }

    return (l_ret);
  }

  /**
    @brief Send ENetPacket to every ENetSocket connected to the ENetSelector.
    @details Send ENetPacket to every ENetSocket connected to the ENetSelector.
    @details EErrors during sends are ignored.
    @param p_packet ENetPacket to be send.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NULL_PTR if p_packet is null.
  */
  void                        ENetSelector::broadcast(ENetPacket *p_packet)
  {
    if (nullptr == p_packet)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      WaitForSingleObject(m_mutexClients, INFINITE);
      for (std::vector<ENetSocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
      {
        p_packet->send(*l_it);
      }
      ReleaseMutex(m_mutexClients);
    }
  }

  /**
    @brief Determine if the ENetSelector is empty.
    @return bool indicating if the ENetSelector is empty.
  */
  bool                        ENetSelector::isEmpty() const
  {
    return (m_socketClients.empty());
  }

  /**
    @brief Determine if the ENetSelector is running.
    @return bool indicating if the ENetSelector is running.
  */
  bool                        ENetSelector::isRunning() const
  {
    return (m_isRunning);
  }

  /**
    @brief Retreive the ENetSelector informations as string.
    @return String containing informations of ENetSelector.
  */
  const std::string           ENetSelector::toString() const
  {
    std::string               l_str;

    l_str = "ENetSelector ";
    l_str += isRunning() ? "running\n" : "stopped\n";
    WaitForSingleObject(m_mutexClients, INFINITE);
    for (std::vector<ENetSocket*>::const_iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
    {
      l_str += "  " + (*l_it)->toString() + "\n";
    }
    ReleaseMutex(m_mutexClients);

    return (l_str);
  }

 }
