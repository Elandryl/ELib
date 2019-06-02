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
    @brief Instantiate ESQLRow, container for a SQL row results.
  */
  ESQLRow::ESQLRow() :
    m_fields()
  {
  }

  /**
    @brief Destroy a ESQLRow, deleting its ESQLFields.
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
    @brief Add datas to ESQLRow.
    @details An ESQLField is created and added at the back of ESQLRow.
    @param p_datas Content for ESQLField to be add.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_FIELD_INVALID is p_datas is null or if ESQLField creation issued an error.
    @eerror EERROR_OOM if ESQLField allocation fail.
  */
  void          ESQLRow::push(char *p_datas)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      ESQLField   *l_field = nullptr;

      l_field = new ESQLField(p_datas);
      if (nullptr != l_field)
      {
        if (EERROR_NONE == mEERROR)
        {
          m_fields.push_back(l_field);
        }
        else
        {
          mEERROR_SA(EERROR_SQL_FIELD_INVALID, mEERROR_G.toString());
        }
      }
      else
      {
        mEERROR_S(EERROR_OOM);
      }
    }
  }
  
  /**
    @brief Add ESQLField to ESQLRow.
    @param p_field ESQLField to be add.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_FIELD_INVALID is p_field is null.
  */
  void          ESQLRow::push(ESQLField *p_field)
  {
    mEERROR_R();
    if (nullptr == p_field)
    {
      mEERROR_S(EERROR_SQL_FIELD_INVALID);
    }

    if (EERROR_NONE == mEERROR)
    {
      m_fields.push_back(p_field);
    }
  }

  /**
    @brief Retrieve a ESQLField from an ESQLRow.
    @param p_field Field of ESQLField to be retrieve.
    @return ESQLField at p_field in success.
    @return nullptr if an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_OUT_OR_RANGE is p_field is bigger than m_fields size.
  */
  ESQLField     *ESQLRow::at(uint32 p_field)
  {
    ESQLField   *l_field = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_fields.size()) < p_field)
    {
      mEERROR_S(EERROR_SQL_OUT_OF_RANGE);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_field = m_fields[p_field];
    }

    return (l_field);
  }
  
  /**
    @brief Retrieve a ESQLField from an ESQLRow using at().
    @param p_field Field of ESQLField to be retrieve.
    @return ESQLField at p_field in success.
    @return nullptr if an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_OUT_OR_RANGE is p_field is bigger than m_fields size.
  */
  ESQLField     *ESQLRow::operator[](uint32 p_field)
  {
    return (at(p_field));
  }
  
  /**
    @brief Retrieve every ESQLFields as an array of buffers.
    @return Array of ESQLFields as buffers in success.
    @return nullptr if an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_OOM if l_datasArray allocation fail.
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
        }
      }
      else
      {
        mEERROR_S(EERROR_OOM);
      }
    }

    return (l_datasArray);
  }

}
