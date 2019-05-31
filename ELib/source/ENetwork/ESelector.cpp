/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ESelector Class.
*/

#include "ENetwork/ESelector.h"

/**
  @brief General scope for ELib components.
*/
namespace                   ELib
{

  /**
    @brief Functor for select thread.
    @param p_param Pointer to param from CreateThread caller.
    @return Unused.
  */
  DWORD WINAPI              SelectFunctor(LPVOID p_param)
  {
    static_cast<ESelector*>(p_param)->select();

    return (0);
  }

  /**
    @brief Instantiate a ESelector and store its client.
    @details Initialize its mutex and store the EPacketHandler that will be use for storage.
    @details ESelector need a least a ESocket client to run. If p_client is null, ESelector will automatically close itself.
    @param p_packetHandler EPacketHandler that will hold the received EPackets.
    @param p_client Pointer to ESocket client that need the creation.
  */
  ESelector::ESelector(EPacketHandler &p_packetHandler, ESocket *p_client) :
    m_packetHandler(p_packetHandler),
    m_socketClients({}),
    m_threadSelect(nullptr),
    m_mutexClients(nullptr),
    m_isRunning(false)
  {
    EPacketType             l_type = EPACKET_TYPE_CONNECT;

    m_mutexClients = CreateMutex(nullptr, false, nullptr);
    m_socketClients.push_back(p_client);
    m_packetHandler.generate(p_client, reinterpret_cast<char*>(&l_type), sizeof(EPacketType));

  }

  /**
    @brief Delete a ESelector.
    @details Release its mutex, terminate its thread.
  */
  ESelector::~ESelector()
  {
    ReleaseMutex(m_mutexClients);
    CloseHandle(m_mutexClients);
    TerminateThread(m_threadSelect, 0);
    CloseHandle(m_threadSelect);
  }

  /**
    @brief Start the ESelector automation.
    @details Create the thread of ESelector and call its select function.
    @details ESelector need to have at least one ESocket to handle.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SELECTOR_RUNNING if the ESelector is already running.
    @eerror EERROR_SELECTOR_EMPTY it the ESelector is empty.
  */
  void                      ESelector::start()
  {
    mEERROR_R();
    if (true == m_isRunning)
    {
      mEERROR_S(EERROR_SELECTOR_RUNNING);
    }
    if (true == isEmpty())
    {
      mEERROR_S(EERROR_SELECTOR_EMPTY);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      m_threadSelect = CreateThread(nullptr, 0, SelectFunctor, this, 0, nullptr);
      m_isRunning = true;
    }
  }

  /**
    @brief Stop the ESelector automation.
    @details Terminate the thread of the ESelector, and release its mutex.
  */
  void                      ESelector::stop()
  {
    if (true == m_isRunning)
    {
      m_isRunning = false;
      ReleaseMutex(m_mutexClients);
      TerminateThread(m_threadSelect, 0);
    }
  }

