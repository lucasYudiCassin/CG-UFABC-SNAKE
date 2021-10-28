#ifndef MAP_HPP_
#define MAP_HPP_

//#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Map {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void setupModel(glm::vec3 color1, glm::vec3 color2, int sides);
  void paintOnePoint(glm::vec3 color1, glm::vec3 color2, glm::vec2 translation,
                     float scale, int sides);

  void restart();

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_vboColors{};

  float m_scale{0.05f};
  int m_sides{6};

  std::vector<glm::vec2> m_Wallpositions;

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;
};

#endif