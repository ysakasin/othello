#include <chrono>
#include <cstdint>
#include <iostream>
#include <list>
#include <vector>
using bitboard = uint64_t;

enum class STATE {
  LOSE = -1,
  DRAW = 0,
  WIN = 1,
};

struct Board {
  size_t depth;
  bitboard dark;
  bitboard light;
};

STATE rev_state(STATE status) {
  const int s = static_cast<int>(status);
  return static_cast<STATE>(-s);
}

std::ostream &operator<<(std::ostream &o, const STATE &state) {
  if (state == STATE::LOSE) {
    o << "LOSE";
  } else if (state == STATE::DRAW) {
    o << "DRAW";
  } else {
    o << "WIN";
  }
  return o;
}

bitboard makeCandidate(bitboard dark, bitboard light) {
  //左右端の番人
  bitboard horizontalWatchBoard = light & 0x7e7e7e7e7e7e7e7e;
  //上下端の番人
  bitboard verticalWatchBoard = light & 0x00FFFFFFFFFFFF00;
  //全辺の番人
  bitboard allSideWatchBoard = light & 0x007e7e7e7e7e7e00;
  //空きマスのみにビットが立っているボード
  bitboard blankBoard = ~(dark | light);
  //隣に相手の色があるかを一時保存する
  bitboard tmp = 0;
  //返り値
  bitboard legalBoard = 0;

  // 8方向チェック (・一度に返せる石は最大6つ ・高速化のためにforを展開)
  //左
  tmp = horizontalWatchBoard & (dark << 1);
  tmp |= horizontalWatchBoard & (tmp << 1);
  tmp |= horizontalWatchBoard & (tmp << 1);
  tmp |= horizontalWatchBoard & (tmp << 1);
  tmp |= horizontalWatchBoard & (tmp << 1);
  tmp |= horizontalWatchBoard & (tmp << 1);
  legalBoard = blankBoard & (tmp << 1);

  //右
  tmp = horizontalWatchBoard & (dark >> 1);
  tmp |= horizontalWatchBoard & (tmp >> 1);
  tmp |= horizontalWatchBoard & (tmp >> 1);
  tmp |= horizontalWatchBoard & (tmp >> 1);
  tmp |= horizontalWatchBoard & (tmp >> 1);
  tmp |= horizontalWatchBoard & (tmp >> 1);
  legalBoard |= blankBoard & (tmp >> 1);

  //上
  tmp = verticalWatchBoard & (dark << 8);
  tmp |= verticalWatchBoard & (tmp << 8);
  tmp |= verticalWatchBoard & (tmp << 8);
  tmp |= verticalWatchBoard & (tmp << 8);
  tmp |= verticalWatchBoard & (tmp << 8);
  tmp |= verticalWatchBoard & (tmp << 8);
  legalBoard |= blankBoard & (tmp << 8);

  //下
  tmp = verticalWatchBoard & (dark >> 8);
  tmp |= verticalWatchBoard & (tmp >> 8);
  tmp |= verticalWatchBoard & (tmp >> 8);
  tmp |= verticalWatchBoard & (tmp >> 8);
  tmp |= verticalWatchBoard & (tmp >> 8);
  tmp |= verticalWatchBoard & (tmp >> 8);
  legalBoard |= blankBoard & (tmp >> 8);

  //右斜め上
  tmp = allSideWatchBoard & (dark << 7);
  tmp |= allSideWatchBoard & (tmp << 7);
  tmp |= allSideWatchBoard & (tmp << 7);
  tmp |= allSideWatchBoard & (tmp << 7);
  tmp |= allSideWatchBoard & (tmp << 7);
  tmp |= allSideWatchBoard & (tmp << 7);
  legalBoard |= blankBoard & (tmp << 7);

  //左斜め上
  tmp = allSideWatchBoard & (dark << 9);
  tmp |= allSideWatchBoard & (tmp << 9);
  tmp |= allSideWatchBoard & (tmp << 9);
  tmp |= allSideWatchBoard & (tmp << 9);
  tmp |= allSideWatchBoard & (tmp << 9);
  tmp |= allSideWatchBoard & (tmp << 9);
  legalBoard |= blankBoard & (tmp << 9);

  //右斜め下
  tmp = allSideWatchBoard & (dark >> 9);
  tmp |= allSideWatchBoard & (tmp >> 9);
  tmp |= allSideWatchBoard & (tmp >> 9);
  tmp |= allSideWatchBoard & (tmp >> 9);
  tmp |= allSideWatchBoard & (tmp >> 9);
  tmp |= allSideWatchBoard & (tmp >> 9);
  legalBoard |= blankBoard & (tmp >> 9);

  //左斜め下
  tmp = allSideWatchBoard & (dark >> 7);
  tmp |= allSideWatchBoard & (tmp >> 7);
  tmp |= allSideWatchBoard & (tmp >> 7);
  tmp |= allSideWatchBoard & (tmp >> 7);
  tmp |= allSideWatchBoard & (tmp >> 7);
  tmp |= allSideWatchBoard & (tmp >> 7);
  legalBoard |= blankBoard & (tmp >> 7);

  return legalBoard;
}

