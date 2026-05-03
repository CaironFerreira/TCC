#pragma once

#include <stddef.h>
#include <stdint.h>

class IDisplayLayout;

struct DisplayLayoutEntry {
  uint8_t id = 0;
  IDisplayLayout* layout = nullptr;

  DisplayLayoutEntry() = default;
  DisplayLayoutEntry(uint8_t layoutId, IDisplayLayout* layoutPtr)
    : id(layoutId), layout(layoutPtr) {}
};
