#include "yu/stream/teestream.hpp"

#include <iostream>
#include <sstream>

int main() {
  std::ostringstream out1;
  std::ostringstream out2;
  yu::stream::oteestream ots(out1, out2);
  ots << "Hello " << "T-stream"<< " World!";
  ots.flush();
  std::cout << "out1 = " << out1.str() << std::endl;
  std::cout << "out2 = " << out2.str() << std::endl;
}
