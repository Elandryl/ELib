/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ESQLResult.
*/

#include <iomanip>
#include <mysql.h>
#include "ESQL/ESQLResult.h"

/**
  @brief General scope for ELib components.
*/
namespace                   ELib
{

  /**
    @brief Constructor for ESQLResult. /!\ EError.
    @details Generate ESQLResult from MYSQL_RES datas.
    @details Ignore ESQLRow failures.
  */
  ESQLResult::ESQLResult(void *p_mysql, void *p_sqlRes) :
    m_mysql(p_mysql),
    m_sqlRes(p_sqlRes),
    m_rows()
  {
    mEERROR_R();
    if (nullptr == p_mysql)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if (nullptr == p_sqlRes)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      MYSQL                 *l_mysql = nullptr;
      MYSQL_RES             *l_sqlRes = nullptr;
      unsigned int          l_nbFields = -1;
      MYSQL_ROW             l_sqlRow = nullptr;

      l_mysql = reinterpret_cast<MYSQL*>(m_mysql);
      l_sqlRes = reinterpret_cast<MYSQL_RES*>(m_sqlRes);
      l_nbFields = mysql_num_fields(l_sqlRes);
      do
      {
        l_sqlRow = mysql_fetch_row(l_sqlRes);
        if (nullptr != l_sqlRow)
        {
          ESQLRow           *l_row = nullptr;

          l_row = new ESQLRow();
          if (nullptr != l_row)
          {
            unsigned long   *l_lengths = nullptr;

            l_lengths = mysql_fetch_lengths(l_sqlRes);
            if (nullptr != l_lengths)
            {
              for (unsigned long l_field = 0; l_field < l_nbFields; ++l_field)
              {
                char        *l_datas = nullptr;

                l_datas = new char[l_lengths[l_field] + 1]();
                if (nullptr != l_datas)
                {
                  memcpy(l_datas, l_sqlRow[l_field], l_lengths[l_field]);
                  l_row->push(l_datas);
                  if (EERROR_NONE != mEERROR)
                  {
                    mEERROR_SH(EERROR_SQL_ROW_ERR);
                  }
                }
                else
                {
                  mEERROR_S(EERROR_MEMORY);
                }
              }
              if (EERROR_NONE == mEERROR)
              {
                m_rows.push_back(l_row);
              }
              else
              {
                delete (l_row);
              }
            }
            else
            {
              mEERROR_SA(EERROR_SQL_MYSQL_ERR, mysql_error(l_mysql));
            }
          }
          else
          {
            mEERROR_S(EERROR_MEMORY);
          }
        }
        else
        {
          if (0 != mysql_errno(l_mysql))
          {
            mEERROR_SA(EERROR_SQL_MYSQL_ERR, mysql_error(l_mysql));
          }
        }
      } while (nullptr != l_sqlRow);
    }
  }

  /**
    @brief Destructor for ESQLResult
    @details Delete its ESQLRows and call mysql_free_result().
  */
  ESQLResult::~ESQLResult()
  {
    while (false == m_rows.empty())
    {
      delete (m_rows.front());
      m_rows.erase(m_rows.begin());
    }
    mysql_free_result(static_cast<MYSQL_RES*>(m_sqlRes));
  }

  /**
    @brief Get ESQLRow from ESQLResult. /!\ EError.
    @param p_field Field of ESQLRow to be retrieve.
    @return ESQLRow on success.
    @return nullptr on failure.
  */
  ESQLRow                   *ESQLResult::at(uint32 p_row)
  {
    ESQLRow                 *l_row = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_rows.size()) <= p_row)
    {
      mEERROR_S(EERROR_OUT_OF_RANGE);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_row = m_rows[p_row];
    }

    return (l_row);
  }

  /**
    @brief Get ESQLRow from ESQLResult. /!\ EError.
    @details Call ESQLResult::at().
    @param p_row Row of ESQLRow to be retrieve.
    @return ESQLRow on success.
    @return nullptr on failure.
  */
  ESQLRow                   *ESQLResult::operator[](uint32 p_row)
  {
    ESQLRow                 *l_row = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_rows.size()) < p_row)
    {
      mEERROR_S(EERROR_OUT_OF_RANGE);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_row = at(p_row);
      if (EERROR_NONE != mEERROR)
      {
        mEERROR_SH(EERROR_SQL_RESULT_ERR);
      }
    }

    return (l_row);
  }
  
  /**
    @brief Get ESQLField from ESQLResult. /!\ EError.
    @param p_row Row of ESQLField to be retrieve.
    @param p_field Field of ESQLField to be retrieve.
    @return ESQLField on success.
    @return nullptr on failure.
  */
  ESQLField                 *ESQLResult::at(uint32 p_row, uint32 p_field)
  {
    ESQLField               *l_field = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_rows.size()) <= p_row)
    {
      mEERROR_S(EERROR_OUT_OF_RANGE);
    }
    if (nullptr == m_rows[p_row])
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_field = (*m_rows[p_row])[p_field];
      if (EERROR_NONE != mEERROR)
      {
        mEERROR_SH(EERROR_SQL_ROW_ERR);
      }
    }

    return (l_field);
  }

  /**
    @brief Get number of rows.
    @return Number of rows.
  */
  uint32                    ESQLResult::getSize() const
  {
    return (static_cast<uint32>(m_rows.size()));
  }

  /**
    @brief Print ESQLResult using EPrinter.
  */
  void                      ESQLResult::print() const
  {
    std::string             l_print = "";

    for (uint32 l_row = 0; l_row < getSize(); ++l_row)
    {
      if (nullptr != m_rows[l_row])
      {
        l_print += "Row " + std::to_string(l_row) + " : ";
        for (uint32 l_field = 0; l_field < m_rows[l_row]->getSize(); ++l_field)
        {
          l_print += static_cast<std::string>(*m_rows[l_row]->at(l_field)) + " - ";
        }
      }
      l_print += "\n";
    }
    mEPRINT_STD(l_print);
  }
  
  /**
    @brief Constructor for ESQLResultIndexed. /!\ EError.
    @details Generate ESQLResultIndexed from MYSQL_RES datas.
    @details No EError reset as done in ESQLResult::ESQLResult() first.
    @details Invalid on failure.
  */
  ESQLResultIndexed::ESQLResultIndexed(void *p_mysql, void *p_sqlRes) :
    ESQLResult(p_mysql, p_sqlRes),
    m_fieldInfos(),
    m_isValid(false)
  {
    if (EERROR_NONE != mEERROR)
    {
      mEERROR_SH(EERROR_SQL_RESULT_ERR);
    }
    if (nullptr == p_mysql)
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }
    if (nullptr == p_sqlRes)
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      MYSQL_FIELD           *l_field = nullptr;

      do
      {
        l_field = mysql_fetch_field(reinterpret_cast<MYSQL_RES*>(p_sqlRes));
        if (nullptr != l_field)
        {
          ESQLFieldInfo     *l_fieldInfo = nullptr;

          l_fieldInfo = new ESQLFieldInfo();
          if (nullptr != l_fieldInfo)
          {
            if (EERROR_NONE == mEERROR)
            {
              l_fieldInfo->m_name = l_field->name;
              l_fieldInfo->m_type = l_field->type;
              l_fieldInfo->m_sign = !(l_field->flags & UNSIGNED_FLAG);
              m_fieldInfos.push_back(l_fieldInfo);
            }
            else
            {
              mEERROR_SH(EERROR_SQL_FIELD_ERR);
              delete (l_fieldInfo);
            }
          }
          else
          {
            mEERROR_S(EERROR_MEMORY);
          }
        }
        if (EERROR_NONE != mEERROR)
        {
          l_field = nullptr;
        }
      } while (nullptr != l_field);
      if (EERROR_NONE == mEERROR)
      {
        m_isValid = true;
      }
    }
  }
  
  /**
    @brief Destructor for ESQLResultIndexed.
    @details Delete its ESQLFieldInfo.
  */
  ESQLResultIndexed::~ESQLResultIndexed()
  {
    while (false == m_fieldInfos.empty())
    {
      delete (m_fieldInfos.front());
      m_fieldInfos.erase(m_fieldInfos.begin());
    }
  }

  /**
    @brief Get ESQLField from ESQLResultIndexed. /!\ EError.
    @param p_row Row of ESQLField to be retrieve.
    @param p_fieldName FieldName of ESQLField to be retrieve.
    @return ESQLField on success.
    @return nullptr on failure.
  */
  ESQLField                 *ESQLResultIndexed::at(uint32 p_row, const std::string &p_fieldName)
  {
    ESQLField               *l_ret = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_rows.size()) <= p_row)
    {
      mEERROR_S(EERROR_OUT_OF_RANGE);
    }
    if (nullptr == m_rows[p_row])
    {
      mEERROR_SH(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      for (uint32 l_field = 0; l_field < m_fieldInfos.size(); ++l_field)
      {
        if (p_fieldName == m_fieldInfos[l_field]->m_name)
        {
          l_ret = ESQLResult::at(p_row, l_field);
          if (EERROR_NONE != mEERROR)
          {
            mEERROR_SH(EERROR_SQL_RESULT_ERR);
          }
        }
      }
    }

    return (l_ret);
  }

  /**
    @brief Get ESQLRow as buffer. /!\ EError.
    @details Can be used on object with proper padding (see pragma pack 1).
    @details ESQLResultIndexed need to be valid.
  */
  void                      ESQLResultIndexed::rowAsBuffer(uint32 p_row, char *p_buffer)
  {
    mEERROR_R();
    if (false == m_isValid)
    {
      mEERROR_S(EERROR_SQL_STATE);
    }
    if (nullptr == p_buffer)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      ESQLRow               *l_row = nullptr;

      l_row = ESQLResult::at(p_row);
      if (nullptr != l_row)
      {
        for (uint32 l_pos = 0; l_pos < m_fieldInfos.size(); ++l_pos)
        {
          if (nullptr != m_fieldInfos[l_pos])
          {
            ESQLField       *l_field = nullptr;

            l_field = l_row->at(l_pos);
            if (nullptr != l_field)
            {
              char          *l_datas = nullptr;
              size_t        l_len = 0;

              l_datas = static_cast<char*>(*l_field);
              if (nullptr != l_datas)
              {
                switch (m_fieldInfos[l_pos]->m_type)
                {
                case MYSQL_TYPE_TINY:
                {
                  l_len = sizeof(int8);
                }
                break;
                case MYSQL_TYPE_SHORT:
                {
                  l_len = sizeof(int16);
                }
                break;
                case MYSQL_TYPE_LONG:
                {
                  l_len = sizeof(int32);
                }
                break;
                case MYSQL_TYPE_LONGLONG:
                {
                  l_len = sizeof(int64);
                }
                break;
                case MYSQL_TYPE_FLOAT:
                {
                  l_len = sizeof(float);
                }
                break;
                case MYSQL_TYPE_DOUBLE:
                {
                  l_len = sizeof(double);
                }
                break;
                case MYSQL_TYPE_VAR_STRING:
                {
                  l_len = strlen(*l_field);
                }
                break;
                default:
                  break;
                }
                memcpy(p_buffer, l_datas, l_len);
                delete (l_datas);
                p_buffer += l_len;
              }
              else
              {
                mEERROR_SH(EERROR_SQL_FIELD_ERR);
              }
            }
            else
            {
              mEERROR_SH(EERROR_SQL_ROW_ERR);
            }
          }
          else
          {
            mEERROR_S(EERROR_NULL_PTR);
          }
        }
      }
      else
      {
        mEERROR_SH(EERROR_SQL_RESULT_ERR);
      }
    }
  }

}