// ビットを数える
size_t popcnt(const bitboard &v) {
  bitboard count = (v & 0x5555555555555555) + ((v >> 1) & 0x5555555555555555);
  count = (count & 0x3333333333333333) + ((count >> 2) & 0x3333333333333333);
  count = (count & 0x0f0f0f0f0f0f0f0f) + ((count >> 4) & 0x0f0f0f0f0f0f0f0f);
  count = (count & 0x00ff00ff00ff00ff) + ((count >> 8) & 0x00ff00ff00ff00ff);
  count = (count & 0x0000ffff0000ffff) + ((count >> 16) & 0x0000ffff0000ffff);
  return (count & 0x00000000ffffffff) + ((count >> 32) & 0x00000000ffffffff);
}

STATE juge(bitboard dark, bitboard light) {
  size_t dscore = popcnt(dark);
  size_t lscore = popcnt(light);
  if (dscore == lscore) {
    return STATE::DRAW;
  } else if (dscore > lscore) {
    return STATE::WIN;
  } else {
    return STATE::LOSE;
  }
}

void print8x8(uint64_t dark, uint64_t light, bool rev = false) {
  if (rev) {
    uint64_t tmp = dark;
    dark = light;
    light = tmp;
  }
  for (int i = 0; i < 8; i++) {
    for (int p = 0; p < 8; p++) {
      if (dark & 1UL) {
        std::cout << "◯";
      } else if (light & 1UL) {
        std::cout << "●";
      } else {
        std::cout << ".";
      }
      dark = dark >> 1;
      light = light >> 1;
    }
    std::cout << std::endl;
  }
  std::cout << "========" << std::endl;
}

void print6x6(uint64_t dark, uint64_t light, bool rev = false) {
  if (rev) {
    uint64_t tmp = dark;
    dark = light;
    light = tmp;
  }
  for (int i = 0; i < 6; i++) {
    for (int p = 0; p < 6; p++) {
      if (dark & 1UL) {
        std::cout << "◯";
      } else if (light & 1UL) {
        std::cout << "●";
      } else {
        std::cout << ".";
      }
      dark = dark >> 1;
      light = light >> 1;
    }
    dark = dark >> 2;
    light = light >> 2;
    std::cout << std::endl;
  }
  std::cout << "========" << std::endl;
}

void print4x4(uint64_t dark, uint64_t light, bool rev = false) {
  if (rev) {
    uint64_t tmp = dark;
    dark = light;
    light = tmp;
  }
  for (int i = 0; i < 4; i++) {
    for (int p = 0; p < 4; p++) {
      if (dark & 1UL) {
        std::cout << "◯";
      } else if (light & 1UL) {
        std::cout << "●";
      } else {
        std::cout << ".";
      }
      dark = dark >> 1;
      light = light >> 1;
    }
    dark = dark >> 4;
    light = light >> 4;
    std::cout << std::endl;
  }
  std::cout << "========" << std::endl;
}

bitboard transfer(const bitboard move, const int k) {
  switch (k) {
  case 0: //上
    return (move << 8) & 0xffffffffffffff00;
  case 1: //右上
    return (move << 7) & 0x7f7f7f7f7f7f7f00;
  case 2: //右
    return (move >> 1) & 0x7f7f7f7f7f7f7f7f;
  case 3: //右下
    return (move >> 9) & 0x007f7f7f7f7f7f7f;
  case 4: //下
    return (move >> 8) & 0x00ffffffffffffff;
  case 5: //左下
    return (move >> 7) & 0x00fefefefefefefe;
  case 6: //左
    return (move << 1) & 0xfefefefefefefefe;
  case 7: //左上
    return (move << 9) & 0xfefefefefefefe00;
  }
  return 0;
}

