#include <mitama/data/static_string.hpp>
#include <mitama/data/type_list.hpp>
#include <mitama/functional/infix.hpp>

#include <iostream>

using namespace mitama::literals::static_string_literals;

int
main()
{
  std::cout << "ok"_ << std::endl;
}
