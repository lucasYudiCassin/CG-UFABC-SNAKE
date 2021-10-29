#ifndef SNAKE_HPP_
#define SNAKE_HPP_

//#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "map.hpp"

class OpenGLWindow;

class Snake {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData, float deltaTime);
  void setRotation(float rotation);
  void setupModel(glm::vec3 color1, glm::vec3 color2, int sides);
  void paintOnePoint(glm::vec3 color1, glm::vec3 color2, glm::vec2 translation,
                     int sides);
  void adicionaPonto();
  void restart();

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_vboColors{};

  float m_rotation{};
  float m_scale{0.05f};
  // float m_foodScale{0.03f};
  // float m_raio{0.6f * m_scale};
  int m_sides{13};
  int m_tamanhoMinimo{6};
  int m_point{0};
  int m_pointCondition{20};

  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};

  std::vector<glm::vec2> m_positions;
  std::vector<glm::vec3> m_Colors;

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

 private:
  void changePosition();
  int quadrante(float rotation);
};

#endif