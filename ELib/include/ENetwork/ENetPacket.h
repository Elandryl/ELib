/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetPacket Class.
*/

#pragma once

#include "EGlobals/EGlobal.h"
#include "ENetwork/ENetSocket.h"

/**
  @brief General scope for ELib components.
*/
namespace             ELib
{

  /**
    @brief Reserved ENetPacketType.
    @brief Custom type can be define with value over ENETPACKET_TYPE_RESERVED.
  */
  enum                ENetPacketType
  {
    ENETPACKET_TYPE_DISCONNECT  = 0x0000,
    ENETPACKET_TYPE_CONNECT     = 0x0001,
    ENETPACKET_TYPE_RAW_DATAS   = 0x0002,
    ENETPACKET_TYPE_RESERVED    = 0x000F  /**< Reserved types range. */
  };

  /**
    @brief ELib object for packet handling.
    @details Derived class for each type need to be provide to ENetPacketHandler automation.
    @details Default send() is provided. If not used, derived send() must send type.
    @details read() need to copy datas in its own space. Originals datas are deleted at automation.
  */
  class               ENetPacket
  {
  public:
    ENetPacket(ENetPacketType p_type, ENetSocket *p_src);                         /**< /!\ .... */
    virtual ~ENetPacket();                                                        /**< /!\ .... */
    virtual void      recv() = 0;
    virtual void      read(const char *p_datas, int32 p_len) = 0;
    virtual void      send(ENetSocket *p_dst = nullptr) = 0;
    virtual void      send(const char *p_datas, int32 p_len, ENetSocket *p_dst);  /**< /!\ ..E. */
    ENetPacketType    getType() const;                                            /**< /!\ .... */
    const ENetSocket  *getSource() const;                                         /**< /!\ .... */
    void              setSource(ENetSocket *p_src);                               /**< /!\ .... */

  protected:
    ENetPacketType    m_type; /**< Type. */
    ENetSocket        *m_src; /**< ENetPacket source. */
  };

  /**
    @brief ENetPacket for disconnection.
  */
  class               ENetPacketDisconnect : public ENetPacket
  {
  public:
    ENetPacketDisconnect(ENetSocket *p_src = nullptr);                            /**< /!\ .... */
    ~ENetPacketDisconnect();                                                      /**< /!\ .... */
    void              recv();                                                     /**< /!\ .... */
    void              read(const char *p_datas = nullptr, int32 p_len = 0);       /**< /!\ .... */
    void              send(ENetSocket *p_dst = nullptr);                          /**< /!\ ..E. */
  };

  /**
    @brief ENetPacket for connection.
  */
  class               ENetPacketConnect : public ENetPacket
  {
  public:
    ENetPacketConnect(ENetSocket *p_src = nullptr);                               /**< /!\ .... */
    ~ENetPacketConnect();                                                         /**< /!\ .... */
    void              recv();                                                     /**< /!\ .... */
    void              read(const char *p_datas = nullptr, int32 p_len = 0);       /**< /!\ .... */
    void              send(ENetSocket *p_dst = nullptr);                          /**< /!\ ..E. */
  };

  /**
    @brief Buffer based ENetPacket.
    @details Buffer of datas preceded by its length.
  */
  class               ENetPacketRawDatas : public ENetPacket
  {
  public:
    ENetPacketRawDatas(ENetSocket *p_src = nullptr);                              /**< /!\ .... */
    ~ENetPacketRawDatas();                                                        /**< /!\ .... */
    void              recv();                                                     /**< /!\ B.E. */
    void              read(const char *p_datas = nullptr, int32 p_len = 0);       /**< /!\ ..E. */
    void              send(ENetSocket *p_dst = nullptr);                          /**< /!\ ..E. */
    int32             getLength() const;                                          /**< /!\ .... */
    const char        *getDatas() const;                                          /**< /!\ .... */
    void              setDatas(char *p_datas, int32 p_len);                       /**< /!\ .... */

  private:
    int32             m_len;    /**< Datas length. */
    char              *m_datas; /**< Datas. */
  };

}
