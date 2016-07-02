#pragma once

#include <string>
#include "ScaledTypes.h"

namespace   ELib
{
  class	    ESQLField
  {
  public:
    ESQLField(char *data);
    ~ESQLField();
    operator uint64() const;
    operator uint32() const;
    operator uint16() const;
    operator uint8() const;
    operator int64() const;
    operator int32() const;
    operator int16() const;
    operator int8() const;
    operator char*() const;
    operator std::string() const;
    operator float() const;
    operator double() const;
    operator bool() const;
    void    print() const;

  private:
    char    *m_data;
  };
}