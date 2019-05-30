/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EPacketHandler Class.
*/

#include "ENetwork/EPacketHandler.h"

#define MAX_DGRAM_LENGTH  (65507) /**< Max length of buffer for recvfrom call. */

/**
  @brief General scope for ELib components.
*/
namespace                 ELib
{

  /**
    @brief Generator for EPacketDisconnect.
    @param p_src Pointer to ESocket source of EPacket.
    @return Pointer to generated EPacketDisconnect.
  */
  EPacket                 *generateEPacketDisconnect(ESocket *p_src)
  {
    return (new EPacketDisconnect(p_src));
  }

  /**
    @brief Generator for EPacketConnect.
    @param p_src Pointer to ESocket source of EPacket.
    @return Pointer to generated EPacketConnect.
  */
  EPacket                 *generateEPacketConnect(ESocket *p_src)
  {
    return (new EPacketConnect(p_src));
  }

  /**
    @brief Generator for EPacketRawDatas.
    @param p_src Pointer to ESocket source of EPacket.
    @return Pointer to generated EPacketRawDatas.
  */
  EPacket                 *generateEPacketRawDatas(ESocket *p_src)
  {
    return (new EPacketRawDatas(p_src));
  }

  /**
    @brief Instantiate a EPacketHandler.
    @details Initialize its mutex and add the ELib EPacketGenerators.
    @details It should be unique in a EServer or EClient.
  */
  EPacketHandler::EPacketHandler() :
    m_generators({}),
    m_packets(),
    m_mutexPackets(nullptr)
  {
    m_mutexPackets = CreateMutex(nullptr, false, nullptr);
    m_generators[EPACKET_TYPE_DISCONNECT] = generateEPacketDisconnect;
    m_generators[EPACKET_TYPE_CONNECT] = generateEPacketConnect;
    m_generators[EPACKET_TYPE_RAW_DATAS] = generateEPacketRawDatas;
  }

  /**
    @brief Delete a EPacketHandler.
    @details Release its mutex.
  */
  EPacketHandler::~EPacketHandler()
  {
    ReleaseMutex(m_mutexPackets);
    CloseHandle(m_mutexPackets);
  }

  /**
    @brief Pop a EPacket from the queue, FIFO order.
    @return Pointer to first EPacket from the queue.
  */
  EPacket                 *EPacketHandler::getPacket()
  {
    EPacket               *l_packet = nullptr;

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
    @brief Receive a EPacket from the ESocket in parameter.
    @details Automatically handle the reception of a EPacket from a ESocket source.
    @param p_src Pointer to ESocket source.
    @eerror EERROR_NONE in success. The received EPacket is added to the queue.
    @eerror EERROR_SOCKET_INVALID if p_src is null.
    @eerror EERROR_PACKET_INVALID if no EPacketGenerator is define for the EPacketType received.
    @eerror EERROR_OOM if EPacket allocation fail.
    @eerror EERROR_PACKET_TRUNCATED if EPacket can't be fully retrieved from the buffer.
    @eerror EERROR_PACKET_RECV if recv() failed.
  */
  void                    EPacketHandler::recvPacket(ESocket *p_src)
  {
    mEERROR_R();
    if (nullptr == p_src)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      int32               l_len = 0;
      EPacketType         l_type = EPACKET_TYPE_DISCONNECT;

      l_len = p_src->recv(reinterpret_cast<char*>(&l_type), sizeof(EPacketType));
      if (EERROR_NONE == mEERROR_G.m_errorCode)
      {
        if ((sizeof(EPacketType) == l_len)
          || (0 == l_len))
        {
          if (m_generators.find(l_type) != m_generators.end())
          {
            EPacket       *l_packet = nullptr;

            l_packet = m_generators[l_type](p_src);
            if (nullptr != l_packet)
            {
              l_packet->recv();
              if (EERROR_NONE == mEERROR_G.m_errorCode)
              {
                WaitForSingleObject(m_mutexPackets, INFINITE);
                m_packets.push(l_packet);
                ReleaseMutex(m_mutexPackets);
              }
              else
              {
                mEERROR_SA(EERROR_PACKET_RECV, mEERROR_G.toString());
              }
            }
            else
            {
              mEERROR_S(EERROR_OOM);
            }
          }
          else
          {
            mEERROR_S(EERROR_PACKET_INVALID);
          }
        }
        else
        {
          mEERROR_S(EERROR_PACKET_TRUNCATED);
        }
      }
      else
      {
        mEERROR_SA(EERROR_PACKET_RECV, mEERROR_G.toString());
      }
    }
  }

