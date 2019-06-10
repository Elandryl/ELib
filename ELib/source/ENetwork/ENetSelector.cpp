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
    @brief Functor for ENetSelector::select(). /!\ EError.
    @param p_selector ENetSelector caller.
    @return Unused.
  */
  DWORD WINAPI                SelectFunctor(LPVOID p_selector)
  {
    mEERROR_R();
    if (nullptr != p_selector)
    {
      static_cast<ENetSelector*>(p_selector)->select();
    }
    else
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    return (0);
  }

  /**
    @brief Constructor for ENetSelector.
  */
  ENetSelector::ENetSelector() :
    m_clients(),
    m_threadSelect(nullptr),
    m_mutexClients(nullptr),
    m_isRunning(false)
  {
  }

  /**
    @brief Destructor for ENetSelector.
    @details Release its mutex, terminate its thread, delete its ENetSockets.
  */
  ENetSelector::~ENetSelector()
  {
    ReleaseMutex(m_mutexClients);
    CloseHandle(m_mutexClients);
    TerminateThread(m_threadSelect, 0);
    CloseHandle(m_threadSelect);
    while (m_clients.empty() != true)
    {
      delete (m_clients.front());
      m_clients.erase(m_clients.begin());
    }
  }

  /**
    @brief Start ENetSelector automation. /!\ EError.
    @details Initialize its mutex, create thread for select().
    @details Need at least one client.
  */
  void                      ENetSelector::start()
  {
    mEERROR_R();
    if (true == m_isRunning)
    {
      mEERROR_S(EERROR_NET_SELECTOR_STATE);
    }
    if (0 == getSize())
    {
      mEERROR_S(EERROR_NET_SELECTOR_EMPTY);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_mutexClients = CreateMutex(nullptr, false, nullptr);
      if (nullptr != m_mutexClients)
      {
        m_threadSelect = CreateThread(nullptr, 0, SelectFunctor, this, 0, nullptr);
        if (nullptr != m_threadSelect)
        {
          m_isRunning = true;
        }
        else
        {
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
    @brief Stop ENetSelector automation. /!\ EError.
    @details Release its mutex, terminate its thread.
  */
  void                        ENetSelector::stop()
  {
    if (false == m_isRunning)
    {
      mEERROR_S(EERROR_NET_SELECTOR_STATE);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_isRunning = false;
      ReleaseMutex(m_mutexClients);
      TerminateThread(m_threadSelect, 0);
    }
  }

  /**
    @brief Loop for connected ENetSocket automation. /!\ Blocking. /!\ Mutex. /!\ EError.
    @details Receive ENetPacket from ENetSocket clients and store them into ENetPacketHandler.
    @details Close ENetPacket client receive failure.
    @details Stop when clients list is empty.
    @details ENetPacketHandler Singleton need to be valid.
  */
  void                        ENetSelector::select()
  {
    while (true == m_isRunning)
    {
      fd_set                  l_set = { 0 };
      uint64                  l_maxFd = 0;
      timeval                 l_timeout = { 0, 100000 };

      mEERROR_R();
      if (nullptr != ENetPacketHandler::getInstance())
      {
        mEERROR_SH(EERROR_NULL_PTR);
        stop();
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SH(EERROR_NET_SELECTOR_ERR);
        }
      }

      if (EERROR_NONE == mEERROR)
      {
        WaitForSingleObject(m_mutexClients, INFINITE);
        FD_ZERO(&l_set);
        for (std::vector<ENetSocket*>::iterator l_client = m_clients.begin(); l_client != m_clients.end(); ++l_client)
        {
          l_maxFd = max(l_maxFd, **l_client);
          FD_SET(**l_client, &l_set);
        }
        if (SOCKET_ERROR != ::select(static_cast<int>(l_maxFd), &l_set, nullptr, nullptr, &l_timeout))
        {
          for (std::vector<ENetSocket*>::iterator l_client = m_clients.begin(); l_client != m_clients.end(); )
          {
            bool                l_closed = false;

            if (0 != FD_ISSET(**l_client, &l_set))
            {
              ENetPacketHandler::getInstance()->recvPacket(*l_client);
              if (EERROR_NONE != mEERROR)
              {
                ENetPacketType  l_type = ENETPACKET_TYPE_DISCONNECT;

                mEERROR_SH(EERROR_NET_PACKETHANDLER_ERR);
                ENetPacketHandler::getInstance()->read(reinterpret_cast<char*>(&l_type), sizeof(ENetPacketType), *l_client);
                if (EERROR_NONE == mEERROR)
                {
                  (*l_client)->close();
                  if (EERROR_NONE == mEERROR)
                  {
                    // delete (*l_client); -> TODO: high risk of segfault when reading packets.
                    l_client = m_clients.erase(l_client);
                    l_closed = true;
                    if (0 == getSize())
                    {
                      stop();
                      if (EERROR_NONE != mEERROR)
                      {
                        mEERROR_SH(EERROR_NET_SELECTOR_ERR);
                      }
                    }
                  }
                  else
                  {
                    mEERROR_SH(EERROR_NET_SOCKET_ERR);
                  }
                }
                else
                {
                  mEERROR_SH(EERROR_NET_PACKETHANDLER_ERR);
                }
              }
            }
            if (false == l_closed)
            {
              l_client++;
            }
          }
        }
        else
        {
          mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(WSAGetLastError()));
        }
        ReleaseMutex(m_mutexClients);
      }
    }
  }

  /**
    @brief Add a connected ENetSocket client to automation. /!\ Mutex. /!\ EError.
    @details Can contains up to ENETSOCKET_MAX_CLIENTS clients.
    @details An EError indicate that ENetSocket client should be discarded.
    @details ENetPacketHandler Singleton need to be valid.
    @param p_client ENetPacket client.
    @return true on success.
    @return false on failure.
  */
  bool                        ENetSelector::addClient(ENetSocket *p_client)
  {
    bool                      l_ret = false;

    mEERROR_R();
    if (nullptr == p_client)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if ((nullptr != p_client)
      && (ENETSOCKET_FLAGS_STATE_CONNECTED != (p_client->getFlags() & ENETSOCKET_FLAGS_STATES)))
    {
      mEERROR_S(EERROR_NET_SOCKET_STATE);
    }
    if ((nullptr != p_client)
      && (ENETSOCKET_FLAGS_PROTOCOL_TCP != (p_client->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS)))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    if (nullptr != ENetPacketHandler::getInstance())
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (ENETSOCKET_MAX_CLIENTS > m_clients.size())
      {
        ENetPacketType        l_type = ENETPACKET_TYPE_CONNECT;

        WaitForSingleObject(m_mutexClients, INFINITE);
        ENetPacketHandler::getInstance()->read(reinterpret_cast<char*>(&l_type), sizeof(ENetPacketType), p_client);
        if (EERROR_NONE == mEERROR)
        {
          m_clients.push_back(p_client);
          l_ret = true;
        }
        else
        {
          /*
            mEERROR_SA(EERROR_NET_PACKETHANDLER_ERR);
            EError is ignored to indicate that ENetSocket client is still valid.
          */
        }
        ReleaseMutex(m_mutexClients);
      }
    }

    return (l_ret);
  }

  /**
    @brief Send ENetPacket to every ENetSocket clients. /!\ Mutex. /!\ EError.
    @param p_packet ENetPacket to be send.
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
      for (std::vector<ENetSocket*>::iterator l_it = m_clients.begin(); l_it != m_clients.end(); ++l_it)
      {
        p_packet->send(*l_it);
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_S(EERROR_NET_PACKET_ERR);
        }
      }
      ReleaseMutex(m_mutexClients);
    }
  }

  /**
    @brief Get number of clients.
    @return Number of clients.
  */
  uint32                      ENetSelector::getSize() const
  {
    return (static_cast<uint32>(m_clients.size()));
  }

  /**
    @brief Get state of ENetSelector.
    @return State.
  */
  bool                        ENetSelector::isRunning() const
  {
    return (m_isRunning);
  }

  /**
    @brief Get ENetSelector informations as string. /!\ Mutex.
    @return Informations of ENetSelector.
  */
  const std::string           ENetSelector::toString() const
  {
    std::string               l_str;

    l_str = "ENetSelector ";
    l_str += isRunning() ? "running " : "stopped ";
    l_str += "(" + std::to_string(getSize()) + " clients).\n";
    WaitForSingleObject(m_mutexClients, INFINITE);
    for (std::vector<ENetSocket*>::const_iterator l_it = m_clients.begin(); l_it != m_clients.end(); ++l_it)
    {
      l_str += "  " + (*l_it)->toString() + ".\n";
    }
    ReleaseMutex(m_mutexClients);

    return (l_str);
  }

 }
