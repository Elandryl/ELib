/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ESQLRow.
*/

#include "ESQL/ESQLRow.h"

/**
  @brief General scope for ELib components.
*/
namespace       ELib
{

  /**
    @brief Constructof for ESQLRow.
  */
  ESQLRow::ESQLRow() :
    m_fields()
  {
  }

  /**
    @brief Destructor for ESQLRow.
    @details Delete its ESQLFields.
  */
  ESQLRow::~ESQLRow()
  {
    while (false == m_fields.empty())
    {
      delete (m_fields.front());
      m_fields.erase(m_fields.begin());
    }
  }
  
  /**
    @brief Add ESQLField to ESQLRow. /!\ EError.
    @param p_field ESQLField to be add.
  */
  void          ESQLRow::push(ESQLField *p_field)
  {
    mEERROR_R();
    if (nullptr == p_field)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_fields.push_back(p_field);
    }
  }

  /**
    @brief Add datas to ESQLRow. /!\ EError.
    @details Create a ESQLField from datas, then call ESQLRow::push().
    @param p_datas Datas to be add.
  */
  void          ESQLRow::push(char *p_datas)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      ESQLField   *l_field = nullptr;

      l_field = new ESQLField(p_datas);
      if (nullptr != l_field)
      {
        if (EERROR_NONE == mEERROR)
        {
          push(l_field);
          if (EERROR_NONE != mEERROR)
          {
            mEERROR_SH(EERROR_SQL_ROW_ERR);
          }
        }
        else
        {
          mEERROR_SH(EERROR_SQL_FIELD_ERR);
        }
        if (EERROR_NONE != mEERROR)
        {
          delete (l_field);
        }
      }
      else
      {
        mEERROR_S(EERROR_MEMORY);
      }
    }
  }

  /**
    @brief Get ESQLField from ESQLRow. /!\ EError.
    @param p_field Field of ESQLField to be retrieve.
    @return ESQLField on success.
    @return nullptr on failure.
  */
  ESQLField     *ESQLRow::at(uint32 p_field)
  {
    ESQLField   *l_field = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_fields.size()) < p_field)
    {
      mEERROR_S(EERROR_OUT_OF_RANGE);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_field = m_fields[p_field];
    }

    return (l_field);
  }
  
  /**
    @brief Get ESQLField from ESQLRow. /!\ EError.
    @details Call ESQLRow::at().
    @param p_field Field of ESQLField to be retrieve.
    @return ESQLField on success.
    @return nullptr on failure.
  */
  ESQLField     *ESQLRow::operator[](uint32 p_field)
  {
    ESQLField   *l_field = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_fields.size()) < p_field)
    {
      mEERROR_S(EERROR_OUT_OF_RANGE);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_field = at(p_field);
      if (EERROR_NONE != mEERROR)
      {
        mEERROR_SH(EERROR_SQL_ROW_ERR);
      }
    }

    return (l_field);
  }

  /**
    @brief Get number of fields.
    @return Number of fields.
  */
  uint32        ESQLRow::getSize() const
  {
    return (static_cast<uint32>(m_fields.size()));
  }
  
  /**
    @brief Get ESQLRow content as buffers array.
    @details Returns a copy of ESQLRow content. User should delete it after usage.
    @return Buffers array on success.
    @return nullptr on failure.
  */
  char          **ESQLRow::toCharArray()
  {
    char        **l_datasArray = nullptr;

    mEERROR_R();
    if (0 < m_fields.size())
    {
      l_datasArray = new char*[m_fields.size() + 1]();
      if (nullptr != l_datasArray)
      {
        for (uint32 l_field = 0; l_field < m_fields.size(); ++l_field)
        {
          l_datasArray[l_field] = static_cast<char*>(*m_fields[l_field]);
          if (EERROR_NONE != mEERROR)
          {
            mEERROR_SH(EERROR_SQL_FIELD_ERR);
          }
        }
      }
      else
      {
        mEERROR_S(EERROR_MEMORY);
      }
    }

    return (l_datasArray);
  }

}
