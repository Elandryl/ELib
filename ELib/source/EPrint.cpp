/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EPrint handling tools.
*/

#include <iostream>
#include <conio.h>
#include "EPrint.h"

/**
  @brief General scope for ELib components.
*/
namespace         ELib
{

  /**
    @brief Processing function for EPRINT_TYPE_SPECIAL_ACTIVE printing.
    @details It is called when a new printing is asked by application.
    @details Make sure only one message of Active is handled.
    @param p_type EPrintType of the new message to be processed.
    @param p_message New message to be processed.
    @param p_messages Queue of messages for special EPrintType.
  */
  void            EPrinterActiveProc(EPrintType p_type, const std::string &p_message, std::queue<std::string> &p_messages)
  {
    if (EPRINT_TYPE_SPECIAL_ACTIVE == p_type)
    {
      if (true == p_messages.empty())
      {
        std::cout << p_message;
      }
      p_messages.push(p_message);
    }
    if (1 == p_messages.size())
    {
      p_messages.push(p_messages.front());
    }
  }

  /**
    @brief Pre-printing function for EPRINT_TYPE_SPECIAL_ACTIVE printing.
    @details It is called before priority printings.
    @details Erase the current Active line before priority printings.
    @param p_messages Queue of messages for active messages.
  */
  void            EPrinterActivePre(std::queue<std::string> &p_messages)
  {
    if (1 < p_messages.size())
    {
      std::cout << '\r' << std::string(p_messages.front().size(), ' ') << '\r';
    }
  }

  /**
    @brief Post-printing function for EPRINT_TYPE_SPECIAL_ACTIVE printing.
    @details It is called after priority printings.
    @details Print the Active line as last console output.
    @details It might not be last console output depending on user implementation.
    @param p_messages Queue of messages for active messages.
  */
  void            EPrinterActivePost(std::queue<std::string> &p_messages)
  {
    if (1 < p_messages.size())
    {
      p_messages.pop();
      std::cout << p_messages.front();
    }
  }

  /**
    @brief Functor for printer thread.
    @param p_param Pointer to param from CreateThread caller.
    @return Unused.
  */
  DWORD WINAPI    PrinterFunctor(LPVOID p_param)
  {
    static_cast<EPrinter*>(p_param)->print();

    return (0);
  }

  /**
    @brief Instantiate EPrinter handling ELibs types.
    @details Create EPrinter for printing handling.
    @details Handle EPRINT_TYPE_PRIORITY_ERROR, EPRINT_TYPE_PRIORITY_STD and EPRINT_TYPE_SPECIAL_ACTIVE by default.
  */
  EPrinter::EPrinter() :
    m_threadPrinter(nullptr),
    m_mutexPrinter(nullptr),
    m_isRunning(false),
    m_priorityMessages(),
    m_specialMessages(),
    m_specialProcPrinters(),
    m_specialPrePrinters(),
    m_specialPostPrinters()
  {
    m_mutexPrinter = CreateMutex(nullptr, false, nullptr);
    m_specialProcPrinters[EPRINT_TYPE_SPECIAL_ACTIVE] = &EPrinterActiveProc;
    m_specialPrePrinters[EPRINT_TYPE_SPECIAL_ACTIVE] = &EPrinterActivePre;
    m_specialPostPrinters[EPRINT_TYPE_SPECIAL_ACTIVE] = &EPrinterActivePost;
  }

  /**
    @brief Destroy a EPrinter.
    @details Release its mutex and terminate its thread.
  */
  EPrinter::~EPrinter()
  {
    ReleaseMutex(m_mutexPrinter);
    CloseHandle(m_mutexPrinter);
    TerminateThread(m_threadPrinter, 0);
    CloseHandle(m_threadPrinter);
  }

  /**
    @brief Start automation of printing handling.
    @details Create a thread for printing loop.
  */
  void            EPrinter::start()
  {
    if (false == m_isRunning)
    {
      m_isRunning = true;
      m_threadPrinter = CreateThread(nullptr, 0, PrinterFunctor, this, 0, nullptr);
    }
  }

  /**
    @brief Loop for printing.
    @details First call every pre special functions depending on their priorities.
    @details Then handle priority printings.
    @details Finally call every post special functions depending on their priorities.
  */
  void            EPrinter::print()
  {
    while (true == m_isRunning)
    {
      bool        l_printed = false;

      WaitForSingleObject(m_mutexPrinter, INFINITE);
      for (std::map<EPrintType, ESpecialPrinter>::iterator l_it = m_specialPrePrinters.begin(); l_it != m_specialPrePrinters.end(); ++l_it)
      {
        if ((nullptr != l_it->second)
          && (false == m_specialMessages[l_it->first].empty()))
        {
          l_it->second(m_specialMessages[l_it->first]);
        }
      }
      for (std::map<EPrintType, std::queue<std::string> >::iterator l_it = m_priorityMessages.begin(); l_it != m_priorityMessages.end(); ++l_it)
      {
        if ((false == l_printed)
          && (false == l_it->second.empty())
          && (EPRINT_TYPE_PRIORITY_HIGHEST < l_it->first)
          && (EPRINT_TYPE_PRIORITY_LOWEST > l_it->first))
        {
          if (EPRINT_TYPE_PRIORITY_ERROR >= l_it->first)
          {
            std::cerr << l_it->second.front().c_str() << std::endl;
          }
          else
          {
            std::cout << l_it->second.front().c_str() << std::endl;
          }
          l_it->second.pop();
          l_printed = true;
        }
      }
      for (std::map<EPrintType, ESpecialPrinter>::iterator l_it = m_specialPostPrinters.begin(); l_it != m_specialPostPrinters.end(); ++l_it)
      {
        if ((nullptr != l_it->second)
          && (false == m_specialMessages[l_it->first].empty()))
        {
          l_it->second(m_specialMessages[l_it->first]);
        }
      }
      ReleaseMutex(m_mutexPrinter);
    }
  }

