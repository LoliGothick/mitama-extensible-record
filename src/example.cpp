#include <mitama/data/record.hpp>

#include <iostream>

using namespace mitama::literals::static_string_literals;

int
main()
{
  auto record = mitama::record{
    mitama::as<"id"_>(42),
  };

  std::cout << record["id"_] << std::endl;
}
