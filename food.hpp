#ifndef FOOD_HPP_
#define FOOD_HPP_

#include <cppitertools/itertools.hpp>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Food {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(float deltaTime);
  void createFood(glm::vec2 translation);
  void removeFood();

 private:
  friend OpenGLWindow;

  GLuint m_program{};

  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_scaleLoc{};
  GLint m_translationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  float m_rotation{};
  float m_scale{0.03f};
  int m_sides{4};
  glm::vec2 m_translation{glm::vec2(0)};

  bool m_foodIsAvailable{};
};

#endif