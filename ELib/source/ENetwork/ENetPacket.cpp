/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetPacket Class.
*/

#include "ENetwork/ENetPacket.h"

/**
  @brief General scope for ELib components.
*/
namespace           ELib
{

  /**
    @brief Instantiate a ENetPacket.
    @param p_type Type of the ENetPacket.
    @param p_src Pointer to ENetSocket source of the ENetPacket.
  */
  ENetPacket::ENetPacket(ENetPacketType p_type, ENetSocket *p_src) :
    m_type(p_type),
    m_src(p_src)
  {
  }

  /**
    @brief Delete a ENetPacket.
  */
  ENetPacket::~ENetPacket()
  {
  }

  /**
    @brief Send ENetPacket informations and datas.
    @details Automatically handle the target selection and ENetPacket informations sending.
    @param p_datas Datas to be send.
    @param p_len Length of datas to be send.
    @param p_dst ENetSocket destination. This can be null if ENetPacket source is a connected ENetSocket.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SOCKET_INVALID if m_src is null or if p_dst is null and m_src protocol is ENETSOCKET_FLAGS_PROTOCOL_UDP.
    @eerror EERROR_NET_PACKET_TRUNCATED if not all the datas were sent successfully.
    @eerror EERROR_NET_PACKET_SEND if send() failed.
    @errror EERROR_OOM if l_datas couldn't be allocated.
  */
  void              ENetPacket::send(const char *p_datas, int32 p_len, ENetSocket *p_dst)
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }
    if ((nullptr == p_dst)
      && (ENETSOCKET_FLAGS_PROTOCOL_UDP == (m_src->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS)))
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      char          *l_datas = nullptr;
      int32         l_len = -1;
      int32         l_ret = -1;

      l_len = sizeof(ENetPacketType) + p_len;
      l_datas = new char[l_len];
      if (nullptr != l_datas)
      {
        memcpy(l_datas, &m_type, sizeof(ENetPacketType));
        memcpy(l_datas + sizeof(ENetPacketType), p_datas, p_len);
        if (nullptr == p_dst)
        {
          l_ret = m_src->send(l_datas, l_len);
        }
        else
        {
          if (ENETSOCKET_FLAGS_PROTOCOL_TCP == (p_dst->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS))
          {
            l_ret = p_dst->send(l_datas, l_len);
          }
          if (ENETSOCKET_FLAGS_PROTOCOL_UDP == (p_dst->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS))
          {
            l_ret = m_src->sendto(l_datas, l_len, p_dst);
          }
        }
        if (EERROR_NONE == mEERROR)
        {
          if (l_ret < l_len)
          {
            mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
          }
        }
        else
        {
          mEERROR_SA(EERROR_NET_PACKET_SEND, mEERROR_G.toString());
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
    @brief Get type of the ENetPacket.
    @return ENetPacketType of the ENetPacket.
  */
  ENetPacketType    ENetPacket::getType() const
  {
    return (m_type);
  }

  /**
    @brief Get the ENetSocket source of the ENetPacket.
    @return Pointer to ENetSocket source of the ENetPacket.
  */
  const ENetSocket  *ENetPacket::getSource() const
  {
    return (m_src);
  }

  /**
    @brief Define the ENetSocket source of the ENetPacket.
    @param p_src Pointer to ENetSocket source of the ENetPacket.
  */
  void              ENetPacket::setSource(ENetSocket *p_src)
  {
    m_src = p_src;
  }

  /**
    @brief Instantiate a ENetPacketDisconnect.
    @param p_src Pointer to ENetSocket source of the ENetPacketDisconnect.
  */
  ENetPacketDisconnect::ENetPacketDisconnect(ENetSocket *p_src) :
    ENetPacket(ENETPACKET_TYPE_DISCONNECT, p_src)
  {
  }

  /**
    @brief Delete a ENetPacketDisconnect.
  */
  ENetPacketDisconnect::~ENetPacketDisconnect()
  {
  }

  /**
    @brief Receive a ENetPacketDisconnect from ENetSocket source. Used for connected protocols.
    @details Handle the reception of a ENetPacketDisconnect from ENetSocket source linked.
  */
  void              ENetPacketDisconnect::recv()
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }
  }

  /**
    @brief Generate a ENetPacketDisconnect from buffer in parameter. Used for connectionless protocols.
    @details Handle the generation of a ENetPacketDisconnect from the datas passed in parameters.
    @param p_datas Pointer to buffer that contains the datas of ENetPacketDisconnect.
    @param p_len Length of p_datas.
    @eerror EERROR_NULL_PTR îf p_datas is null.
  */
  void              ENetPacketDisconnect::generate(const char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
  }

  /**
    @brief Send a ENetPacketDisconnect.
    @details Handle the transmission of a ENetPacketDisconnect from ENetSocket source in ENetPacket to ENetSocket destination.
    @details ENetSocket destination is taken from parameter if provided, or is source if connected.
    @param p_dst Pointer to ENetSocket of destination.
  */
  void              ENetPacketDisconnect::send(ENetSocket *p_dst)
  {
    ENetPacket::send(nullptr, 0, p_dst);
  }

  /**
    @brief Instantiate a ENetPacketConnect.
    @param p_src Pointer to ENetSocket source of the ENetPacketConnect.
  */
  ENetPacketConnect::ENetPacketConnect(ENetSocket *p_src) :
    ENetPacket(ENETPACKET_TYPE_CONNECT, p_src)
  {
  }

  /**
    @brief Delete a ENetPacketConnect.
  */
  ENetPacketConnect::~ENetPacketConnect()
  {
  }

  /**
    @brief Receive a ENetPacketConnect from ENetSocket source. Used for connected protocols.
    @details Handle the reception of a ENetPacketConnect from ENetSocket source linked.
  */
  void              ENetPacketConnect::recv()
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }
  }

  /**
    @brief Generate a ENetPacketConnect from buffer in parameter. Used for connectionless protocols.
    @details Handle the generation of a ENetPacketConnect from the datas passed in parameters.
    @param p_datas Pointer to buffer that contains the datas of ENetPacketConnect.
    @param p_len Length of p_datas.
    @eerror EERROR_NULL_PTR îf p_datas is null.
  */
  void              ENetPacketConnect::generate(const char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
  }

  /**
    @brief Send a ENetPacketConnect.
    @details Handle the transmission of a ENetPacketConnect from ENetSocket source in ENetPacket to ENetSocket destination.
    @details ENetSocket destination is taken from parameter if provided, or is source if connected.
    @param p_dst Pointer to ENetSocket of destination.
  */
  void              ENetPacketConnect::send(ENetSocket *p_dst)
  {
    ENetPacket::send(nullptr, 0, p_dst);
  }

  /**
    @brief Instantiate a ENetPacketRawDatas.
    @param p_src Pointer to ENetSocket source of the ENetPacketRawDatas.
  */
  ENetPacketRawDatas::ENetPacketRawDatas(ENetSocket *p_src) :
    ENetPacket(ENETPACKET_TYPE_RAW_DATAS, p_src),
    m_len(0),
    m_datas(nullptr)
  {
  }

  /**
    @brief Delete a ENetPacketRawDatas.
  */
  ENetPacketRawDatas::~ENetPacketRawDatas()
  {
    if (nullptr != m_datas)
    {
      delete (m_datas);
    }
  }

  /**
    @brief Receive a ENetPacketRawDatas from ENetSocket source. Used for connected protocols.
    @details Handle the reception of a ENetPacketRawDatas from ENetSocket source linked.
    @details m_
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SOCKET_INVALID if ENetSocket source is null.
    @eerror EERROR_NET_PACKET_TRUNCATED if ENetPacket can't be fully retrieved from ENetSocket source.
    @eerror EERROR_NET_PACKET_RECV if recv() failed.
  */
  void              ENetPacketRawDatas::recv()
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_NET_SOCKET_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      int32         l_len = -1;

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
                mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
              }
            }
            else
            {
              mEERROR_SA(EERROR_NET_PACKET_RECV, mEERROR_G.toString());
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
    @brief Generate a ENetPacketRawDatas from buffer in parameter. Used for connectionless protocols.
    @details Handle the generation of a ENetPacketRawDatas from the datas passed in parameters.
    @param p_datas Pointer to buffer that contains the datas of ENetPacketRawDatas.
    @param p_len Length of p_datas.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NULL_PTR îf p_datas is null.
    @eerror EERROR_NET_PACKET_TRUNCATED if ENetPacket can't be fully retrieved from the buffer.
  */
  void              ENetPacketRawDatas::generate(const char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
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
        mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
      }
    }
    else
    {
      mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
    }
  }

  /**
    @brief Send a ENetPacketRawDatas.
    @details Handle the transmission of a ENetPacketRawDatas from ENetSocket source in ENetPacket to ENetSocket destination.
    @details ENetSocket destination is taken from parameter if provided, or is source if connected.
    @param p_dst Pointer to ENetSocket of destination.
    @eerror EERROR_NONE in success.
    @eerror EERROR_NET_SOCKET_INVALID if ENetSocket m_src is null.
    @eerror EERROR_NET_PACKET_TRUNCATED if not all the datas were sent successfully.
    @eerror EERROR_NET_PACKET_SEND if send() failed.
    @eerror EERROR_NULL_PTR îf p_datas is null.
  */
  void              ENetPacketRawDatas::send(ENetSocket *p_dst)
  {
    char            *l_datas = nullptr;
    int32           l_len = -1;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_len = sizeof(int32) + m_len;
      l_datas = new char[l_len];
      if (nullptr != l_datas)
      {
        memcpy(l_datas, &l_len, sizeof(int32));
        memcpy(l_datas + sizeof(int32), l_datas, l_len);
        ENetPacket::send(l_datas, l_len, p_dst);
        delete (l_datas);
      }
      else
      {
        mEERROR_S(EERROR_OOM);
      }
    }
  }

  /**
    @brief Get length of datas in the ENetPacketRawDatas.
    @return Length of datas in the ENetPacketRawDatas.
  */
  int32             ENetPacketRawDatas::getLength() const
  {
    return (m_len);
  }

  /**
    @brief Set length of datas in the ENetPacketRawDatas.
    @param p_len Length of the datas in the ENetPacketRawDatas.
  */
  void              ENetPacketRawDatas::setLength(int32 p_len)
  {
    m_len = p_len;
  }

  /**
    @brief Get datas of the ENetPacketRawDatas.
    @return Pointer to the datas in the ENetPacketRawDatas.
  */
  const char        *ENetPacketRawDatas::getDatas() const
  {
    return (m_datas);
  }

  /**
    @brief Set datas of the ENetPacketRawDatas.
    @param p_datas Pointer to the datas to set in the ENetPacketRawDatas.
  */
  void              ENetPacketRawDatas::setDatas(char *p_datas)
  {
    m_datas = p_datas;
  }

}
