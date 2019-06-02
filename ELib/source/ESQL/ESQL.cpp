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
    @brief Instantiate a ESQL object for ELib SQL handling.
    @details Call mysql_init.
    @ethrow EERROR_SQL_MYSQL_EERROR if mysql_init fail.
  */
  ESQL::ESQL() :
    m_mysql(nullptr),
    m_isConnected(true)
  {
    mEERROR_R();
    m_mysql = mysql_init(nullptr);
    if (nullptr == m_mysql)
    {
      mETHROW_S(EERROR_SQL_MYSQL_ERROR);
    }
  }
  
  /**
    @brief Destroy a ESQL object.
    @details Call mysql_close.
  */
  ESQL::~ESQL()
  {
    if (nullptr != m_mysql)
    {
      mysql_close(reinterpret_cast<MYSQL*>(m_mysql));
    }
  }
  
  /**
    @brief Connect ESQL to a database.
    @details Database need to be running.
    @param p_hostname Internet host address in number-and-dots notation.
    @param p_port Internet host port.
    @param p_user Username for account to be used.
    @param p_password Password for account to be used.
    @param p_database Name of database to connect.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_MYSQL_ERROR if mysql_real_connect fail.
  */
  void          ESQL::connect(const std::string &p_hostname, uint16 port, const std::string &p_user,
                  const std::string &p_password, const std::string &p_database)
  {
    mEERROR_R();
    if (EERROR_NONE == mEERROR)
    {
      MYSQL     *l_mysql = nullptr;

      l_mysql = mysql_real_connect(reinterpret_cast<MYSQL*>(m_mysql), p_hostname.c_str(),
        p_user.c_str(), p_password.c_str(), p_database.c_str(), port, nullptr, 0);
      if (nullptr != m_mysql)
      {
        m_isConnected = true;
      }
      else
      {
        mEERROR_SA(EERROR_SQL_MYSQL_ERROR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
      }
    }
  }
  
  /**
    @brief Send a SQL query to ESQL database.
    @details ESQL need to be connected to a database.
    @param p_query SQL query to be send.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_NOT_CONNECTED if ESQL was not previously connected.
    @eerror EERROR_SQL_MYSQL_ERROR if mysql_real_query fail.
  */
  void          ESQL::query(const std::string &p_query)
  {
    mEERROR_R();
    if (false == m_isConnected)
    {
      mEERROR_S(EERROR_SQL_NOT_CONNECTED);
    }

    if (EERROR_NONE == mEERROR)
    {
      int32     l_ret = -1;

      l_ret = mysql_real_query(reinterpret_cast<MYSQL*>(m_mysql), p_query.c_str(), p_query.size());
      if (0 != l_ret)
      {
        mEERROR_SA(EERROR_SQL_MYSQL_ERROR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
      }
    }
  }

  /**
    @brief Retrieve results from last SQL query to ESQL database.
    @details ESQL need to be connected to a database.
    @param p_indexed Determine if ESQLResult need to be indexed.
    @return ESQLResult with parsed datas in success.
    @return nullptr in failure.
    @eerror EERROR_NONE in success.
    @eerror EERROR_SQL_NOT_CONNECTED if ESQL was not previously connected.
    @eerror EERROR_SQL_MYSQL_ERROR if mysql_use_result fail.
    @eerror EERROR_OOM if ESQLResult allocation fail.
    @eerror EERROR_SQL_RESULT_FAILURE if ESQLResult instantiation fail.
  */
  ESQLResult    *ESQL::getResult(bool p_indexed)
  {
    MYSQL_RES   *l_sqlRes = nullptr;
    ESQLResult  *l_result = nullptr;
    
    mEERROR_R();
    if (false == m_isConnected)
    {
      mEERROR_S(EERROR_SQL_NOT_CONNECTED);
    }

    if (EERROR_NONE == mEERROR)
    {
      l_sqlRes = mysql_use_result(reinterpret_cast<MYSQL*>(m_mysql));
      if (nullptr != l_sqlRes)
      {
        if (true == p_indexed)
        {
          l_result = new ESQLResult(m_mysql, l_sqlRes);
        }
        else
        {
          l_result = new ESQLResultIndexed(m_mysql, l_sqlRes);
        }
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SA(EERROR_SQL_RESULT_FAILURE, mEERROR_G.toString());
        }
        else
        {
          if (nullptr == l_result)
          {
            mEERROR_S(EERROR_OOM);
          }
        }
      }
      else
      {
        mEERROR_SA(EERROR_SQL_MYSQL_ERROR, mysql_error(reinterpret_cast<MYSQL*>(m_mysql)));
      }
    }

    return (l_result);
  }

}
