#pragma once
#include <peanats/internal/globals.h>
#include <peanats/internal/peastring.h>

PEANATS_NAMESPACE_BEGIN

// Forward declaration
class Peanats;

class Message
{
public:
  Message(Peanats* client)
    : client(client) {}

  // Members
  Peastring  subject;
  Peastring  replyto;
  Peastring  payload;
  Peanats*   client;
  uint32_t   sid;
};

PEANATS_NAMESPACE_END