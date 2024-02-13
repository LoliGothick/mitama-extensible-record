#include <mitama/data/record.hpp>

#include <iostream>

using namespace mitama::literals::static_string_literals;

using namespace mitama::literals;
using namespace std::literals;

int
main()
{
  // declare record type
  using Person = mitama::record<mitama::named<"name"_, std::string>, mitama::named<"age"_, int>>;

  // make record
  Person john = mitama::empty += ("name"_ = "John"s) += ("age"_ = 42);

  // access to rows
  john["name"_]; // "John"
  john["age"_];  // 42
}