  /**
    @brief Stop automation printing.
    @details Terminate the print loop thread.
  */
  void            EPrinter::stop()
  {
    if (true == m_isRunning)
    {
      ReleaseMutex(m_mutexPrinter);
      TerminateThread(m_threadPrinter, 0);
      m_isRunning = false;
    }
  }

  /**
    @brief User function for printing.
    @details Handle message for printing automation, depending on its type.
    @details Special messages are handle by user defined Proc function. Container for them are provided with GetSpecialMessages.
    @param p_type EPrintType of the new message to be handle.
    @param p_message New message to be handle.
  */
  void            EPrinter::print(EPrintType p_type, const std::string &p_message)
  {
    if (true == m_isRunning)
    {
      WaitForSingleObject(m_mutexPrinter, INFINITE);
      for (std::map<EPrintType, ESpecialProc>::iterator l_it = m_specialProcPrinters.begin(); l_it != m_specialProcPrinters.end(); ++l_it)
      {
        if (nullptr != l_it->second)
        {
          l_it->second(p_type, p_message, m_specialMessages[l_it->first]);
        }
      }
      if ((EPRINT_TYPE_PRIORITY_HIGHEST < p_type)
        && (EPRINT_TYPE_PRIORITY_LOWEST > p_type))
      {
        m_priorityMessages[p_type].push(p_message);
      }
      ReleaseMutex(m_mutexPrinter);
    }
  }

  /**
    @brief ELib getLine functions.
    @details This function is provided in accordance with ESpecial active type.
  */
  std::string     EPrinter::getLine()
  {
    std::string   l_line = "";
    char          l_cin = 0;

    l_cin = _getch();
    while ('\r' != l_cin)
    {
      if ('\b' != l_cin)
      {
        std::cout << l_cin;
        l_line += l_cin;
        if (false == m_specialMessages[EPRINT_TYPE_SPECIAL_ACTIVE].empty())
        {
          m_specialMessages[EPRINT_TYPE_SPECIAL_ACTIVE].front() += l_cin;
        }
        else
        {
          m_specialProcPrinters[EPRINT_TYPE_SPECIAL_ACTIVE](EPRINT_TYPE_SPECIAL_ACTIVE, l_line, m_specialMessages[EPRINT_TYPE_SPECIAL_ACTIVE]);
        }
      }
      else
      {
        if (0 < l_line.size())
        {
          l_line.erase(l_line.size() - 1);
          m_specialMessages[EPRINT_TYPE_SPECIAL_ACTIVE].front().erase(m_specialMessages[EPRINT_TYPE_SPECIAL_ACTIVE].front().size() - 1);
          std::cout << "\b \b";
        }
      }
      l_cin = _getch();
    }
    std::cout << std::endl;
    
    return (l_line);
  }

  /**
    @brief Container for message of specified special EPrintType.
    @param p_type EPrintType of messages to be retrieved.
    @return queue of message of specified special EPrintType.
  */
  std::queue<std::string>   &EPrinter::getSpecialMessages(EPrintType p_type)
  {
    return (m_specialMessages[p_type]);
  }

  /**
    @brief Add ESpecialProc to automation for specified special EPrintType.
    @param p_type EPrintType of messages to be processed.
    @param p_proc ESpecialProc for processing automation function.
  */
  void            EPrinter::setSpecialProcPrinter(EPrintType p_type, ESpecialProc p_proc)
  {
    m_specialProcPrinters[p_type] = p_proc;
  }

  /**
    @brief Add ESpecialPrinter for pre-printing automation for specified special EPrintType.
    @param p_type EPrintType of messages to be processed.
    @param p_proc ESpecialPrinter for pre-printing automation function.
  */
  void            EPrinter::setSpecialPrePrinter(EPrintType p_type, ESpecialPrinter p_printer)
  {
    m_specialPrePrinters[p_type] = p_printer;
  }

  /**
    @brief Add ESpecialPrinter for post-printing automation for specified special EPrintType.
    @param p_type EPrintType of messages to be processed.
    @param p_proc ESpecialPrinter for post-printing automation function.
  */
  void            EPrinter::setSpecialPostPrinter(EPrintType p_type, ESpecialPrinter p_printer)
  {
    m_specialPostPrinters[p_type] = p_printer;
  }

}
