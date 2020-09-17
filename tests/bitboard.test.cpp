#include "catch.hpp"
#include "bitboard.hpp"

using namespace slchess;

TEST_CASE("check_bitboard_size", "[bitboard]") {

  bitboard<4, 5> bb;

  CHECK(sizeof(bb) == 8);

}