  /**
    @brief Generate a EPacket from the buffer in parameter.
    @details Automatically handle the generation of a EPacket from a buffer.
    @param p_src Pointer to ESocket source.
    @param p_datas Pointer to buffer that contains the datas of EPacket.
    @param p_len Length of the buffer.
    @eerror EERROR_NONE in success. The received EPacket is added to the queue.
    @eerror EERROR_SOCKET_INVALID if p_src is null.
    @eerror EERROR_OOM if EPacket allocation fail.
    @eerror EERROR_PACKET_INVALID if no EPacketGenerator is define for the EPacketType received.
    @eerror EERROR_PACKET_TRUNCATED if p_datas is null or generate() failed.
  */
  void                    EPacketHandler::generate(ESocket *p_src, char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_src)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_PACKET_TRUNCATED);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      if ((sizeof(EPacketType) <= p_len)
        || (0 == p_len))
      {
        EPacketType       l_type = EPACKET_TYPE_DISCONNECT;

        if (0 != p_len)
        {
          l_type = *reinterpret_cast<EPacketType*>(p_datas);
        }
        if (m_generators.find(l_type) != m_generators.end())
        {
          EPacket         *l_packet = nullptr;

          l_packet = m_generators[l_type](p_src);
          if (nullptr != l_packet)
          {
            l_packet->generate(p_datas + sizeof(EPacketType), p_len - sizeof(EPacketType));
            if (EERROR_NONE == mEERROR_G.m_errorCode)
            {
              l_packet->setSource(p_src);
              WaitForSingleObject(m_mutexPackets, INFINITE);
              m_packets.push(l_packet);
              ReleaseMutex(m_mutexPackets);
            }
            else
            {
              mEERROR_SA(EERROR_PACKET_TRUNCATED, mEERROR_G.toString());
            }
          }
          else
          {
            mEERROR_S(EERROR_OOM);
          }
        }
        else
        {
          mEERROR_S(EERROR_PACKET_INVALID);
        }
      }
      else
      {
        mEERROR_S(EERROR_PACKET_TRUNCATED);
      }
    }
  }

  /**
    @brief Add a EPacketGenerator to automation.
    @details An EPacketGenerator is a pointer to function that generate a EPacket.
    @details User can add custom EPacketGenerator with type over EPACKET_TYPE_RESERVED.
    @param p_type EPacketType of EPacket that the EPacketGenerator handle.
    @param p_generator EPacketGenerator that is add to automation.
    @eerror EERROR_NONE in success.
    @eerror EERROR_PACKET_RESERVED if the EPacketType is in range of EPACKET_TYPE_RESERVED.
  */
  void                    EPacketHandler::setGenerator(EPacketType type, EPacketGenerator generator)
  {
    mEERROR_R();
    if (EPACKET_TYPE_RESERVED < type)
    {
      mEERROR_S(EERROR_PACKET_RESERVED);
    }

    if (EERROR_NONE == mEERROR_G.m_errorCode)
    {
      m_generators[type] = generator;
    }
  }

  /**
    @brief Clean the queue from every EPackets linked to ESocket parameter.
    @param p_socket Pointer to ESocket that will cleaned from the queue.
  */
  void                    EPacketHandler::cleanSocket(const ESocket *p_socket)
  {
    size_t                l_size = 0;

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
