#include "bitboard.hpp"
#include "catch.hpp"

/**
 * A couple of remarks.
 *
 * The tests are written with a 64bit compiler in mind. It's possible that on 32bit compiler
 * some of tests will fail. I don't care about this for now.
 *
 * At the moment of creating the code, the implementation uses only bitset to store the bits.
 * However, I can imagine changing the storage implementation. Because of this, it's important
 * to write all the tests for both: a one-word-size bitset and multiple-word-size bitset.
 *
 * I will also use extensively bitboard<8,8> for chess implementation.
 *
 * To get good test coverage for the above cases, I make sure that every test checks all:
 *
 *   - bitboard<2,3>   (6 bits,   sizeof()=1*8)
 *   - bitboard<8,8>   (64 bits,  sizeof()=1*8)
 *   - bitboard<10,10> (100 bits, sizeof()=2*8)
 *
 */

using namespace slchess;

TEST_CASE("check_bitboard_size", "[bitboard]") {
  // ensure that there is no accidental size change

  size_t word_size = sizeof(unsigned long);

  // this should use one unsigned long variable
  bitboard<2, 3> bb2x3;
  CHECK(sizeof(bb2x3) == word_size);

  // this should use 64 bits
  bitboard<8, 8> bb8x8;
  CHECK(sizeof(bb8x8) == word_size);

  // this should be a little bit more than 64 bits
  bitboard<10, 10> bb10x10;
  CHECK(sizeof(bb10x10) == 2 * word_size);
}

struct params {
  const size_t files;
  const size_t ranks;
  const bool always_check_range;
};

#define CHECK_MSG(cond, msg) \
  do {                       \
    INFO(msg);               \
    cond;                    \
  } while ((void)0, 0)

#define CHECK_MESSAGE_FALSE(cond, msg) CHECK_MSG(CHECK_FALSE(cond), msg)

#define CHECK_BB_FALSE(cond)                                                                   \
  CHECK_MESSAGE_FALSE(cond,                                                                    \
                      "bitboard<" << files_ << "x" << ranks_ << "> failed for field [" << file \
                                  << ";" << rank << "]");

#define CHECK_BB(cond, bb, file, rank)                                                       \
  CHECK_MSG(cond,                                                                            \
            "bitboard<" << bb.files() << "x" << bb.ranks() << "> failed for field [" << file \
                        << ";" << rank << "]");

#define CHECK_BB_THROWS(cond, exception, matcher, bb, file, rank) \
  CHECK_BB(CHECK_THROWS_AS(cond, exception), bb, file, rank);     \
  CHECK_BB(CHECK_THROWS_WITH(cond, matcher), bb, file, rank);

#define CHECK_BB_NOTHROW(cond, bb, file, rank) CHECK_BB(CHECK_NOTHROW(cond), bb, file, rank)

template <size_t files_, size_t ranks_, bool always_check_range_>
void test_all_fields_are_empty_on_create() {
  bitboard<files_, ranks_, always_check_range_> bb;

  // check if all bits are set to zero
  for (size_t file = 0; file < bb.files(); file++) {
    for (size_t rank = 0; rank < bb.ranks(); rank++) {
      CHECK_BB_FALSE(bb.get(File(file), Rank(rank)));
    }
  }
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_getting_too_large_field_coordinates() {
  // here we check only if there is an exception in case of `always_check_range_=true`
  // when the file or rank is too large

  bitboard<files_, ranks_, always_check_range_> bb;
  const int MAX_RAND = 120;
  const int MAX_ROUNDS = 100;

  for (int _ = 0; _ < MAX_ROUNDS; ++_) {
    auto file = File(rand() % MAX_RAND);
    auto rank = Rank(rand() % MAX_RAND);

    if (bb.always_check_range()) {
      if (file >= bb.files()) {  // we have too large file number
        CHECK_BB_THROWS(
            bb.set(file, rank), std::out_of_range&, "Requested file is too large.", bb, file, rank);
      } else if (rank >= bb.ranks()) {  // we have too large file number
        CHECK_BB_THROWS(
            bb.set(file, rank), std::out_of_range&, "Requested rank is too large.", bb, file, rank);
      } else {
        CHECK_BB_NOTHROW(bb.set(file, rank), bb, file, rank);
      }
    }
  }
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void test_basic_operations() {
  bitboard<files_, ranks_, always_check_range_> bb;

  // for setter we have the problem that the setting too large bit can write some more memory
  // than allocated for the bitboard, so let's check just the allocated parts
  if (!bb.always_check_range() && bb.size() < bb.capacity()) {
    auto file = File(bb.files());
    auto rank = Rank(bb.ranks() - 1);
    bb.set(file, rank);
    auto x = bb.get(file, rank);
  }

  //    bb.set(file, rank);
  //    CHECK(bb.get(File(file), Rank(rank)));
}

TEST_CASE("test_bitboard", "[bitboard]") {
  static constexpr params bb2x3 = {2, 3, true};
  static constexpr params bb2x3_nocheck = {2, 3, false};
  static constexpr params bb8x8 = {8, 8, true};
  static constexpr params bb8x8_nocheck = {8, 8, false};
  static constexpr params bb10x10 = {10, 10, true};
  static constexpr params bb10x10_nocheck = {10, 10, false};

#define run_tests(func)                                                               \
  func<bb2x3.files, bb2x3.ranks, bb2x3.always_check_range>();                         \
  func<bb8x8.files, bb8x8.ranks, bb8x8.always_check_range>();                         \
  func<bb10x10.files, bb10x10.ranks, bb10x10.always_check_range>();                   \
  func<bb2x3_nocheck.files, bb2x3_nocheck.ranks, bb2x3_nocheck.always_check_range>(); \
  func<bb8x8_nocheck.files, bb8x8_nocheck.ranks, bb8x8_nocheck.always_check_range>(); \
  func<bb10x10_nocheck.files, bb10x10_nocheck.ranks, bb10x10_nocheck.always_check_range>();

  SECTION("check basic operations") {
    run_tests(test_all_fields_are_empty_on_create);
    run_tests(check_getting_too_large_field_coordinates);
    run_tests(test_basic_operations);
  }
}