  /**
    @brief Handle the reception of EPacket for the ESocket clients contained in the ESelector.
    @details Automatically receive the EPacket of ESockets in the list of clients handle by this ESelector.
    @details Store the received EPackets in the EPacketHandler linked at the creation of ESelector.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SELECTOR_SELECT if select() fail.
    @eerror EERROR_SELECTOR_RECV if recvPacket() fail.
  */
  void                      ESelector::select()
  {
    while (true == m_isRunning)
    {
      fd_set                l_set = { 0 };
      uint64                l_maxFd = 0;
      timeval               l_timeout = { 0, 100000 };
      int32                 l_ret = SOCKET_ERROR;

      mEERROR_R();
      WaitForSingleObject(m_mutexClients, INFINITE);
      FD_ZERO(&l_set);
      for (std::vector<ESocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
      {
        l_maxFd = max(l_maxFd, **l_it);
        FD_SET(**l_it, &l_set);
      }
      l_ret = ::select(static_cast<int>(l_maxFd), &l_set, nullptr, nullptr, &l_timeout);
      if (SOCKET_ERROR != l_ret)
      {
        for (std::vector<ESocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
        {
          if (0 != FD_ISSET(**l_it, &l_set))
          {
            m_packetHandler.recvPacket(*l_it);
            if (EERROR_NONE != mEERROR_G.m_errorCode)
            {
              EPacketType   l_type = EPACKET_TYPE_DISCONNECT;

              mEERROR_SA(EERROR_SELECTOR_RECV, mEERROR_G.toString());
              m_packetHandler.generate(*l_it, reinterpret_cast<char*>(&l_type), sizeof(EPacketType));
              (*l_it)->close();
            }
          }
        }
        for (std::vector<ESocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); )
        {
          if (ESOCKET_FLAGS_STATE_UNINITIALIZED == (*l_it)->getFlags())
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
        mEERROR_SA(EERROR_SELECTOR_SELECT, getWSAErrString());
      }
      ReleaseMutex(m_mutexClients);
    }
  }

  /**
    @brief Add a ESocket client to the set of this ESelector.
    @details Add the client only if there is enough space for it.
    @details The client is only added when the Semaphore is released. Might cause a delay.
    @details If an error occurs, ESocket client is invalid and can't be added to any ESelector. It should be discarded by user.
    @param p_client Pointer to ESocket to be add to ESelector handling.
    @return true in success.
    @return false if p_client is null, ESelector is already full or not running, or an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SOCKET_INVALID if p_client is null.
  */
  bool                      ESelector::addClient(ESocket *p_client)
  {
    bool                    l_ret = false;

    mEERROR_R();
    if (nullptr == p_client)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }

    if ((EERROR_NONE == mEERROR_G.m_errorCode)
      && (MAX_CLIENTS > m_socketClients.size()))
    {
      WaitForSingleObject(m_mutexClients, INFINITE);
      if (true == m_isRunning)
      {
        EPacketType         l_type = EPACKET_TYPE_CONNECT;

        m_packetHandler.generate(p_client, reinterpret_cast<char*>(&l_type), sizeof(EPacketType));
        m_socketClients.push_back(p_client);
        l_ret = true;
      }
      ReleaseMutex(m_mutexClients);
    }

    return (l_ret);
  }

  /**
    @brief Send EPacket to every ESocket connected to the ESelector.
    @details Send EPacket to every ESocket connected to the ESelector.
    @details EErrors during sends are ignored.
    @param p_packet EPacket to be send.
    @eerror EERROR_NONE in success.
    @eerror EERROR_PACKET_INVALID is p_packet is null.
  */
  void                      ESelector::broadcast(EPacket *p_packet)
  {
    if (nullptr == p_packet)
    {
      mEERROR_S(EERROR_PACKET_INVALID);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      WaitForSingleObject(m_mutexClients, INFINITE);
      for (std::vector<ESocket*>::iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
      {
        p_packet->send(*l_it);
      }
      ReleaseMutex(m_mutexClients);
    }
  }

  /**
    @brief Determine if the ESelector is empty.
    @return bool indicating if the ESelector is empty.
  */
  bool                      ESelector::isEmpty() const
  {
    return (m_socketClients.empty());
  }

  /**
    @brief Determine if the ESelector is running.
    @return bool indicating if the ESelector is running.
  */
  bool                      ESelector::isRunning() const
  {
    return (m_isRunning);
  }

  /**
    @brief Retreive the ESelector informations as string.
    @return String containing informations of ESelector.
  */
  const std::string         ESelector::toString() const
  {
    std::string             l_str;

    l_str = "ESelector ";
    l_str += isRunning() ? "running\n" : "stopped\n";
    WaitForSingleObject(m_mutexClients, INFINITE);
    for (std::vector<ESocket*>::const_iterator l_it = m_socketClients.begin(); l_it != m_socketClients.end(); ++l_it)
    {
      l_str += "  " + (*l_it)->toString() + "\n";
    }
    ReleaseMutex(m_mutexClients);

    return (l_str);
  }

 }
