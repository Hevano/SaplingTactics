#include "lib.hpp"

auto main() -> int
{
  auto const lib = library {};

  return lib.name == "SaplingTactics" ? 0 : 1;
}
