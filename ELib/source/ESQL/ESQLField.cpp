//#include "ESQL/ESQLField.h"
//#include <iostream>
//#include <iomanip>
//
//namespace               ELib
//{
//  ESQLField::ESQLField(char *data) :
//    m_data(data)
//  {
//  }
//  
//  ESQLField::~ESQLField()
//  {
//    if (m_data)
//      delete(m_data);
//  }
//  
//  ESQLField::operator   uint64() const
//  {
//    return (static_cast<uint64>(strtoul(m_data, nullptr, 10)));
//  }
//  
//  ESQLField::operator   uint32() const
//  {
//    return (static_cast<uint32>(strtoul(m_data, nullptr, 10)));
//  }
//  
//  ESQLField::operator   uint16() const
//  {
//    return (static_cast<uint16>(strtoul(m_data, nullptr, 10)));
//  }
//  
//  ESQLField::operator   uint8() const
//  {
//    return (static_cast<uint8>(atoi(m_data)));
//  }
//  
//  ESQLField::operator   int64() const
//  {
//    return (static_cast<int64>(strtol(m_data, nullptr, 10)));
//  }
//  
//  ESQLField::operator   int32() const
//  {
//    return (static_cast<int32>(strtol(m_data, nullptr, 10)));
//  }
//  
//  ESQLField::operator   int16() const
//  {
//    return (static_cast<int16>(strtol(m_data, nullptr, 10)));
//  }
//  
//  ESQLField::operator   int8() const
//  {
//    return (static_cast<int8>(atoi(m_data)));
//  }
//  
//  ESQLField::operator   char*() const
//  {
//    return (static_cast<char*>(memcpy(new char[strlen(m_data) + 1](), m_data, strlen(m_data))));
//  }
//  
//  ESQLField::operator   std::string() const
//  {
//    return (std::string(m_data));
//  }
//  
//  ESQLField::operator   float() const
//  {
//    return (static_cast<float>(strtof(m_data, nullptr)));
//  }
//  
//  ESQLField::operator   double() const
//  {
//    return (static_cast<double>(strtod(m_data, nullptr)));
//  }
//  
//  ESQLField::operator   bool() const
//  {
//    return (static_cast<uint8>(strtoul(m_data, nullptr, 10)) != 0);
//  }
//  
//  void                  ESQLField::print() const
//  {
//    std::cout << std::left << std::setw(20) << m_data;
//  }
//}