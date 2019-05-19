#include "chip8.h"
#include <iostream>

#define BOOST_TEST_MODULE chip8test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test1 )
{
  BOOST_CHECK( 2 == 1 );
}

