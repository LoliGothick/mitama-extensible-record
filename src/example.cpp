#include <mitama/data/record.hpp>

#include <iostream>

using namespace mitama::literals;
using namespace std::literals;

// declare record type
using Person = mitama::record<
    mitama::named<"name"_, std::string>, mitama::named<"age"_, int>>;

void
func(Person)
{
  // ...
}

int
main()
{
  // make record
  Person john = mitama::empty_record += ("name"_ = "John"s) += ("age"_ = 42);

  // access to rows
  john["name"_]; // "John"
  john["age"_]; // 42

  // CTAD
  [[maybe_unused]] mitama::record _ = {
    "name"_ = "Tom"s,
    "age"_ = 42,
  };

  // named parameters?
  func({ "name"_ = "John"s, "age"_ = 42 });

  [[maybe_unused]] auto [name, age] = mitama::select<"name"_, "age"_>(john);
}
