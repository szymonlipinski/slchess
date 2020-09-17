#include <cstdlib>
#include <bitset>

namespace slchess {

template<size_t files_, size_t ranks_>
class bitboard {
 private:
  std::bitset<files_ * ranks_> fields;
};

}