/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetPacketHandler Class.
*/

#include "ENetwork/ENetPacketHandler.h"

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief Generator for ENetPacketDisconnect.
    @param p_src Pointer to ENetSocket source of ENetPacket.
    @return Pointer to generated ENetPacketDisconnect.
  */
  ENetPacket            *generateENetPacketDisconnect(ENetSocket *p_src)
  {
    return (new ENetPacketDisconnect(p_src));
  }

  /**
    @brief Generator for ENetPacketConnect.
    @param p_src Pointer to ENetSocket source of ENetPacket.
    @return Pointer to generated ENetPacketConnect.
  */
  ENetPacket            *generateENetPacketConnect(ENetSocket *p_src)
  {
    return (new ENetPacketConnect(p_src));
  }

  /**
    @brief Generator for ENetPacketRawDatas.
    @param p_src Pointer to ENetSocket source of ENetPacket.
    @return Pointer to generated ENetPacketRawDatas.
  */
  ENetPacket            *generateENetPacketRawDatas(ENetSocket *p_src)
  {
    return (new ENetPacketRawDatas(p_src));
  }

  /**
    @brief Instantiate a ENetPacketHandler.
    @details Initialize its mutex and add the ELib ENetPacketGenerators.
    @details It should be unique in a EServer or EClient.
  */
  ENetPacketHandler::ENetPacketHandler() :
    m_generators({}),
    m_packets(),
    m_mutexPackets(nullptr)
  {
    m_mutexPackets = CreateMutex(nullptr, false, nullptr);
    m_generators[ENETPACKET_TYPE_DISCONNECT] = generateENetPacketDisconnect;
    m_generators[ENETPACKET_TYPE_CONNECT] = generateENetPacketConnect;
    m_generators[ENETPACKET_TYPE_RAW_DATAS] = generateENetPacketRawDatas;
  }

  /**
    @brief Delete a ENetPacketHandler.
    @details Release its mutex.
  */
  ENetPacketHandler::~ENetPacketHandler()
  {
    ReleaseMutex(m_mutexPackets);
    CloseHandle(m_mutexPackets);
  }

  /**
    @brief Pop a ENetPacket from the queue, FIFO order.
    @return Pointer to first ENetPacket from the queue.
  */
  ENetPacket            *ENetPacketHandler::getPacket()
  {
    ENetPacket          *l_packet = nullptr;

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
    @brief Receive a ENetPacket from the ENetSocket in parameter.
    @details Automatically handle the reception of a ENetPacket from a ENetSocket source.
    @param p_src Pointer to ENetSocket source.
    @eerror EERROR_NONE in success. The received ENetPacket is added to the queue.
    @eerror EERROR_NET_SOCKET_INVALID if p_src is null.
    @eerror EERROR_NET_PACKET_INVALID if no ENetPacketGenerator is define for the ENetPacketType received.
    @eerror EERROR_OOM if ENetPacket allocation fail.
    @eerror EERROR_NET_PACKET_TRUNCATED if ENetPacket can't be fully retrieved from the buffer.
    @eerror EERROR_NET_PACKET_RECV if recv() failed.
  */
  void                  ENetPacketHandler::recvPacket(ENetSocket *p_src)
  {
    mEERROR_R();
    if (nullptr == p_src)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      int32             l_len = 0;
      ENetPacketType    l_type = ENETPACKET_TYPE_DISCONNECT;

      l_len = p_src->recv(reinterpret_cast<char*>(&l_type), sizeof(ENetPacketType));
      if (EERROR_NONE == mEERROR)
      {
        if ((sizeof(ENetPacketType) == l_len)
          || (0 == l_len))
        {
          if (m_generators.find(l_type) != m_generators.end())
          {
            ENetPacket  *l_packet = nullptr;

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
                mEERROR_SA(EERROR_NET_PACKET_RECV, mEERROR_G.toString());
              }
            }
            else
            {
              mEERROR_S(EERROR_OOM);
            }
          }
          else
          {
            mEERROR_S(EERROR_NET_PACKET_INVALID);
          }
        }
        else
        {
          mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
        }
      }
      else
      {
        mEERROR_SA(EERROR_NET_PACKET_RECV, mEERROR_G.toString());
      }
    }
  }

  /**
    @brief Generate a ENetPacket from the buffer in parameter.
    @details Automatically handle the generation of a ENetPacket from a buffer.
    @param p_src Pointer to ENetSocket source.
    @param p_datas Pointer to buffer that contains the datas of ENetPacket.
    @param p_len Length of the buffer.
    @eerror EERROR_NONE in success. The received ENetPacket is added to the queue.
    @eerror EERROR_NET_SOCKET_INVALID if p_src is null.
    @eerror EERROR_OOM if ENetPacket allocation fail.
    @eerror EERROR_NET_PACKET_INVALID if no ENetPacketGenerator is define for the ENetPacketType received.
    @eerror EERROR_NET_PACKET_TRUNCATED if generate() failed.
    @eerror EERROR_NULL_PTR if p_datas is null.
  */
  void                  ENetPacketHandler::generate(ENetSocket *p_src, char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_src)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if ((sizeof(ENetPacketType) <= p_len)
        || (0 == p_len))
      {
        ENetPacketType  l_type = ENETPACKET_TYPE_DISCONNECT;

        if (0 != p_len)
        {
          l_type = *reinterpret_cast<ENetPacketType*>(p_datas);
        }
        if (m_generators.find(l_type) != m_generators.end())
        {
          ENetPacket    *l_packet = nullptr;

          l_packet = m_generators[l_type](p_src);
          if (nullptr != l_packet)
          {
            l_packet->generate(p_datas + sizeof(ENetPacketType), p_len - sizeof(ENetPacketType));
            if (EERROR_NONE == mEERROR)
            {
              l_packet->setSource(p_src);
              WaitForSingleObject(m_mutexPackets, INFINITE);
              m_packets.push(l_packet);
              ReleaseMutex(m_mutexPackets);
            }
            else
            {
              mEERROR_SA(EERROR_NET_PACKET_TRUNCATED, mEERROR_G.toString());
            }
          }
          else
          {
            mEERROR_S(EERROR_OOM);
          }
        }
        else
        {
          mEERROR_S(EERROR_NET_PACKET_INVALID);
        }
      }
      else
      {
        mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
      }
    }
  }

  /**
    @brief Add a ENetPacketGenerator to automation.
    @details An ENetPacketGenerator is a pointer to function that generate a ENetPacket.
    @details User can add custom ENetPacketGenerator with type over ENETPACKET_TYPE_RESERVED.
    @param p_type ENetPacketType of ENetPacket that the ENetPacketGenerator handle.
    @param p_generator ENetPacketGenerator that is add to automation.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_PACKET_RESERVED if the ENetPacketType is in range of ENETPACKET_TYPE_RESERVED.
  */
  void                  ENetPacketHandler::setGenerator(ENetPacketType type, ENetPacketGenerator generator)
  {
    mEERROR_R();
    if (ENETPACKET_TYPE_RESERVED >= type)
    {
      mEERROR_S(EERROR_NET_PACKET_RESERVED);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_generators[type] = generator;
    }
  }

  /**
    @brief Clean the queue from every ENetPackets linked to ENetSocket parameter.
    @param p_socket Pointer to ENetSocket that will cleaned from the queue.
  */
  void                  ENetPacketHandler::cleanSocket(const ENetSocket *p_socket)
  {
    size_t              l_size = 0;

    l_size = m_packets.size();
    WaitForSingleObject(m_mutexPackets, INFINITE);
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
