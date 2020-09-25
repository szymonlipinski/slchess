#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

namespace slchess {

/**
 * Representation of the bitboards file.
 *
 * The constructor is not implicit, so you have to use `File(1)` for every call,
 * as there is no implicit conversion from int.
 *
 * There is an automated conversion the other side, from the File to the size_t.
 *
 * There is also an operator for literals: `12_f` which created `File(12)`.
 */
class File {
 protected:
  size_t value;  ///< File value

 public:
  explicit File(size_t value) : value(value){};
  [[nodiscard]] operator size_t() const { return value; }
};

/**
 * Operator for custom literals like: `12_f`.
 *
 * @param n File value.
 * @return A new File object.
 */
File operator"" _f(unsigned long long int n) { return File(n); }

class Rank {
 protected:
  size_t value;  ///< Rank value

 public:
  explicit Rank(size_t value) : value(value){};
  [[nodiscard]] operator size_t() const { return value; }
};

Rank operator"" _r(unsigned long long int n) { return Rank(n); }

class Square {
 public:
  const File file;  ///< Square file
  const Rank rank;  ///< Square rank

  explicit Square(File file, Rank rank) : file(file), rank(rank) {}
  [[nodiscard]] operator File() { return file; }
  [[nodiscard]] operator Rank() { return rank; }
};

/**
 * Comma operator for creating a Square object.
 *
 * This can be useful for code like this:
 *
 *      File f(1);
 *      Rank r(2);
 *      Square sq = (f, r);
 *
 * The comma operator has the lowest precedence, there are also some compiler checks disallowing
 * pure comma lists, so you have to
 *
 *
 * @param file
 * @param rank
 * @return
 */
Square operator,(File file, Rank rank) { return Square(file, rank); }

/**
 * A function for calculating the position in an array for the (file, rank) coordinates.
 *
 * @param file Square file to get the position for.
 * @param rank Square rank to get the position for.
 * @param max_files Maximum number of files for the bitboard.
 * @return Array index for the square(file, rank).
 */
[[nodiscard]] constexpr size_t coordinates_to_index(size_t file, size_t rank, size_t max_files) {
  return rank * max_files + file;
}

/**
 * A generic bitboard representation.
 *
 * The main idea is to have a bit for each board field.
 *
 * This implementation is based on the std::bitboard. As a result the memory used for storing an MxN
 * bitboard is MxN rounded to the smallest multiple of `sizeof(unsigned long)`.
 *
 * The API is heavily inspired by the bitset
 * https://www.cplusplus.com/reference/bitset/bitset/
 *
 * The main differences are:
 *  - the operator [] is used only for the double bracket version like bitboard[File(1)][Rank(2)]
 *  - functions check the range only when the bitboard is created with the `always_check_range_`
 * param
 *  - the only function always checking the range is `test`
 *  - there is no function getting only size_t params, all the functions require either File, Rank
 * or Square param
 *
 *
 * @tparam files_ Number of the files for the bitboard.
 * @tparam ranks_ Number of the ranks for the bitboard
 * @tparam always_check_range_ If true, all the access functions always check the range, if it's
 * false, only the `test` function checks.
 */

template <size_t files_, size_t ranks_, bool always_check_range_ = true>
class bitboard {
 private:
  std::bitset<files_ * ranks_> fields;

  /**
   * A helper function to calculate the array index for the (file, rank) field.
   *
   * This is the only place where we check if the file and rank values are not too large and only
   * if the argument `always_check_range_` is set to true.
   *
   * @param file square file to get the position for.
   * @param rank square rank to get the position for.
   * @return Array index for the square(file, rank).
   */
  [[nodiscard]] size_t make_index(File file, Rank rank) const noexcept(!always_check_range_) {
    auto pos = coordinates_to_index(file, rank, files_);

    if constexpr (always_check_range_) {
      assert_field_coordinates(file, rank);
    }

    return pos;
  }

