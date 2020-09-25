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

#define CHECK_MSG_TRUE(cond, msg) CHECK_MSG(CHECK(cond), msg)

#define CHECK_BB(cond, bb, file, rank)                                                       \
  CHECK_MSG(cond,                                                                            \
            "bitboard<" << bb.files() << "x" << bb.ranks() << "> failed for field [" << file \
                        << ";" << rank << "]");

#define CHECK_BB_FALSE(cond, bb, file, rank) CHECK_BB(CHECK_FALSE(cond), bb, file, rank)

#define CHECK_BB_TRUE(cond, bb, file, rank) CHECK_BB(CHECK(cond), bb, file, rank)

#define CHECK_BB_THROWS(cond, exception, matcher, bb, file, rank) \
  CHECK_BB(CHECK_THROWS_AS(cond, exception), bb, file, rank);     \
  CHECK_BB(CHECK_THROWS_WITH(cond, matcher), bb, file, rank);

#define CHECK_BB_NOTHROW(cond, bb, file, rank) CHECK_BB(CHECK_NOTHROW(cond), bb, file, rank)

TEST_CASE("check file and rank", "[file, rank]") {
  SECTION("check file and rank constructor and conversion to size_t") {
    const int MAX_RAND = 120;
    const int MAX_ROUNDS = 100;

    for (int _ = 0; _ < MAX_ROUNDS; ++_) {
      size_t n = rand() % MAX_RAND;

      File file(n);
      Rank rank(n);

      // check the size of the File
      CHECK_MSG_TRUE(sizeof(file) == sizeof(size_t),
                     "The size of File should be the same as the size_t, got sizeof(file)="
                         << sizeof(file) << " while sizeof(size_t)==" << sizeof(size_t)
                         << " for number=" << n);

      // check the size of the Rank
      CHECK_MSG_TRUE(sizeof(rank) == sizeof(size_t),
                     "The size of Rank should be the same as the size_t, got sizeof(rank)="
                         << sizeof(rank) << " while sizeof(size_t)==" << sizeof(size_t)
                         << " for number=" << n);

      // check automatic conversion of File to size_t
      CHECK_MSG_TRUE(file == n,
                     "The value of the File(n) should be n when converted to size_t. Got "
                         << file << " while the n==" << n;);

      // check automatic conversion of Rank to size_t
      CHECK_MSG_TRUE(rank == n,
                     "The value of the Rank(n) should be n when converted to size_t. Got "
                         << rank << " while the n==" << n;);
    }
  }

  SECTION("check the literal operator") {
    CHECK(File(0) == 0_f);
    CHECK(File(10) == 10_f);
    CHECK(Rank(0) == 0_r);
    CHECK(Rank(12) == 12_r);
  }
}

TEST_CASE("check square", "[square]") {
  SECTION("check square constructor and converting to File and Rank") {
    const int MAX_RAND = 120;
    const int MAX_ROUNDS = 100;

    for (int _ = 0; _ < MAX_ROUNDS; ++_) {
      size_t f = rand() % MAX_RAND;
      size_t r = rand() % MAX_RAND;

      File file(f);
      Rank rank(r);
      Square square(file, rank);

      // check the size of the Square
      CHECK_MSG_TRUE(sizeof(square) == 2 * sizeof(size_t),
                     "The size of Square should be twice the size_t, got sizeof(square)="
                         << sizeof(square) << " while sizeof(size_t)==" << sizeof(size_t));

      auto casted_file = static_cast<File>(square);
      // check automatic conversion of Square to File
      CHECK_MSG_TRUE(
          static_cast<File>(square) == file,
          "The value of the Square(file, rank) when converted to File, should give File. Got "
              << casted_file << " while the File==" << file;);

      auto casted_rank = static_cast<Rank>(square);
      // check automatic conversion of Square to Rank
      CHECK_MSG_TRUE(
          static_cast<Rank>(square) == rank,
          "The value of the Square(file, rank) when converted to Rank, should give Rank. Got "
              << casted_rank << " while the Rank==" << rank;);
    }
  }

  SECTION("check the comma operator") {
    const int MAX_RAND = 120;
    const int MAX_ROUNDS = 100;

    for (int _ = 0; _ < MAX_ROUNDS; ++_) {
      size_t f = rand() % MAX_RAND;
      size_t r = rand() % MAX_RAND;

      File file(f);
      Rank rank(r);
      Square square = (file, rank);

      auto casted_file = static_cast<File>(square);
      // check automatic conversion of Square to File
      CHECK_MSG_TRUE(
          static_cast<File>(square) == file,
          "The value of the Square(file, rank) when converted to File, should give File. Got "
              << casted_file << " while the File==" << file;);

      auto casted_rank = static_cast<Rank>(square);
      // check automatic conversion of Square to Rank
      CHECK_MSG_TRUE(
          static_cast<Rank>(square) == rank,
          "The value of the Square(file, rank) when converted to Rank, should give Rank. Got "
              << casted_rank << " while the Rank==" << rank;);
    }
  }
}

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

