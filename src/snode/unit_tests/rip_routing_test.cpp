

#define BOOST_TEST_MODULE custom_main
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>
#include <iostream>
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE(test1)
{
  BOOST_TEST(false);
  BOOST_CHECK(1==2);
  BOOST_CHECK(2==2);
  BOOST_REQUIRE(2==2);
}

void make_use_of(char**)
{
  std::cout << "Using custom entry point..." << std::endl;
}

int main(int argc, char* argv[], char* envp[])
{
  make_use_of(envp);
  return utf::unit_test_main(init_unit_test, argc, argv);
}

