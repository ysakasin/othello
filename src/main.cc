#include <chrono>
#include <cstdint>
#include <iostream>
using bitboard = uint64_t;

enum class STATE {
  LOSE = -1,
  DRAW = 0,
  WIN = 1,
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

bitboard makeLegalBoard(bitboard dark, bitboard light) {
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

// must `~(dark | light) == 0`
STATE juge_finished_board(bitboard dark) {
  int score = popcnt(dark);
  if (score > 8 * 8 / 2) {
    return STATE::WIN;
  } else if (score == 8 * 8 / 2) {
    return STATE::DRAW;
  } else {
    return STATE::LOSE;
  }
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

STATE negamax(bitboard dark, bitboard light, bool pass = false) {
  if (~(dark | light) == 0) {
    // 終局
    cnt++;
    if (cnt >= 10000000ULL) {
      end_at = std::chrono::high_resolution_clock::now();
      size_t millisec = std::chrono::duration_cast<std::chrono::milliseconds>(
                            end_at - begin_at)
                            .count();
      std::cout << cnt << std::endl;
      std::cout << millisec << " ms" << std::endl;
      exit(1);
    }
    return juge_finished_board(dark);
  }

  STATE max = STATE::LOSE;
  bitboard space = makeLegalBoard(dark, light);
  if (space == 0) {
    if (pass) {
      cnt_totyu++;
      cnt++;
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

int main() {
  // initial board
  uint64_t dark = (1UL << 28) | (1UL << 35);
  uint64_t light = (1UL << 27) | (1UL << 36);
  // print8x8(dark, light);

  { // first step
    uint64_t space = makeLegalBoard(dark, light);
    // print8x8(space, 0);
    bitboard move = space & (~space + 1);
    bitboard rev = getRevPat(dark, light, move);

    dark = dark ^ (move | rev);
    light = light ^ rev;
    // print8x8(dark, light);
  }

  begin_at = std::chrono::high_resolution_clock::now();
  std::cout << rev_state(negamax(light, dark)) << std::endl;
  std::cout << cnt << std::endl;
  std::cout << cnt_totyu << std::endl;
  return 0;
}
