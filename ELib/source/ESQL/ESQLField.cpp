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
    @brief Instantiate ESQLField, container for a SQL field.
    @param p_datas Content for ESQLField to be stored.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::ESQLField(char *p_datas) :
    m_datas(p_datas)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }
  }

  /**
    @brief Destroy a ESQLField, deleting its datas.
  */
  ESQLField::~ESQLField()
  {
    if (nullptr != m_datas)
    {
      delete (m_datas);
    }
  }
  
  /**
    @brief Convert ESQLField to uint64 type.
    @return ESQLField content as uint64 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   uint64() const
  {
    uint64              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint64>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to uint32 type.
    @return ESQLField content as uint32 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   uint32() const
  {
    uint32              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint32>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to uint16 type.
    @return ESQLField content as uint16 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   uint16() const
  {
    uint16              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint16>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to uint8 type.
    @return ESQLField content as uint8 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   uint8() const
  {
    uint8               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<uint8>(strtoul(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to int64 type.
    @return ESQLField content as int64 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   int64() const
  {
    int64               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int64>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to int32 type.
    @return ESQLField content as int32 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   int32() const
  {
    int32               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int32>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to int16 type.
    @return ESQLField content as int16 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   int16() const
  {
    int16               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int16>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to int8 type.
    @return ESQLField content as int8 in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   int8() const
  {
    int8                l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = static_cast<int8>(strtol(m_datas, nullptr, 10));
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to char* type.
    @details A copy of the content is created and need to be deleted by user.
    @return ESQLField content as char* in success.
    @return nullptr if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
    @eerror EERROR_OOM if l_datas allocation fail.
  */
  ESQLField::operator   char*() const
  {
    char                *l_datas = nullptr;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
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
        mEERROR_S(EERROR_OOM);
      }
    }

    return (l_datas);
  }
  
  /**
    @brief Convert ESQLField to string type.
    @return ESQLField content as string in success.
    @return Empty string if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   const std::string() const
  {
    std::string         l_ret = "";

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = std::string(m_datas);
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to float type.
    @return ESQLField content as float in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   float() const
  {
    float               l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = strtof(m_datas, nullptr);
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to double type.
    @return ESQLField content as double in success.
    @return 0 if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   double() const
  {
    double              l_ret = 0;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_ret = strtod(m_datas, nullptr);
    }

    return (l_ret);
  }
  
  /**
    @brief Convert ESQLField to bool type.
    @return ESQLField content as bool in success.
    @return false if an EError occured.
    @eerror EERROR_SUCCESS in success.
    @eerror EERROR_SQL_FIELD_INVALID if p_datas is null.
  */
  ESQLField::operator   bool() const
  {
    bool                l_ret = false;

    mEERROR_R();
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
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

}
