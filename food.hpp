#ifndef FOOD_HPP_
#define FOOD_HPP_

//#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "map.hpp"

class OpenGLWindow;

class Food {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(float deltaTime);
  void setRotation(float rotation);
  void setupModel(glm::vec3 color1, glm::vec3 color2, int sides);
  void paintOnePoint(glm::vec3 color1, glm::vec3 color2, glm::vec2 translation,
                     float scale, int sides);

  void restart();

  GLuint m_program{};
  GLint m_translationLoc{};

  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_vboColors{};

  float m_rotation{};
  float m_foodScale{0.03f};
  int m_sides{4};
  bool in_comidaDisponivel{};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_foodPosition{0, 0};

  // int m_viewportWidth{};
  // int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

 private:
  void geraComida();
};

#endif