template <size_t files_, size_t ranks_, bool always_check_range_>
void test_all_fields_are_empty_on_create() {
  bitboard<files_, ranks_, always_check_range_> bb;

  // check if all bits are set to false
  for (size_t file = 0; file < bb.files(); file++) {
    for (size_t rank = 0; rank < bb.ranks(); rank++) {
      CHECK_BB_FALSE(bb.get(File(file), Rank(rank)), bb, file, rank);
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

    // a couple of constants for simpler code
    bool file_too_large = file >= bb.files();
    bool rank_too_large = rank >= bb.ranks();
    bool expecting_throw = bb.always_check_range() && (file_too_large || rank_too_large);

#define check_throws(func_call, bb, file, rank)                                             \
  {                                                                                         \
    if (expecting_throw) {                                                                  \
      if (file_too_large) {                                                                 \
        CHECK_BB_THROWS(                                                                    \
            func_call, std::out_of_range&, "Requested file is too large.", bb, file, rank); \
      } else if (rank_too_large) {                                                          \
        CHECK_BB_THROWS(                                                                    \
            func_call, std::out_of_range&, "Requested rank is too large.", bb, file, rank); \
      }                                                                                     \
    } else {                                                                                \
      CHECK_BB_NOTHROW(func_call, bb, file, rank);                                          \
    }                                                                                       \
  }

    check_throws(bb.get(file, rank), bb, file, rank);
    check_throws(bb.get(Square(file, rank)), bb, file, rank);

    check_throws(bb.set(file, rank), bb, file, rank);
    check_throws(bb.set(Square(file, rank)), bb, file, rank);

    check_throws(bb.reset(file, rank), bb, file, rank);
    check_throws(bb.reset(Square(file, rank)), bb, file, rank);
  }
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void test_setting_too_large_bit() {
  // For any setters we have the problem that the setting too large bit can write some more memory
  // than allocated for the bitboard, so let's check just the allocated parts.
  // For this there is the `capacity()` function which returns the real number of allocated bits.
  //
  // Of course, this will work only and only if the the `always_check_range_==false`.

  bitboard<files_, ranks_, always_check_range_> bb;

  if (!bb.always_check_range() && bb.size() < bb.capacity()) {
    for (int n = -1; n < int(bb.capacity() - bb.size()); n++) {
      auto file = File(bb.files());
      auto rank = Rank(bb.ranks() + 1);

      CHECK(coordinates_to_index(file, rank, files_) >= bb.size());
      CHECK(coordinates_to_index(file, rank, files_) < bb.capacity());

      // set|get(file, rank)
      bb.set(file, rank);
      CHECK_BB_TRUE(bb.get(file, rank), bb, file, rank);

      bb.set(file, rank, false);
      CHECK_BB_FALSE(bb.get(file, rank), bb, file, rank);

      bb.set(file, rank);
      CHECK_BB_TRUE(bb.get(file, rank), bb, file, rank);

      bb.reset(file, rank);
      CHECK_BB_FALSE(bb.get(file, rank), bb, file, rank);

      // set|get(square)
      Square square(file, rank);

      bb.set(square);
      CHECK_BB_TRUE(bb.get(square), bb, file, rank);

      bb.set(square, false);
      CHECK_BB_FALSE(bb.get(square), bb, file, rank);

      bb.set(square);
      CHECK_BB_TRUE(bb.get(square), bb, file, rank);

      bb.reset(square);
      CHECK_BB_FALSE(bb.get(square), bb, file, rank);
    }
  }
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_basic_info_getters() {
  bitboard<files_, ranks_, always_check_range_> bb;

  CHECK(bb.ranks() == ranks_);
  CHECK(bb.files() == files_);
  CHECK(bb.always_check_range() == always_check_range_);
  CHECK(bb.size() == ranks_ * files_);
  CHECK(bb.capacity() == sizeof(bb) * 8);
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_test_function() {
  // this should always throw exception, regardless the `always_check_range_` argument.

  bitboard<files_, ranks_, always_check_range_> bb;
  const int MAX_RAND = 120;
  const int MAX_ROUNDS = 100;

  for (int _ = 0; _ < MAX_ROUNDS; ++_) {
    auto file = File(rand() % MAX_RAND);
    auto rank = Rank(rand() % MAX_RAND);
    auto square = (file, rank);

    // a couple of constants for simpler code
    bool file_too_large = file >= bb.files();
    bool rank_too_large = rank >= bb.ranks();

    if (file_too_large) {
      CHECK_BB_THROWS(
          bb.test(file, rank), std::out_of_range&, "Requested file is too large.", bb, file, rank);
      CHECK_BB_THROWS(
          bb.test(square), std::out_of_range&, "Requested file is too large.", bb, file, rank);

    } else if (rank_too_large) {
      CHECK_BB_THROWS(
          bb.test(file, rank), std::out_of_range&, "Requested rank is too large.", bb, file, rank);
      CHECK_BB_THROWS(
          bb.test(square), std::out_of_range&, "Requested rank is too large.", bb, file, rank);
    } else {
      CHECK_BB_NOTHROW(bb.test(file, rank), bb, file, rank);
      CHECK_BB_NOTHROW(bb.test(square), bb, file, rank);
    }
  }
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_set_all_function() {
  bitboard<files_, ranks_, always_check_range_> bb;

  // this should set all bits to true
  bb.set();

  // check if all bits are set to true
  for (size_t file = 0; file < bb.files(); file++) {
    for (size_t rank = 0; rank < bb.ranks(); rank++) {
      CHECK_BB_TRUE(bb.get(File(file), Rank(rank)), bb, file, rank);
    }
  }
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_set_function() {
  {
    bitboard<files_, ranks_, always_check_range_> bb;

    auto f = 0_f;
    auto r = 2_r;

    bb.set(f, r);

    // check if the only bit is set to true
    for (size_t file = 0; file < bb.files(); file++) {
      for (size_t rank = 0; rank < bb.ranks(); rank++) {
        auto expected_value = (file == f && rank == r);
        CHECK_BB_TRUE(bb.get(File(file), Rank(rank)) == expected_value, bb, file, rank);
      }
    }
  }
  // and the same for the set(square) now
  {
    bitboard<files_, ranks_, always_check_range_> bb;

    auto f = 0_f;
    auto r = 2_r;

    bb.set(Square(f, r));

    // check if the only bit is set to true
    for (size_t file = 0; file < bb.files(); file++) {
      for (size_t rank = 0; rank < bb.ranks(); rank++) {
        auto expected_value = (file == f && rank == r);
        CHECK_BB_TRUE(bb.get(File(file), Rank(rank)) == expected_value, bb, file, rank);
      }
    }
  }
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_all_function() {
  bitboard<files_, ranks_, always_check_range_> bb;

  CHECK(bb.all() == false);

  bb.set(File(0), Rank(1));
  CHECK(bb.all() == false);

  bb.set();
  CHECK(bb.all() == true);
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_none_function() {
  bitboard<files_, ranks_, always_check_range_> bb;

  CHECK(bb.none() == true);

  bb.set(File(0), Rank(1));
  CHECK(bb.none() == false);

  bb.set();
  CHECK(bb.none() == false);

  bb.reset();
  CHECK(bb.none() == true);
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_any_function() {
  bitboard<files_, ranks_, always_check_range_> bb;

  CHECK(bb.any() == false);

  bb.set(File(0), Rank(1));
  CHECK(bb.any() == true);

  bb.set();
  CHECK(bb.any() == true);

  bb.reset();
  CHECK(bb.any() == false);
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_reset_all_function() {
  bitboard<files_, ranks_, always_check_range_> bb;

  bb.set();
}

template <size_t files_, size_t ranks_, bool always_check_range_>
void check_reset_function() {
  {
    bitboard<files_, ranks_, always_check_range_> bb;

    auto f = 1_f;
    auto r = 2_r;

    bb.set();
    bb.reset(f, r);

    // check if the only bit is set to true
    for (size_t file = 0; file < bb.files(); file++) {
      for (size_t rank = 0; rank < bb.ranks(); rank++) {
        auto expected_value = !(file == f && rank == r);
        CHECK_BB_TRUE(bb.get(File(file), Rank(rank)) == expected_value, bb, file, rank);
      }
    }
  }
  // and the same for the set(square) now
  {
    bitboard<files_, ranks_, always_check_range_> bb;

    auto f = 0_f;
    auto r = 2_r;

    bb.set();
    bb.reset(Square(f, r));

    // check if the only bit is set to true
    for (size_t file = 0; file < bb.files(); file++) {
      for (size_t rank = 0; rank < bb.ranks(); rank++) {
        auto expected_value = !(file == f && rank == r);
        CHECK_BB_TRUE(bb.get(File(file), Rank(rank)) == expected_value, bb, file, rank);
      }
    }
  }
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
    run_tests(check_basic_info_getters);
    run_tests(test_all_fields_are_empty_on_create);
    run_tests(check_getting_too_large_field_coordinates);
    run_tests(test_setting_too_large_bit);
    run_tests(check_set_function);
    run_tests(check_reset_function);
    run_tests(check_test_function);
  }

  SECTION("check information functions") {
    run_tests(check_all_function);
    run_tests(check_none_function);
    run_tests(check_any_function);
  }

  SECTION("check to_string") {
    bitboard<2, 3> bb;
    CHECK(bb.to_string() == "------");

    bb.set(File(0), Rank(0));
    CHECK(bb.to_string() == "-----x");

    bb.set(File(0), Rank(2));
    bb.set(File(0), Rank(0), false);
    CHECK(bb.to_string() == "-x----");
  }

  SECTION("check to_string() with no range check") {
    bitboard<2, 3, false> bb;
    CHECK(bb.to_string() == "------");

    bb.set(File(0), Rank(0));
    CHECK(bb.to_string() == "-----x");

    bb.set(File(5), Rank(5));
    CHECK(bb.to_string() == "-----x");
  }
}