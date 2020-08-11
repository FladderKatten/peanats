#pragma once
#include "peanats/internal/globals.h"
#include "peanats/internal/logger.h"
#include "peanats/internal/message.h"
#include "peanats/internal/token.h"
#include "peanats/internal/receiver.h"

#include <inttypes.h>


PEANATS_NAMESPACE_BEGIN

// [ Forward decl ]
class Peanats;

//! State machine parser that processes a 'Receiver' 
//!
class Parser
{
public:
  // --------------------------------------------------------------------------
  // [ Constructor / Destructor ]
  // --------------------------------------------------------------------------
  Parser(Peanats* parent)
    : peanats(parent),
      remaining_payload(0),
      cs(0),
      top(0)
  {
    clear_state();
  }
      
  ~Parser() {}

  // --------------------------------------------------------------------------
  // [ Interface ]
  // --------------------------------------------------------------------------
protected:
  //! log interface for derived class
  virtual void log( const std::string&) = 0;

  //! logn interface for derived class
  virtual void logn(const std::string&) = 0;

  //! +OK callback interface for derived class
  virtual void on_parser_ok() {};
  
  //! PING callback interface for derived class
  virtual void on_parser_ping() {};
  
  //! PONG callback interface for derived class
  virtual void on_parser_pong() {};

  //! MSG callback interface for derived class
  virtual void on_parser_msg(Message& m) {};

  //! -ERR callback interface for derived class
  virtual void on_parser_err(Peastring&) {};

  //! INFO callback interface for derived class
  virtual void on_parser_info(Peastring&, Peastring&) {};

  // --------------------------------------------------------------------------
  // [ Internal ]
  // --------------------------------------------------------------------------

  //! call the state machine parser to process a 'Receiver'
  inline void ragel_parser(Receiver&);

  //! gets the first final state of the machine
  inline const int first_final_state();

  //! resets the state of the machine
  inline void clear_state();


  // --------------------------------------------------------------------------
  // [ Api ]
  // --------------------------------------------------------------------------
public:
  //! calls the state machine parser and adjust the 'Reciver' afterwards
  inline void parse(Receiver& receiver)
  {
    ragel_parser(receiver);

    // if we've finished without any started tokesm we've consumed the
    // whole buffer and can reset it
    if (tokens.empty()) {
      debug_logn("Finished in state without any tokens " + std::to_string(cs) + " and consumed the buffer");
      receiver.clear();
      return;
    }

    // Otherwise we may need to shift the buffer, but if we still have space
    // to recevice data, we'll wait until really full
    if (receiver.want()) {
      return;
    }

    // if the first token is not already at the start of the buffer
    // we move the data that needs saving to the front of the buffer
    if (tokens.front()._ptr != receiver.data()) {
      PEANATS_ASSERT(!tokens.empty(), "missing tokens");

      auto from = tokens.front()._ptr;
      auto dest = receiver.data();
      auto distance = from - dest;
      auto size = receiver._cursor - from;

      // move the data to the front of the buffer
      std::memmove(dest, from, size);

      // adjust the token pointers. Compare less or equal because
      // a token might be started but not finished
      for (size_t i = 0; i <= tokens.size(); i++)
        tokens[i]._ptr -= distance;

      // reset the cursor
      receiver._cursor -= distance;
      receiver._size = size;

      return;
    }

    // Otherwise we have a packet that is too large
    logn("parser_error: buffer is too small to handle the packet");
    clear_state();
    receiver.clear();
    return;
  }

  // --------------------------------------------------------------------------
  // [ Members ]
  // --------------------------------------------------------------------------
private:
  int cs;                         //!< state machine internal state
  int stack[4], top = 0;          //!< state machine internal stack

  TokenList tokens;               //!< list of 'Token'
  size_t remaining_payload = 0;   //!< the remaining payload when processing
  Peanats* peanats;               //!< parent peanats base class
};

PEANATS_NAMESPACE_END

#include <peanats/internal/autogen/ragel_parser.h>


