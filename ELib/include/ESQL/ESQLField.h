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
    ESQLField(char *p_datas);               /**< /!\ ..E. */
    ~ESQLField();                           /**< /!\ .... */
    operator    uint64() const;             /**< /!\ ..E. */
    operator    uint32() const;             /**< /!\ ..E. */
    operator    uint16() const;             /**< /!\ ..E. */
    operator    uint8() const;              /**< /!\ ..E. */
    operator    int64() const;              /**< /!\ ..E. */
    operator    int32() const;              /**< /!\ ..E. */
    operator    int16() const;              /**< /!\ ..E. */
    operator    int8() const;               /**< /!\ ..E. */
    operator    char*() const;              /**< /!\ ..E. */
    operator    const std::string() const;  /**< /!\ ..E. */
    operator    float() const;              /**< /!\ ..E. */
    operator    double() const;             /**< /!\ ..E. */
    operator    bool() const;               /**< /!\ ..E. */

  private:
    char        *m_datas; /**< ESQLField content as bytes. */
  };

}
