/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for EPrint handling tools.
*/

#pragma once

#include <map>
#include <queue>
#include "EGlobal.h"

/**
  @brief General scope for ELib components.
*/
namespace                         ELib
{

  /**
    @brief EPrintType default list.
    @details User can define custom type with value between EPRINT_TYPE_PRIORITY_HIGHEST and EPRINT_TYPE_PRIORITY_LOWEST.
    @details User can define custom type with special behavior at value over EPRINT_TYPE_SPECIALS.
    @details ESpecialPrinter can be provided for special types on pre and post priority printing.
  */
  enum                            EPrintType
  {
    EPRINT_TYPE_PRIORITY_HIGHEST  = 0x00, /**< Highest priority */
    EPRINT_TYPE_PRIORITY_ERROR    = 0x20, /**< ELib error printing. High priotiry. */
    EPRINT_TYPE_PRIORITY_STD      = 0x60, /**< ELib standard printing. Medium priority. */
    EPRINT_TYPE_PRIORITY_LOWEST   = 0x9F, /**< Lowest priority */
    EPRINT_TYPE_SPECIALS          = 0xA0, /**< Special types */
    EPRINT_TYPE_SPECIAL_ACTIVE    = 0xC0  /**< ELib active printing. Keep the current printing on last line. */
  };

  /**
    @brief Pointer to Function for ESpecialPrinter handling.
    @details This can be provided for Pre and Post handling of EPrinter.
    @param p_messages Queue of messages for special EPrintType.
  */
  typedef void(*ESpecialPrinter)(std::queue<std::string> &p_messages);

  /**
    @brief Pointer to Function for ESpecialPrinter processing.
    @details This can be provided for Generation handling of EPrinter.
    @param p_type EPrintType of the new message to be processed.
    @param p_message New message to be processed.
    @param p_messages Queue of messages for special EPrintType.
  */
  typedef void(*ESpecialProc)(EPrintType p_type, const std::string &p_message, std::queue<std::string> &p_messages);

  /**
    @brief ELib object for printing handling.
    @details Automatically print every message of an application depending on its type.
    @details PriorityMessage are printed depending on priotiry level.
    @details SpecialMessage are printed depending on ESpecialPrinter provided.
    @details Basic handling are provided. More can be provided with custom EPacketType.
  */
  class                           EPrinter
  {
  public:
    EPrinter();
    ~EPrinter();
    void                          start();
    void                          print();
    void                          stop();
    void                          print(EPrintType p_type, const std::string &p_message);
    std::string                   getLine();
    std::queue<std::string>       &getSpecialMessages(EPrintType p_type);
    void                          setSpecialProcPrinter(EPrintType p_type, ESpecialProc p_proc);
    void                          setSpecialPrePrinter(EPrintType p_type, ESpecialPrinter p_printer);
    void                          setSpecialPostPrinter(EPrintType p_type, ESpecialPrinter p_printer);

  private:
    HANDLE                        m_threadPrinter;        /**< Handle for printer thread. */
    HANDLE                        m_mutexPrinter;         /**< Semaphore for EPrinter thread safety. */
    bool                          m_isRunning;            /**< Indicate if EPrinter is running. */
    std::map<EPrintType,
      std::queue<std::string> >   m_priorityMessages;     /**< Queues for priority messages. */
    std::map<EPrintType,
      std::queue<std::string> >   m_specialMessages;      /**< Queues for special messages. */
    std::map<EPrintType,
      ESpecialProc>               m_specialProcPrinters;  /**< Condition for special messages. */
    std::map<EPrintType,
      ESpecialPrinter>            m_specialPrePrinters;   /**< Printers for special messages. */
    std::map<EPrintType,
      ESpecialPrinter>            m_specialPostPrinters;  /**< Printers for special messages. */
  };

  /**
    @brief Global variable for printing handling.
    @details Easy access maccro is provided with mEPRINT_G
    @details Easy usage maccro is provided with mEPRINT.
  */
  extern EPrinter                 __gEPrinter;

}

#define mEPRINT_G                 (ELib::__gEPrinter)

#define mEPRINT_STD(p_message)                                    \
{                                                                 \
  mEPRINT_G.print(ELib::EPRINT_TYPE_PRIORITY_STD, p_message);     \
}

#define mEPRINT_ERR(p_message)                                    \
{                                                                 \
  mEPRINT_G.print(ELib::EPRINT_TYPE_PRIORITY_ERROR, p_message);   \
}

#define mEPRINT_TYPE(p_type, p_message)                           \
{                                                                 \
  mEPRINT_G.print(p_type, p_message);                             \
}
