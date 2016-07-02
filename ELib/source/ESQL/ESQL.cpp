#include "ESQL/ESQL.h"
#include <iostream>
#include <mysql.h>

namespace     ELib
{
  ESQL::ESQL()
  {
    m_mysql = mysql_init(nullptr);
  }
  
  ESQL::~ESQL()
  {
    if (m_mysql)
      mysql_close(static_cast<MYSQL*>(m_mysql));
  }
  
  bool	      ESQL::connect(const char *hostname, const char *user, const char *password, const char *database, uint16 port)
  {
    return (mysql_real_connect(static_cast<MYSQL*>(m_mysql), hostname, user, password, database, port, NULL, 0) != NULL);
  }
  
  bool	      ESQL::query(const char *query, unsigned long length)
  {
    return (!mysql_real_query(static_cast<MYSQL*>(m_mysql), query, length ? length : static_cast<unsigned long>(strlen(query))));
  }
  
  ESQLResult  *ESQL::getResult(bool indexed)
  {
    MYSQL_RES *sqlres = NULL;
    
    if ((sqlres = mysql_use_result(static_cast<MYSQL*>(m_mysql))) == NULL)
      return (NULL);
    return (indexed ? new ESQLIndexedResult(sqlres) : new ESQLResult(sqlres));
  }
}