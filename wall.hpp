#ifndef WALL_HPP_
#define WALL_HPP_

//#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Walls {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void restart();

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  struct Wall {
    GLuint m_vao{};
    GLuint m_vbo{};

    int m_polygonSides{};

    glm::vec2 m_translation{glm::vec2(0)};
  } __attribute__((aligned(32)));

  float m_scale{0.05f};

  std::list<Wall> m_walls;

  std::default_random_engine m_randomEngine;

  Walls::Wall createWall(glm::vec2 translation = glm::vec2(0));
};

#endif