bitboard getRevPat(const bitboard &black, const bitboard &white,
                   const bitboard move) {
  if ((black | white) & move)
    return 0;
  bitboard rev = 0;
  for (int k = 0; k < 8; ++k) {
    bitboard rev_ = 0;
    bitboard mask = transfer(move, k);
    while (mask && (mask & white)) {
      rev_ |= mask;
      mask = transfer(mask, k);
    }
    if (mask & black)
      rev |= rev_;
  }
  return rev;
}

static uint64_t cnt = 0;
static uint64_t cnt_totyu = 0;

static std::chrono::high_resolution_clock::time_point begin_at;
static std::chrono::high_resolution_clock::time_point end_at;

bitboard mask4x4 = 0x000000000f0f0f0f;
bitboard mask4x6 = 0x000000003f3f3f3f;
bitboard mask4x8 = 0x00000000ffffffff;
bitboard mask6x6 = 0x00003f3f3f3f3f3f;

STATE negamax(bitboard dark, bitboard light, bool pass = false) {
  if (~(dark | light | mask4x8) == 0) {
    // 終局
    cnt++;
    if (cnt % 10000000ULL == 0) {
      std::cout << cnt << std::endl;
    }
    return juge(dark, light);
  }

  STATE max = STATE::LOSE;
  bitboard space = makeCandidate(dark, light) & mask4x8;
  if (space == 0) {
    if (pass) {
      cnt_totyu++;
      cnt++;
      if (cnt % 10000000ULL == 0) {
        std::cout << cnt << std::endl;
      }
      return juge(dark, light);
    }

    return rev_state(negamax(light, dark, true));
  }

  while (space != 0) {
    bitboard move = space & (~space + 1);
    bitboard rev = getRevPat(dark, light, move);

    STATE score = rev_state(negamax(light ^ rev, dark ^ (move | rev)));
    if (score == STATE::WIN) {
      return STATE::WIN;
    } else if (score == STATE::DRAW) {
      max = STATE::DRAW;
    }

    space ^= move;
  }
  return max;
}

static std::list<Board> board_queue;

std::pair<bitboard, bitboard> initial_board_8x8 = {(1UL << 28) | (1UL << 35), (1UL << 27) | (1UL << 36)};
std::pair<bitboard, bitboard> initial_board_6x6 = {(1UL << 19) | (1UL << 26), (1UL << 18) | (1UL << 27)};
std::pair<bitboard, bitboard> initial_board_4x4 = {(1UL << 10) | (1UL << 17), (1UL << 9) | (1UL << 18)};
std::pair<bitboard, bitboard> initial_board_4x6 = {(1UL << 11) | (1UL << 18), (1UL << 10) | (1UL << 19)};
std::pair<bitboard, bitboard> initial_board_4x8 = {(1UL << 12) | (1UL << 19), (1UL << 11) | (1UL << 20)};

int main() {

  // print8x8(initial_board_8x8.first, initial_board_8x8.second);
  // print6x6(initial_board_6x6.first, initial_board_6x6.second);
  // print4x4(initial_board_4x4.first, initial_board_4x4.second);
  // print8x8(initial_board_4x6.first, initial_board_4x6.second);

  bitboard dark = initial_board_4x8.first;
  bitboard light = initial_board_4x8.second;

  bitboard candidate = makeCandidate(dark, light);
  print8x8(candidate, 0);
  print8x8(mask4x6, 0);

  // initial board
  // uint64_t dark = (1UL << 28) | (1UL << 35);
  // uint64_t light = (1UL << 27) | (1UL << 36);

  { // first step
    uint64_t space = makeCandidate(dark, light);
    // print8x8(space, 0);
    bitboard move = space & (~space + 1);
    bitboard rev = getRevPat(dark, light, move);

    dark = dark ^ (move | rev);
    light = light ^ rev;
    print8x8(dark, light);
  }

  std::cout << rev_state(negamax(light, dark)) << std::endl;
  std::cout << cnt << std::endl;

  return 0;
}