  void assert_field_coordinates(File file, Rank rank) const {
    if (file >= files_) {
      throw std::out_of_range("Requested file is too large.");
    }
    if (rank >= ranks_) {
      throw std::out_of_range("Requested rank is too large.");
    }
  }

 protected:
  /**
   * This is a little bit dangerous function. It returns a reference to the internal fields
   * variable.
   *
   * This is created to be used only in tests to be sure that some operations really modified
   * appropriate bits. To use it, create a class inheriting from the `bitboard` and then you will
   * have the access to the function.
   */
  [[maybe_unused]] auto get_fields() { return &fields; }

 public:
  bitboard() = default;
  ~bitboard() = default;

  /**
   * Constructor converting an unsigned variable to the bitboard.
   *
   * @param value Value to convert to the bitboard.
   */
  constexpr bitboard(unsigned long long value) noexcept : fields(value) {}

  /**
   * Returns the number of the ranks of the bitboard.
   */
  [[nodiscard]] constexpr auto ranks() const { return ranks_; }

  /**
   * Returns the number of the files of the bitboard.
   */
  [[nodiscard]] constexpr auto files() const { return files_; }

  /**
   * Returns the value for the template argument `always_check_range_`.
   */
  [[nodiscard]] constexpr auto always_check_range() const { return always_check_range_; }

  /**
   * Returns the number of bits available for the whole board. Basically it's ranks_ * files_.
   */
  [[nodiscard]] size_t size() const { return fields.size(); }

  /**
   * Returns theoretical maximum number of bits available for the allocated memory.
   *
   * Currently the implementation is using the std::bitset, which uses an array of `unsigned long`
   * (the size on my machine is 8B) for the internal bits representation.
   * For bitset<4> it will use whole `unsigned long` variable. This way the `size()=4` but the
   * `capacity()=64`. As the size of the `std::bitset` is exactly the size of the internal array,
   * I can just use `sizeof(std::bitset<4>) * 8` to get the number of available bits.
   *
   * This is extremely useful for testing the bitboard with `always_check_range_=false`.
   * When I have e.g. `bitboard<3,3>` with just 9 bits, there are 8 bytes allocated, so I can set
   * any bit in the range of 0 to 63 and it's memory safe as it won't change any other variable
   * in the memory.
   */
  [[nodiscard]] size_t capacity() const { return sizeof(fields) * 8; }

  /**
   * Sets all the fields of the bitboard to true.
   *
   * @return
   */
  bitboard& set() noexcept {
    fields.set();
    return *this;
  }

  /**
   * Sets the given field to the required value.
   *
   * @param file Field file to set to the required value.
   * @param rank  Field rank to set to the required value.
   * @param value The value to set
   * @return Reference to the bitboard object.
   */
  bitboard& set(File file, Rank rank, bool value = true) noexcept(!always_check_range_) {
    auto index = make_index(file, rank);
    fields[index] = value;
    return *this;
  }

  /**
   * Sets the given field to the required value.
   *
   * @param square Square to set to the required value.
   * @param value The value to set
   * @return Reference to the bitboard object.
   */
  bitboard& set(Square square, bool value = true) noexcept(!always_check_range_) {
    return set(square, square, value);
  }

  /**
   * Sets all the fields to false.
   *
   * @return Reference to the bitboard object.
   */
  bitboard& reset() noexcept {
    fields.reset();
    return *this;
  }

  /**
   * Sets the given field to false.
   *
   * @param file Field file to set to the required value.
   * @param rank  Field rank to set to the required value.
   * @return Reference to the bitboard object.
   */
  bitboard& reset(File file, Rank rank) noexcept(!always_check_range_) {
    auto index = make_index(file, rank);
    fields[index] = false;
    return *this;
  }

  /**
   * Sets the given field to false.
   *
   * @param square Square to set to the required value.
   * @param value The value to set
   * @return Reference to the bitboard object.
   */
  bitboard& reset(Square square) noexcept(!always_check_range_) { return reset(square, square); }

