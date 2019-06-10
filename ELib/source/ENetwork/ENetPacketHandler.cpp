/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetPacketHandler Class.
*/

#include "ENetwork/ENetPacketHandler.h"

/**
  @brief General scope for ELib components.
*/
namespace                     ELib
{

  /**
    @brief Generator for ENetPacketDisconnect. /!\ EError.
    @param p_src ENetSocket source of ENetPacket.
    @return Generated ENetPacketDisconnect on success.
    @return nullptr on failure.
  */
  ENetPacket                  *generateENetPacketDisconnect(ENetSocket *p_src)
  {
    ENetPacket                *l_packet = nullptr;

    mEERROR_R();
    l_packet = new ENetPacketDisconnect(p_src);
    if (nullptr == l_packet)
    {
      mEERROR_S(EERROR_MEMORY);
    }

    return (l_packet);
  }

  /**
    @brief Generator for ENetPacketConnect. /!\ EError.
    @param p_src ENetSocket source of ENetPacket.
    @return Generated ENetPacketConnect on success.
    @return nullptr on failure.
  */
  ENetPacket                  *generateENetPacketConnect(ENetSocket *p_src)
  {
    ENetPacket                *l_packet = nullptr;

    mEERROR_R();
    l_packet = new ENetPacketConnect(p_src);
    if (nullptr == l_packet)
    {
      mEERROR_S(EERROR_MEMORY);
    }

    return (l_packet);
  }

  /**
    @brief Generator for ENetPacketRawDatas. /!\ EError.
    @param p_src ENetSocket source of ENetPacket.
    @return Generated ENetPacketRawDatas on success.
    @return nullptr on failure.
  */
  ENetPacket                  *generateENetPacketRawDatas(ENetSocket *p_src)
  {
    ENetPacket                *l_packet = nullptr;

    mEERROR_R();
    l_packet = new ENetPacketRawDatas(p_src);
    if (nullptr == l_packet)
    {
      mEERROR_S(EERROR_MEMORY);
    }

    return (l_packet);
  }

  /**
    @brief Constructor for ENetPacketHandler.
    @details Add Basics ENetPacketGenerators.
  */
  ENetPacketHandler::ENetPacketHandler() :
    m_generators(),
    m_packets(),
    m_mutexPackets(nullptr)
  {
    m_generators[ENETPACKET_TYPE_DISCONNECT] = generateENetPacketDisconnect;
    m_generators[ENETPACKET_TYPE_CONNECT] = generateENetPacketConnect;
    m_generators[ENETPACKET_TYPE_RAW_DATAS] = generateENetPacketRawDatas;
  }

  /**
    @brief Destructor for ENetPacketHandler.
    @details Release its mutex.
  */
  ENetPacketHandler::~ENetPacketHandler()
  {
    ReleaseMutex(m_mutexPackets);
    CloseHandle(m_mutexPackets);
  }

  /**
    @brief Singleton for ENetPacketHandler. /!\ EError.
    @details Initialize its mutex.
    @return ENetPacketHandler unique instance on success.
    @return nullptr on failure.
  */
  ENetPacketHandler           *ENetPacketHandler::getInstance()
  {
    static ENetPacketHandler  *l_instance = nullptr;

    mEERROR_R();
    if (nullptr == l_instance)
    {
      HANDLE                  l_mutex = nullptr;

      l_mutex = CreateMutex(nullptr, false, nullptr);
      if (nullptr != l_mutex)
      {
        l_instance = new ENetPacketHandler();
        if (nullptr == l_instance)
        {
          l_instance->m_mutexPackets = l_mutex;
        }
        else
        {
          mEERROR_S(EERROR_MEMORY);
          CloseHandle(l_mutex);
        }
      }
      else
      {
        mEERROR_SA(EERROR_WINDOWS_ERR, WindowsErrString(GetLastError()));
      }
    }

    return (l_instance);
  }

  /**
    @brief Pop a ENetPacket from the queue. /!\ Mutex.
    @return First ENetPacket from the queue.
    @return nullptr if queue is empty.
  */
  ENetPacket                  *ENetPacketHandler::popPacket()
  {
    ENetPacket                *l_packet = nullptr;

    WaitForSingleObject(m_mutexPackets, INFINITE);
    if (false == m_packets.empty())
    {
      l_packet = m_packets.front();
      m_packets.pop();
    }
    ReleaseMutex(m_mutexPackets);

    return (l_packet);
  }

  /**
    @brief Receive a ENetPacket from connected ENetSocket source. /!\ Blocking. /!\ Mutex. /!\ EError.
    @details Receive ENetPacketType, then call its ENetPacket...::recv().
    @details Can be blocking on bad behavior, waiting for datas that were not properly sent.
    @details On success, received ENetPacket is added to queue.
    @details Source must be valid.
    @param p_src ENetSocket source.
  */
  void                        ENetPacketHandler::recvPacket(ENetSocket *p_src)
  {
    mEERROR_R();
    if (nullptr == p_src)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if ((nullptr != p_src)
      && (ENETSOCKET_FLAGS_PROTOCOL_TCP != (p_src->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS)))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }

