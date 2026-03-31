#include <http/http.hpp>
#include <iostream>

int main()
{
  auto nodes = http::make_chain(5);
  std::cout << "nodes=" << nodes.size() << "\n";
  return nodes.size() == 5 ? 0 : 1;
}
