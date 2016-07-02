#include "ESQL/ESQLResult.h"
#include <iostream>
#include <iomanip>
#include <mysql.h>

namespace	    ELib
{
  /* ESQLResult */
  
  ESQLResult::ESQLResult(void *sqlres) :
    m_sqlres(sqlres)
  {
    MYSQL_ROW	    row;
    int		    nbFields;
    unsigned long   *lengths;
    
    nbFields = mysql_num_fields(static_cast<MYSQL_RES*>(m_sqlres));
    while ((row = mysql_fetch_row(static_cast<MYSQL_RES*>(m_sqlres))))
    {
      m_rows.push_back(new ESQLRow());
      lengths = mysql_fetch_lengths(static_cast<MYSQL_RES*>(m_sqlres));
      for (int i = 0; i < nbFields; ++i)
	m_rows.back()->push((static_cast<char*>(memcpy(new char[lengths[i] + 1](), row[i], lengths[i]))));
    }
  }
  
  ESQLResult::~ESQLResult()
  {
    mysql_free_result(static_cast<MYSQL_RES*>(m_sqlres));
  }

  ESQLRow	    *ESQLResult::at(uint32 row)
  {
    if (row >= static_cast<uint32>(m_rows.size()))
      return (NULL);
    return (m_rows.at(row));
  }
  
  ESQLRow	    *ESQLResult::operator[](uint32 row)
  {
    return (at(row));
  }
  
  ESQLField	    *ESQLResult::at(uint32 row, uint32 column)
  {
    return (at(row)->at(column));
  }
  
  void		    ESQLResult::print() const
  {
    for (std::vector<ESQLRow*>::const_iterator it = m_rows.begin(); it != m_rows.end(); ++it)
      (*it)->print();
    std::cout << std::endl;
  }
  
  /* ESQLIndexedResult */
  
  ESQLIndexedResult::ESQLIndexedResult(void *sqlres) :
    ESQLResult(sqlres)
  {
    MYSQL_FIELD	    *field;
    
    while ((field = mysql_fetch_field(static_cast<MYSQL_RES*>(sqlres))))
    {
      FieldInfo	    infos;
      
      infos.name = field->name;
      infos.type = field->type;
      infos.sign = !(field->flags & UNSIGNED_FLAG);
      m_columnInfos.push_back(infos);
    }
  }
  
  ESQLIndexedResult::~ESQLIndexedResult()
  {
  }
  
  ESQLField	    *ESQLIndexedResult::at(uint32 row, const char *columnName)
  {
    for (std::vector<FieldInfo>::iterator it = m_columnInfos.begin(); it != m_columnInfos.end(); ++it)
      if (strcmp(columnName, (*it).name) == 0)
	return (ESQLResult::at(row)->at(static_cast<uint32>(it - m_columnInfos.begin())));
    return (nullptr);
  }
  
  void		    ESQLIndexedResult::rowAsObject(uint32 row, char *objectAddr)
  {
    for (std::vector<FieldInfo>::iterator it = m_columnInfos.begin(); it != m_columnInfos.end(); ++it)
    {
      ESQLField	    *field = at(row, it->name);
      
      switch (it->type)
      {
	case MYSQL_TYPE_TINY:
	  if (it->sign)
	    assignDataToObject<int8>(objectAddr, field);
	  else
	    assignDataToObject<uint8>(objectAddr, field);
	  break;
	case MYSQL_TYPE_SHORT:
	  if (it->sign)
	    assignDataToObject<int16>(objectAddr, field);
	  else
	    assignDataToObject<uint16>(objectAddr, field);
	  break;
	case MYSQL_TYPE_LONG:
	  if (it->sign)
	    assignDataToObject<int32>(objectAddr, field);
	  else
	    assignDataToObject<int32>(objectAddr, field);
	  break;
	case MYSQL_TYPE_LONGLONG:
	  if (it->sign)
	    assignDataToObject<int64>(objectAddr, field);
	  else
	    assignDataToObject<uint64>(objectAddr, field);
	  break;
	case MYSQL_TYPE_FLOAT:
	  assignDataToObject<float>(objectAddr, field);
	  break;
	case MYSQL_TYPE_DOUBLE:
	  assignDataToObject<double>(objectAddr, field);
	  break;
	case MYSQL_TYPE_VAR_STRING:
	  assignDataToObject<char*>(objectAddr, field);
	  break;
      }
    }
  }
  
  void		    ESQLIndexedResult::print() const
  {
    for (std::vector<FieldInfo>::const_iterator it = m_columnInfos.begin(); it != m_columnInfos.end(); ++it)
      std::cout << std::left << it->name << "-" << std::setw(15) << (it->sign ? '1' : '0');
    std::cout << std::endl;
    ESQLResult::print();
  }
}