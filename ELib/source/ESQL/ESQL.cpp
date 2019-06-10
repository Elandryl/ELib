/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ESQL, ELib SQL handling object.
*/

#include <mysql.h>
#include "ESQL/ESQL.h"

/**
  @brief General scope for ELib components.
*/
namespace       ELib
{

  /**
    @brief Constructor for ESQL.
  */
  ESQL::ESQL() :
    m_mysql(nullptr),
    m_isConnected(false)
  {
  }
  
  /**
    @brief Destructor for ESQL.
    @details Call ESQL::close().
  */
  ESQL::~ESQL()
  {
    if (true == m_isConnected)
    {
      close();
    }
  }

  /**
    @brief Singleton for ESQL. /!\ EError.
    @return ESQL unique instance on success.
    @return nullptr on failure.
  */
  ESQL            *ESQL::getInstance()
  {
    static ESQL   *l_instance = nullptr;

    mEERROR_R();
    if (nullptr == l_instance)
    {
      l_instance = new ESQL();
      if (nullptr == l_instance)
      {
        mEERROR_S(EERROR_MEMORY);
      }
    }

    return (l_instance);
  }
  
  /**
    @brief Connect ESQL to a database. /!\ EError.
    @details Database need to be running.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Internet host port.
    @param p_user Username for account to be used.
    @param p_password Password for account to be used.
    @param p_database Name of database to connect.
  */
  void            ESQL::connect(const std::string &p_hostname, uint16 port, const std::string &p_user,
                    const std::string &p_password, const std::string &p_database)
  {
    mEERROR_R();
    m_mysql = mysql_init(nullptr);
    if (nullptr != m_mysql)
    {
      MYSQL       *l_mysql = nullptr;

      l_mysql = mysql_real_connect(reinterpret_cast<MYSQL*>(m_mysql), p_hostname.c_str(),
        p_user.c_str(), p_password.c_str(), p_database.c_str(), port, nullptr, 0);
      if (nullptr != l_mysql)
      {
        m_isConnected = true;
      }
      else
      {
        mEERROR_SA(EERROR_SQL_MYSQL_ERR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
      }
    }
    else
    {
      mEERROR_S(EERROR_MEMORY);
    }
  }
  
  /**
    @brief Execute SQL query. /!\ EError.
    @details ESQL need to be connected to a database.
    @param p_query SQL query to be executed.
  */
  void            ESQL::query(const std::string &p_query)
  {
    mEERROR_R();
    if (false == m_isConnected)
    {
      mEERROR_S(EERROR_SQL_STATE);
    }
    if (nullptr == m_mysql)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      int32       l_ret = -1;

      l_ret = mysql_real_query(reinterpret_cast<MYSQL*>(m_mysql), p_query.c_str(), static_cast<unsigned long>(p_query.size()));
      if (0 != l_ret)
      {
        mEERROR_SA(EERROR_SQL_MYSQL_ERR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
      }
    }
  }

  /**
    @brief Get ESQLResult from last ESQL::query(). /!\ EError.
    @details ESQL need to be connected to a database.
    @details EERROR_SQL_RESULT_ERR is non-fatal (see related constructor).
    @param p_indexed Determine if ESQLResultIndexed is requested.
    @return ESQLResult or ESQLResultIndexed on success or non-fatal failure.
    @return nullptr in fatal failure.
  */
  ESQLResult      *ESQL::getResult(bool p_indexed)
  {
    ESQLResult    *l_result = nullptr;

    mEERROR_R();
    if (false == m_isConnected)
    {
      mEERROR_S(EERROR_SQL_STATE);
    }
    if (nullptr == m_mysql)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      MYSQL_RES   *l_sqlRes = nullptr;

      l_sqlRes = mysql_use_result(reinterpret_cast<MYSQL*>(m_mysql));
      if (nullptr != l_sqlRes)
      {
        if (true == p_indexed)
        {
          l_result = new ESQLResultIndexed(m_mysql, l_sqlRes);
        }
        else
        {
          l_result = new ESQLResult(m_mysql, l_sqlRes);
        }
        if (nullptr != l_result)
        {
          if (EERROR_NONE != mEERROR)
          {
            mEERROR_SH(EERROR_SQL_RESULT_ERR);
          }
        }
        else
        {
          mEERROR_S(EERROR_MEMORY);
        }
      }
      else
      {
        mEERROR_SA(EERROR_SQL_MYSQL_ERR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
      }
    }

    return (l_result);
  }

  /**
    @brief Close ESQL connection.
  */
  void            ESQL::close()
  {
    if (nullptr != m_mysql)
    {
      mysql_close(reinterpret_cast<MYSQL*>(m_mysql));
      m_mysql = nullptr;
      m_isConnected = false;
    }
  }

}
