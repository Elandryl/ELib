//#include "ESQL/ESQL.h"
//#include <iostream>
//#include <mysql.h>
//
//namespace       ELib
//{
//  ESQL::ESQL()
//  {
//    if ((m_mysql = mysql_init(nullptr)) == nullptr)
//      EThrow(EERROR_MYSQL_INIT);
//  }
//  
//  ESQL::~ESQL()
//  {
//    if (m_mysql)
//      mysql_close(static_cast<MYSQL*>(m_mysql));
//  }
//  
//  EErrorCode    ESQL::connect(const char *hostname, const char *user, const char *password, const char *database, uint16 port)
//  {
//    if (mysql_real_connect(static_cast<MYSQL*>(m_mysql), hostname, user, password, database, port, nullptr, 0) == nullptr)
//    {
//      std::cout << mysql_error(reinterpret_cast<MYSQL*>(m_mysql)) << std::endl;
//      EReturn(EERROR_SQL_CONNECT);
//    }
//    return (EERROR_NONE);
//  }
//  
//  EErrorCode    ESQL::query(const char *query, unsigned long length)
//  {
//    if (mysql_real_query(static_cast<MYSQL*>(m_mysql), query, length ? length : static_cast<unsigned long>(strlen(query))))
//      EReturn(EERROR_SQL_QUERY);
//    return (EERROR_NONE);
//  }
//  
//  ESQLResult    *ESQL::getResult(bool indexed)
//  {
//    MYSQL_RES   *sqlres;
//    
//    if ((sqlres = mysql_use_result(static_cast<MYSQL*>(m_mysql))) == nullptr)
//      EReturnValue(EERROR_SQL_RESULT, nullptr);
//    return (indexed ? new ESQLIndexedResult(sqlres) : new ESQLResult(sqlres));
//  }
//}