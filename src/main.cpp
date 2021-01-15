#include "computer.hpp"

auto main() -> int {
  auto cmp = synacor::Computer();
  cmp.read_file("challenge.bin");
  cmp.run(0);

  return 0;
}
