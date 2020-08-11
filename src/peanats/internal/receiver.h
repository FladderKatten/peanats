#pragma once
#include <peanats/internal/globals.h>
#include <inttypes.h>

PEANATS_NAMESPACE_BEGIN


//! Class for managing a copy-free and allocation-free
//! tcp receive buffer
class Receiver
{
public:
  //! ctor, 1k default 
  Receiver(size_t capacity = 1024)
    : _data(new char[capacity]),
      _cursor(_data),
      _capacity(capacity),
      _size(0) {}

  // dtor
  ~Receiver() {
    if (_capacity)
      delete[] _data;
  }

  //! gets a pointer to the raw data of the buffer
  inline char* data() const { return _data; }

  //! gets the current size of the buffer
  inline const size_t have() { return _size; }

  //! gets the number of free elements until full
  inline const size_t want() { return _capacity - _size; }

  //! clears the content of the buffer
  void clear() { _size = 0; _cursor = _data; }

  //! appends a character to the buffer
  void push_back(const char c) {
    PEANATS_ASSERT(_size < _capacity, "buffer overflow");
    _data[_size++] = c;
  }

  // [ Members ]
  char*  _data;
  char*  _cursor;
  size_t _capacity;
  size_t _size;
};



PEANATS_NAMESPACE_END