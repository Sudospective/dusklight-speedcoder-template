#include "dst/dst.hpp"

#include "components/example.hpp"
#include "components/mine.hpp"

// This is DST's entry point.
// You can write your code here, but
// you must return a unique pointer of your
// base component using this:

// return std::make_unique<ClassName>();

std::unique_ptr<DST::Component> dst_main() {
  return std::make_unique<ExampleComponent>();
} 
