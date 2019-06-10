/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ESQLField.
*/

#include <iomanip>
#include "ESQL/ESQLField.h"

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief Constructor for ESQLField. /!\ EError.
    @param p_datas Datas of ESQLField.
  */
  ESQLField::ESQLField(char *p_datas) :
    m_datas(p_datas)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
  }

  /**
    @brief Destructor ESQLField.
    @details Delete its datas.
  */
  ESQLField::~ESQLField()
  {
    if (nullptr != m_datas)
    {
      delete (m_datas);
    }
  }
  
  /**
    @brief Get ESQLField as uint64. /!\ EError.
    @return uint64 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   uint64() const
  {
    uint64              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint64>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as uint32. /!\ EError.
    @return uint32 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   uint32() const
  {
    uint32              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint32>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as uint16. /!\ EError.
    @return uint16 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   uint16() const
  {
    uint16              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint16>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as uint8. /!\ EError.
    @return uint8 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   uint8() const
  {
    uint8               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint8>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as int64. /!\ EError.
    @return int64 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   int64() const
  {
    int64               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int64>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as int32. /!\ EError.
    @return int32 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   int32() const
  {
    int32               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int32>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as int16. /!\ EError.
    @return int16 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   int16() const
  {
    int16               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int16>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as int8. /!\ EError.
    @return int8 in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator   int8() const
  {
    int8                l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int8>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }

  /**
    @brief Get ESQLField as float. /!\ EError.
    @return float in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator float() const
  {
    float               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = strtof(m_datas, nullptr);
    }

    return (l_ret);
  }

  /**
    @brief Get ESQLField as double. /!\ EError.
    @return double in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator double() const
  {
    double              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = strtod(m_datas, nullptr);
    }

    return (l_ret);
  }

  /**
    @brief Get ESQLField as bool. /!\ EError.
    @return bool in success.
    @return 0 if an EError occured.
  */
  ESQLField::operator bool() const
  {
    bool                l_ret = false;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (0 != strtol(m_datas, nullptr, 10))
      {
        l_ret = true;
      }
    }

    return (l_ret);
  }
  
  /**
    @brief Get ESQLField as char*. /!\ EError.
    @details Returns a copy of ESQLField datas. User should delete it after usage.
    @return char* in success.
    @return nullptr if an EError occured.
  */
  ESQLField::operator   char*() const
  {
    char                *l_datas = nullptr;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      size_t            l_len = 0;

      l_len = strlen(m_datas);
      l_datas = new char[l_len + 1]();
      if (nullptr != l_datas)
      {
        memcpy(l_datas, m_datas, strlen(m_datas));
      }
      else
      {
        mEERROR_S(EERROR_MEMORY);
      }
    }

    return (l_datas);
  }
  
  /**
    @brief Get ESQLField as const std::string. /!\ EError.
    @return const std::string in success.
    @return Empty std::string if an EError occured.
  */
  ESQLField::operator   const std::string() const
  {
    std::string         l_ret = "";

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = std::string(m_datas);
    }

    return (l_ret);
  }

}
