#ifndef SNAKE_HPP_
#define SNAKE_HPP_

//#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Snake {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData, float deltaTime);
  void setRotation(float rotation);
  void incrementSize(float deltaTime);
  void restart();

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};
  GLint m_colorLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  float m_rotation{};
  float m_scale{0.05f};
  int m_sides{13};
  int m_tamanhoMinimo{6};

  bool m_ateFood{};

  glm::vec2 m_velocity{glm::vec2(0)};

  struct Point {
    glm::vec2 m_translation;
    glm::vec3 m_color;
  } __attribute__((aligned(32)));
  std::vector<Point> m_snake;
  std::default_random_engine m_randomEngine;
};

#endif