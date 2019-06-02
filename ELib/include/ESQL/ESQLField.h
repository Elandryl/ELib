/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ESQLField.
*/

#pragma once

#include "EGlobals/EGlobal.h"

/**
  @brief General scope for ELib components.
*/
namespace       ELib
{

  /**
    @brief ELib object for SQL field container.
  */
  class         ESQLField
  {
  public:
    ESQLField(char *p_datas);
    ~ESQLField();
    operator    uint64() const;
    operator    uint32() const;
    operator    uint16() const;
    operator    uint8() const;
    operator    int64() const;
    operator    int32() const;
    operator    int16() const;
    operator    int8() const;
    operator    char*() const;
    operator    std::string() const;
    operator    float() const;
    operator    double() const;
    operator    bool() const;

  private:
    char        *m_datas; /**< ESQLField content as bytes. */
  };

}
