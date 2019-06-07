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
namespace                 ELib
{

  /**
    @brief Instantiate a ESQLResult from MYSQL_RES datas.
    @details Create and fill a ESQLResult with ESQLRow from datas parsed in MYSQL_RES parameter.
    @details Ignore rows that caused a EError, filling with successful one.
    @param p_mysql Handle for mysql connection.
    @param p_sqlRes MYSQL_RES datas from a request.
    @eerror EERROR_NULL_PTR if p_sqlRes is null.
    @eerror EERROR_OOM if field or row allocation fail.
  */
  ESQLResult::ESQLResult(void *p_mysql, void *p_sqlRes) :
    m_mysql(p_mysql),
    m_sqlRes(p_sqlRes),
    m_rows()
  {
    mEERROR_R();
    if (nullptr == p_sqlRes)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      MYSQL_RES           *l_sqlRes = nullptr;
      MYSQL_ROW           l_sqlRow = nullptr;
      unsigned int        l_nbFields = -1;

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
                char          *l_datas = nullptr;

                l_datas = new char[l_lengths[l_field] + 1]();
                if (nullptr != l_datas)
                {
                  memcpy(l_datas, l_sqlRow[l_field], l_lengths[l_field]);
                  l_row->push(l_datas);
                }
                else
                {
                  l_field = l_nbFields;
                  mEERROR_S(EERROR_OOM);
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
              mEERROR_SA(EERROR_SQL_MYSQL_ERROR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
            }
          }
          else
          {
            l_sqlRow = nullptr;
            mEERROR_S(EERROR_OOM);
          }
        }
        else
        {
          if (0 != mysql_errno(reinterpret_cast<MYSQL*>(m_mysql)))
          {
            mEERROR_SA(EERROR_SQL_MYSQL_ERROR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
          }
        }
      } while (nullptr != l_sqlRow);
    }
  }

  /**
    @brief Destroy a ESQLResult, deleting its ESQLRows.
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
    @brief Retrieve a ESQLRow from an ESQLResult.
    @param p_row Row in ESQLRow list to be retrieve.
    @return ESQLRow at p_row in success.
    @return nullptr if an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_OUT_OR_RANGE is p_row is bigger than m_rows size.
  */
  ESQLRow                 *ESQLResult::at(uint32 p_row)
  {
    ESQLRow               *l_row = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_rows.size()) <= p_row)
    {
      mEERROR_S(EERROR_SQL_OUT_OF_RANGE);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_row = m_rows[p_row];
    }

    return (l_row);
  }

  /**
    @brief Retrieve a ESQLRow from an ESQLResult using at().
    @param p_row Row in ESQLRow list to be retrieve.
    @return ESQLRow at p_row in success.
    @return nullptr if an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_OUT_OR_RANGE is p_row is bigger than m_rows size.
  */
  ESQLRow                 *ESQLResult::operator[](uint32 p_row)
  {
    return (at(p_row));
  }
  
  /**
    @brief Retrieve a ESQLField from an ESQLResult.
    @param p_row Row in ESQLRow list to be retrieve.
    @param p_field Field of ESQLField in ESQLRow list to be retrieve.
    @return ESQLField at p_field in p_row in success.
    @return nullptr if an EError occured.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_OUT_OR_RANGE is p_row is bigger than m_rows size or p_field is bigger than m_rows[p_row] size.
    @eerror EERROR_NULL_PTR if m_rows[p_row] is null.
  */
  ESQLField               *ESQLResult::at(uint32 p_row, uint32 p_field)
  {
    ESQLField             *l_field = nullptr;

    mEERROR_R();
    if (static_cast<uint32>(m_rows.size()) <= p_row)
    {
      mEERROR_S(EERROR_SQL_OUT_OF_RANGE);
    }
    if (nullptr == m_rows[p_row])
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_field = (*m_rows[p_row])[p_field];
      if (EERROR_NONE != mEERROR)
      {
        mEERROR_SA(EERROR_SQL_OUT_OF_RANGE, mEERROR_G.toString());
      }
    }

    return (l_field);
  }

  /**
    @brief Retrieve the number of ESQLRow in ESQLResult.
    @return Number of ESQLRow in ESQLResult.
  */
  uint32                  ESQLResult::getSize() const
  {
    return (static_cast<uint32>(m_rows.size()));
  }

  /**
    @brief Print ESQLResult using EPrinter.
  */
  void                      ESQLResult::print() const
  {
    std::string         l_print = "";
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
    @brief Instantiate a ESQLResultIndexed from MYSQL_RES datas.
    @details Inherit from ESQLResult.
    @details Retrieve and contain ESQLFields informations.
    @param p_mysql Handle for mysql connection.
    @param p_sqlRes MYSQL_RES datas from a request.
    @eerror EERROR_NULL_PTR if p_sqlRes is null.
    @eerror EERROR_OOM if field or row allocation fail.
  */
  ESQLResultIndexed::ESQLResultIndexed(void *p_mysql, void *p_sqlRes) :
    ESQLResult(p_mysql, p_sqlRes),
    m_fieldInfos()
  {
    mEERROR_R();
    if (nullptr == p_sqlRes)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      MYSQL_RES           *l_sqlRes = nullptr;
      MYSQL_FIELD         *l_field = nullptr;

      l_sqlRes = reinterpret_cast<MYSQL_RES*>(p_sqlRes);
      do
      {
        l_field = mysql_fetch_field(reinterpret_cast<MYSQL_RES*>(p_sqlRes));
        if (nullptr != l_field)
        {
          ESQLFieldInfo     *l_fieldInfo = nullptr;

          l_fieldInfo = new ESQLFieldInfo();
          if (nullptr != l_fieldInfo)
          {
            l_fieldInfo->m_name = l_field->name;
            l_fieldInfo->m_type = l_field->type;
            l_fieldInfo->m_sign = !(l_field->flags & UNSIGNED_FLAG);
            m_fieldInfos.push_back(l_fieldInfo);
          }
          else
          {
            l_field = nullptr;
            mEERROR_S(EERROR_OOM);
          }
        }
      } while (nullptr != l_field);
    }
  }
  
  /**
    @brief Destroy a ESQLResultIndexed, deleting its ESQLFieldInfo.
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
    @brief Retrieve a ESQLField from an ESQLResultIndexed.
    @param p_row Row in ESQLRow list to be retrieve.
    @param p_fieldName Name of ESQLField in ESQLRow list to be retrieve.
    @return ESQLField named p_fieldName in p_row in success.
    @return nullptr if an EError occured or if p_fieldName is not found.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_OUT_OR_RANGE is p_row is bigger than m_rows size or l_field is bigger than m_rows[p_row] size.
    @eerror EERROR_NULL_PTR if m_rows[p_row] is null.
  */
  ESQLField               *ESQLResultIndexed::at(uint32 p_row, const std::string &p_fieldName)
  {
    ESQLField             *l_ret = nullptr;

    for (uint32 l_field = 0; l_field < m_fieldInfos.size(); ++l_field)
    {
      if (p_fieldName == m_fieldInfos[l_field]->m_name)
      {
        l_ret = ESQLResult::at(p_row, l_field);
      }
    }

    return (l_ret);
  }

  /**
    @brief Convert a ESQLRow to an buffer.
    @details Can be used on object padded properly (pragma pack 1).
    @eerror EERROR_NONE in success.
    @eerror EERROR_NULL_PTR if p_buffer is null.
    @eerror EERROR_SQL_OUT_OF_RANGE if ESQLRow at p_row couldn't be retrieve or if ESQLFieldInfos is not filled.
  */
  void                    ESQLResultIndexed::rowAsBuffer(uint32 p_row, char *p_buffer)
  {
    mEERROR_R();
    if (nullptr == p_buffer)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if (m_rows.size() <= p_row)
    {
      mEERROR_S(EERROR_SQL_OUT_OF_RANGE);
    }

    if (EERROR_NONE == mEERROR)
    {
      for (uint32 l_field = 0; l_field < m_fieldInfos.size(); ++l_field)
      {
        ESQLField           *l_datas = nullptr;
        size_t              l_len = 0;

        l_datas = ESQLResult::at(p_row, l_field);
        if (nullptr != m_fieldInfos[l_field])
        {
          switch (m_fieldInfos[l_field]->m_type)
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
            l_len = strlen(*l_datas);
          }
          break;
          default:
            break;
          }
          if (nullptr != l_datas)
          {
            memcpy(p_buffer, *l_datas, l_len);
            p_buffer += l_len;
          }
        }
        else
        {
          mEERROR_SA(EERROR_SQL_OUT_OF_RANGE, mEERROR_G.toString());
        }
      }
    }
  }

}