    if (EERROR_NONE == mEERROR)
    {
      int32                   l_len = 0;
      ENetPacketType          l_type = ENETPACKET_TYPE_DISCONNECT;

      l_len = p_src->recv(reinterpret_cast<char*>(&l_type), sizeof(ENetPacketType));
      if (EERROR_NONE == mEERROR)
      {
        if ((sizeof(ENetPacketType) == l_len)
          || (0 == l_len)) // Generate an ENetPacketDisconnect.
        {
          if (m_generators.find(l_type) != m_generators.end())
          {
            ENetPacket        *l_packet = nullptr;

            l_packet = m_generators[l_type](p_src);
            if (nullptr != l_packet)
            {
              l_packet->recv();
              if (EERROR_NONE == mEERROR)
              {
                WaitForSingleObject(m_mutexPackets, INFINITE);
                m_packets.push(l_packet);
                ReleaseMutex(m_mutexPackets);
              }
              else
              {
                mEERROR_SH(EERROR_NET_PACKET_ERR);
              }
            }
            else
            {
              mEERROR_SH(EERROR_NET_PACKETHANDLER_ERR);
            }
          }
          else
          {
            mEERROR_S(EERROR_NET_PACKET_TYPE);
          }
        }
        else
        {
          mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
        }
      }
      else
      {
        mEERROR_SH(EERROR_NET_SOCKET_ERR);
      }
    }
  }

  /**
    @brief Read a ENetPacket from buffer. /!\ Mutex. /!\ EError.
    @details Read ENetPacketType, then call its ENetPacket...::read().
    @details On success, read ENetPacket is added to queue.
    @param p_datas Buffer of datas to be read.
    @param p_len Length of buffer.
    @param p_src ENetSocket source.
  */
  void                        ENetPacketHandler::read(char *p_datas, int32 p_len, ENetSocket *p_src)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if ((sizeof(ENetPacketType) <= p_len)
        || (0 == p_len)) // Generate an ENetPacketDisconnect.
      {
        ENetPacketType        l_type = ENETPACKET_TYPE_DISCONNECT;

        if (0 != p_len)
        {
          l_type = *reinterpret_cast<ENetPacketType*>(p_datas);
        }
        if (m_generators.find(l_type) != m_generators.end())
        {
          ENetPacket          *l_packet = nullptr;

          l_packet = m_generators[l_type](p_src);
          if (nullptr != l_packet)
          {
            l_packet->read(p_datas + sizeof(ENetPacketType), p_len - sizeof(ENetPacketType));
            if (EERROR_NONE == mEERROR)
            {
              l_packet->setSource(p_src);
              WaitForSingleObject(m_mutexPackets, INFINITE);
              m_packets.push(l_packet);
              ReleaseMutex(m_mutexPackets);
            }
            else
            {
              mEERROR_SH(EERROR_NET_PACKET_ERR);
            }
          }
          else
          {
            mEERROR_SH(EERROR_NET_PACKETHANDLER_ERR);
          }
        }
        else
        {
          mEERROR_S(EERROR_NET_PACKET_TYPE);
        }
      }
      else
      {
        mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
      }
    }
  }

  /**
    @brief Add type/generator to automation. /!\ EError.
    @details Type must be over ENETPACKET_TYPE_RESERVED.
    @param p_type Type to be added.
    @param p_generator Generator for type.
  */
  void                        ENetPacketHandler::setGenerator(ENetPacketType p_type, ENetPacketGenerator p_generator)
  {
    mEERROR_R();
    if (ENETPACKET_TYPE_RESERVED >= p_type)
    {
      mEERROR_SA(EERROR_NET_PACKET_TYPE, "Trying to add ENetPacketGenerator with reserved ENetPacketType.");
    }

    if (EERROR_NONE == mEERROR)
    {
      m_generators[p_type] = p_generator;
    }
  }

  /**
    @brief Clean queue from ENetPackets linked to ENetSocket in parameter. /!\ Mutex.
    @param p_socket ENetSocket that will cleaned from queue.
  */
  void                        ENetPacketHandler::cleanSocket(const ENetSocket *p_socket)
  {
    size_t                    l_size = 0;

    WaitForSingleObject(m_mutexPackets, INFINITE);
    l_size = m_packets.size();
    for (size_t l_pos = 0; l_pos < l_size; ++l_pos)
    {
      if (*m_packets.front()->getSource() != *p_socket)
      {
        m_packets.push(m_packets.front());
      }
      m_packets.pop();
    }
    ReleaseMutex(m_mutexPackets);
  }

}
