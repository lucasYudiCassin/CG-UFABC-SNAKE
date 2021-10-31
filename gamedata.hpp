#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left };
enum class State { Playing, GameOver, Win };

struct GameData {
  State m_state{State::Playing};
  std::bitset<2> m_input;  // [up, down, left, right]

  int m_score{0};
  int m_scoreLimit{20};
} __attribute__((aligned(32)));

#endif