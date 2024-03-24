#include <catch2/catch_test_macros.hpp>

#include <base/document_tree/Container.h>

using namespace DocumentTree;

SCENARIO("Container basic manipulations", "[simple]")
{
  GIVEN("a Container")
  {
    Container a;
    WHEN("we do nothing")
    {
      THEN("name is unset and the uuid is valid")
      {
        REQUIRE(a.getName() == "");
        REQUIRE(a.getUuid().isValid());
      }
    }
    AND_WHEN("we set a name") {
      std::string name("container_name");
      a.setName(name);
      THEN("the name must be set") {
        REQUIRE(a.getName() == name);
      }
    }
    AND_WHEN("we set the name again") {
      std::string name("another name!!");
      a.setName(name);
      THEN("the name must be set") {
        REQUIRE(a.getName() == name);
      }
    }
  }
}
