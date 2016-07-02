#include "ESQL/ESQLRow.h"
#include <iostream>

namespace ELib
{
  ESQLRow::ESQLRow()
  {
  }
  
  ESQLRow::~ESQLRow()
  {
  }
  
  void	      ESQLRow::push(char *data)
  {
    m_fields.push_back(new ESQLField(data));
  }
  
  void	      ESQLRow::push(ESQLField *field)
  {
    m_fields.push_back(field);
  }
  
  ESQLField   *ESQLRow::at(uint32 column)
  {
    if (column >= static_cast<uint32>(m_fields.size()))
      return (NULL);
    return (m_fields[column]);
  }
  
  ESQLField   *ESQLRow::operator[](uint32 column)
  {
    return (at(column));
  }
  
  char	      **ESQLRow::toCharArray()
  {
    char      **dataArray;
    
    dataArray = new char*[m_fields.size()] + 1;
    for (uint32 field = 0; field < m_fields.size(); ++field)
      dataArray[field] = *m_fields[field];
    dataArray[m_fields.size()] = NULL;
    return (dataArray);
  }
  
  void	      ESQLRow::print() const
  {
    for (std::vector<ESQLField*>::const_iterator it = m_fields.begin(); it != m_fields.end(); ++it)
      (*it)->print();
    std::cout << std::endl;
  }
}