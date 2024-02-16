#include <mitama/data/record.hpp>

#include <cassert>
#include <string>

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
  assert(john["name"_] == "John");
  assert(john["age"_] == 42);

  // CTAD
  mitama::record tom = {
    "age"_ = 24, // <------
    "name"_ = "Tom"s, // <-
  };

  // john = tom; // error
  john = mitama::shrink(tom); // ok

  // named parameters?
  func({ "name"_ = "John"s, "age"_ = 42 });

  auto [name, age] = john.dissolve();

  assert(name.value() == "Tom");
  assert(age.value() == 24);
}
