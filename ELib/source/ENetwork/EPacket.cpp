/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EPacket Class.
*/

#include "ENetwork/EPacket.h"

/**
  @brief General scope for ELib components.
*/
namespace         ELib
{

  /**
    @brief Instantiate a EPacket.
    @param p_type Type of the EPacket.
    @param p_src Pointer to ESocket source of the EPacket.
  */
  EPacket::EPacket(EPacketType p_type, ESocket *p_src) :
    m_type(p_type),
    m_src(p_src)
  {
  }

  /**
    @brief Delete a EPacket.
  */
  EPacket::~EPacket()
  {
  }

  /**
    @brief Send EPacket informations and datas.
    @details Automatically handle the target selection and EPacket informations sending.
    @param p_datas Datas to be send.
    @param p_len Length of datas to be send.
    @param p_dst ESocket destination. This can be null if EPacket source is a connected ESocket.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SOCKET_INVALID if m_src is null or if p_dst is null and m_src protocol is ESOCKET_FLAGS_PROTOCOL_UDP.
    @eerror EERROR_PACKET_TRUNCATED if not all the datas were sent successfully.
    @eerror EERROR_PACKET_SEND if send() failed.
    @errror EERROR_OOM if l_datas couldn't be allocated.
  */
  void            EPacket::send(const char *p_datas, int32 p_len, ESocket *p_dst)
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }
    if ((nullptr == p_dst)
      && (ESOCKET_FLAGS_PROTOCOL_UDP == (m_src->getFlags() & ESOCKET_FLAGS_PROTOCOLS)))
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      char        *l_datas = nullptr;
      int32       l_len = -1;
      int32       l_ret = -1;

      l_len = sizeof(EPacketType) + p_len;
      l_datas = new char[l_len];
      if (nullptr != l_datas)
      {
        memcpy(l_datas, &m_type, sizeof(ELib::EPacketType));
        memcpy(l_datas + sizeof(ELib::EPacketType), p_datas, p_len);
        if (nullptr == p_dst)
        {
          l_ret = m_src->send(l_datas, l_len);
        }
        else
        {
          if (ESOCKET_FLAGS_PROTOCOL_TCP == (p_dst->getFlags() & ESOCKET_FLAGS_PROTOCOLS))
          {
            l_ret = p_dst->send(l_datas, l_len);
          }
          if (ESOCKET_FLAGS_PROTOCOL_UDP == (p_dst->getFlags() & ESOCKET_FLAGS_PROTOCOLS))
          {
            l_ret = m_src->sendto(l_datas, l_len, p_dst);
          }
        }
        if (EERROR_NONE == mEERROR)
        {
          if (l_ret < l_len)
          {
            mEERROR_S(EERROR_PACKET_TRUNCATED);
          }
        }
        else
        {
          mEERROR_SA(EERROR_PACKET_SEND, mEERROR_G.toString());
        }
        delete (l_datas);
      }
      else
      {
        mEERROR_S(EERROR_OOM);
      }
    }
  }

  /**
    @brief Get type of the EPacket.
    @return EPacketType of the EPacket.
  */
  EPacketType     EPacket::getType() const
  {
    return (m_type);
  }

  /**
    @brief Get the ESocket source of the EPacket.
    @return Pointer to ESocket source of the EPacket.
  */
  const ESocket   *EPacket::getSource() const
  {
    return (m_src);
  }

  /**
    @brief Define the ESocket source of the EPacket.
    @param p_src Pointer to ESocket source of the EPacket.
  */
  void            EPacket::setSource(ESocket *p_src)
  {
    m_src = p_src;
  }

  /**
    @brief Instantiate a EPacketDisconnect.
    @param p_src Pointer to ESocket source of the EPacketDisconnect.
  */
  EPacketDisconnect::EPacketDisconnect(ESocket *p_src) :
    EPacket(EPACKET_TYPE_DISCONNECT, p_src)
  {
  }

  /**
    @brief Delete a EPacketDisconnect.
  */
  EPacketDisconnect::~EPacketDisconnect()
  {
  }

  /**
    @brief Receive a EPacketDisconnect from ESocket source. Used for SOCK_STREAM protocols.
    @details Handle the reception of a EPacketDisconnect from ESocket source linked.
  */
  void            EPacketDisconnect::recv()
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }
  }

  /**
    @brief Generate a EPacketDisconnect from buffer in parameter. Used for SOCK_DGRAM protocols.
    @details Handle the generation of a EPacketDisconnect from the datas passed in parameters.
    @param p_datas Pointer to buffer that contains the datas of EPacketDisconnect.
    @param p_len Length of p_datas.
  */
  void            EPacketDisconnect::generate(const char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_PACKET_TRUNCATED);
    }
  }

  /**
    @brief Send a EPacketDisconnect.
    @details Handle the transmission of a EPacketDisconnect from ESocket source in EPacket to ESocket destination.
    @details ESocket destination is taken from parameter if provided, or is source if connected.
    @param p_dst Pointer to ESocket of destination.
  */
  void            EPacketDisconnect::send(ESocket *p_dst)
  {
    EPacket::send(nullptr, 0, p_dst);
    //}
  }

  /**
    @brief Instantiate a EPacketConnect.
    @param p_src Pointer to ESocket source of the EPacketConnect.
  */
  EPacketConnect::EPacketConnect(ESocket *p_src) :
    EPacket(EPACKET_TYPE_CONNECT, p_src)
  {
  }

  /**
    @brief Delete a EPacketConnect.
  */
  EPacketConnect::~EPacketConnect()
  {
  }

  /**
    @brief Receive a EPacketConnect from ESocket source. Used for SOCK_STREAM protocols.
    @details Handle the reception of a EPacketConnect from ESocket source linked.
  */
  void            EPacketConnect::recv()
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }
  }

  /**
    @brief Generate a EPacketConnect from buffer in parameter. Used for SOCK_DGRAM protocols.
    @details Handle the generation of a EPacketConnect from the datas passed in parameters.
    @param p_datas Pointer to buffer that contains the datas of EPacketConnect.
    @param p_len Length of p_datas.
  */
  void            EPacketConnect::generate(const char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_PACKET_TRUNCATED);
    }
  }

  /**
    @brief Send a EPacketConnect.
    @details Handle the transmission of a EPacketConnect from ESocket source in EPacket to ESocket destination.
    @details ESocket destination is taken from parameter if provided, or is source if connected.
    @param p_dst Pointer to ESocket of destination.
  */
  void            EPacketConnect::send(ESocket *p_dst)
  {
    EPacket::send(nullptr, 0, p_dst);
  }

  /**
    @brief Instantiate a EPacketRawDatas.
    @param p_src Pointer to ESocket source of the EPacketRawDatas.
  */
  EPacketRawDatas::EPacketRawDatas(ESocket *p_src) :
    EPacket(EPACKET_TYPE_RAW_DATAS, p_src),
    m_len(0),
    m_datas(nullptr)
  {
  }

  /**
    @brief Delete a EPacketRawDatas.
  */
  EPacketRawDatas::~EPacketRawDatas()
  {
  }

  /**
    @brief Receive a EPacketRawDatas from ESocket source. Used for SOCK_STREAM protocols.
    @details Handle the reception of a EPacketRawDatas from ESocket source linked.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SOCKET_INVALID if ESocket source is null.
    @eerror EERROR_PACKET_TRUNCATED if EPacket can't be fully retrieved from ESocket source.
    @eerror EERROR_PACKET_RECV if recv() failed.
  */
  void            EPacketRawDatas::recv()
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      int32       l_len = -1;

      l_len = m_src->recv(reinterpret_cast<char*>(&m_len), sizeof(int32));
      if (EERROR_NONE == mEERROR)
      {
        if (sizeof(int32) == l_len)
        {
          m_datas = new char[m_len];
          if (nullptr != m_datas)
          {
            l_len = m_src->recv(m_datas, m_len);
            if (EERROR_NONE == mEERROR)
            {
              if (m_len > l_len)
              {
                mEERROR_S(EERROR_PACKET_TRUNCATED);
              }
            }
            else
            {
              mEERROR_SA(EERROR_PACKET_RECV, mEERROR_G.toString());
            }
            if (EERROR_NONE != mEERROR)
            {
              m_len = 0;
              delete (m_datas);
              m_datas = nullptr;
            }
          }
          else
          {
            mEERROR_S(EERROR_OOM);
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
    @brief Generate a EPacketRawDatas from buffer in parameter. Used for SOCK_DGRAM protocols.
    @details Handle the generation of a EPacketRawDatas from the datas passed in parameters.
    @param p_datas Pointer to buffer that contains the datas of EPacketRawDatas.
    @param p_len Length of p_datas.
    @eerror EERROR_NONE in success.
    @eerror EERROR_PACKET_TRUNCATED if p_datas is null or EPacket can't be fully retrieved from the buffer.
  */
  void            EPacketRawDatas::generate(const char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_PACKET_TRUNCATED);
    }

    if (sizeof(int32) <= p_len)
    {
      if (*reinterpret_cast<const int32*>(p_datas) == p_len - sizeof(int32))
      {
        m_len = *reinterpret_cast<const int32*>(p_datas);
        m_datas = new char[m_len];
        if (nullptr != m_datas)
        {
          memcpy(m_datas, p_datas + sizeof(int32), m_len);
        }
        else
        {
          mEERROR_S(EERROR_OOM);
        }
      }
      else
      {
        mEERROR_S(EERROR_PACKET_TRUNCATED);
      }
    }
    else
    {
      mEERROR_S(EERROR_PACKET_TRUNCATED);
    }
  }

  /**
    @brief Send a EPacketRawDatas.
    @details Handle the transmission of a EPacketRawDatas from ESocket source in EPacket to ESocket destination.
    @details ESocket destination is taken from parameter if provided, or is source if connected.
    @param p_dst Pointer to ESocket of destination.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SOCKET_INVALID if ESocket m_src is null.
    @eerror EERROR_PACKET_TRUNCATED if not all the datas were sent successfully.
    @eerror EERROR_PACKET_SEND if send() failed.
  */
  void            EPacketRawDatas::send(ESocket *p_dst)
  {
    char          *l_datas = nullptr;
    int32         l_len = -1;

    mEERROR_R();
    l_len = sizeof(int32) + m_len;
    l_datas = new char[l_len];
    if (nullptr != l_datas)
    {
      memcpy(l_datas, &l_len, sizeof(int32));
      memcpy(l_datas + sizeof(int32), l_datas, l_len);
    }
    else
    {
      mEERROR_S(EERROR_OOM);
    }

    if (EERROR_NONE == mEERROR)
    {
      EPacket::send(m_datas, m_len, p_dst);
    }
  }

  /**
    @brief Get length of datas in the EPacketRawDatas.
    @return Length of datas in the EPacketRawDatas.
  */
  int32           EPacketRawDatas::getLength() const
  {
    return (m_len);
  }

  /**
    @brief Set length of datas in the EPacketRawDatas.
    @param p_len Length of the datas in the EPacketRawDatas.
  */
  void            EPacketRawDatas::setLength(int32 p_len)
  {
    m_len = p_len;
  }

  /**
    @brief Get datas of the EPacketRawDatas.
    @return Pointer to the datas in the EPacketRawDatas.
  */
  const char      *EPacketRawDatas::getDatas() const
  {
    return (m_datas);
  }

  /**
    @brief Set datas of the EPacketRawDatas.
    @param p_datas Pointer to the datas to set in the EPacketRawDatas.
  */
  void            EPacketRawDatas::setDatas(char *p_datas)
  {
    m_datas = p_datas;
  }

}