  /**
   * Gets the value of the given field.
   *
   * @param file Field file to set to the required value.
   * @param rank  Field rank to set to the required value.
   * @return Value for the field.
   */
  [[nodiscard]] bool get(File file, Rank rank) const noexcept(!always_check_range_) {
    auto index = make_index(file, rank);
    return fields[index];
  }

  /**
   * Gets the value of the given field.
   *
   * @param square Square to set to the required value.
   * @return Value for the field.
   */
  [[nodiscard]] bool get(Square square) const noexcept(!always_check_range_) {
    return get(square, square);
  }
  /**
   * This is the only function here, which always checks the coordinates range,
   * regardless the `always_check_range_` value.
   *
   * @param file
   * @param rank
   * @return
   */
  [[nodiscard]] bool test(File file, Rank rank) const {
    auto pos = coordinates_to_index(file, rank, files_);
    assert_field_coordinates(file, rank);
    return fields[pos];
  }

  /**
   * @see{test}
   */
  [[nodiscard]] bool test(Square square) const { return test(square, square); }

  /**
   * Class to support [][] operator for the bitboard.
   */
  class Proxy {
   private:
    bitboard& bb;
    File file;

   public:
    Proxy(bitboard& bitboard, File file) : bb(bitboard), file(file) {}
    bool operator[](Rank rank) const { return bb.get(file, rank); };
  };

  /**
   * Operator for the double [][] to access the board fields.
   */
  Proxy operator[](File file) { return Proxy(*this, file); }

  // to_ulong
  // to_ullong

  /*
bitset& operator&= (const bitset& rhs) noexcept;
bitset& operator|= (const bitset& rhs) noexcept;
bitset& operator^= (const bitset& rhs) noexcept;
bitset& operator<<= (size_t pos) noexcept;
bitset& operator>>= (size_t pos) noexcept;
bitset operator~() const noexcept;
bitset operator<<(size_t pos) const noexcept;
bitset operator>>(size_t pos) const noexcept;
bool operator== (const bitset& rhs) const noexcept;
bool operator!= (const bitset& rhs) const noexcept;

template<size_t N>
  bitset<N> operator& (const bitset<N>& lhs, const bitset<N>& rhs) noexcept;
template<size_t N>
  bitset<N> operator| (const bitset<N>& lhs, const bitset<N>& rhs) noexcept;
template<size_t N>
  bitset<N> operator^ (const bitset<N>& lhs, const bitset<N>& rhs) noexcept;

template<class charT, class traits, size_t N>
  basic_istream<charT, traits>&
    operator>> (basic_istream<charT,traits>& is, bitset<N>& rhs);
template<class charT, class traits, size_t N>
  basic_ostream<charT, traits>&
    operator<< (basic_ostream<charT,traits>& os, const bitset<N>& rhs);
   */

  /**
   * Converts the bitboard to a string.
   *
   * Internally it uses directly the `std::bitset::to_string`.
   *
   * @param empty_character Character to be used when a field is empty.
   * @param set_character Character to be used when a field is set.
   * @return String with bitboard representation.
   */
  [[nodiscard]] std::string to_string(char empty_character = '-',
                                      char set_character = 'x') const noexcept {
    std::basic_string x = fields.to_string(empty_character, set_character);
    return x;
  }

  /**
   * Returns true if all fields are set.
   *
   * @return True if all fields are set, false otherwise.
   */
  [[nodiscard]] bool all() const noexcept { return fields.all(); }

  /**
   * Returns true if any of the fields are set.
   *
   * @return True if any of the fields are set, false otherwise.
   */
  [[nodiscard]] bool none() const noexcept { return fields.none(); }

  /**
   * Returns true if none of the fields are set.
   *
   * @return True if none of the fields are set, false otherwise.
   */
  [[nodiscard]] bool any() const noexcept { return fields.any(); }
};

}  // namespace slchess