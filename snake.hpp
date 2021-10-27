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
  void setupModelPoint();
  void adicionaPontoPoint();
  void Render(glm::vec2 translation);

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};
  // Test Point
  GLint m_pointSizeLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_vboColors{};

  // GLuint m_vboTranslation{};
  // GLuint m_ebo{};

  glm::vec4 m_color{1};
  float m_rotation{};
  float m_scale{0.05f};
  // float m_foodScale{0.03f};
  float m_raio{0.6f * m_scale};
  int m_sides{15};
  int m_tamanhoMinimo{10};
  int m_point{0};
  int m_pointCondition{40};
  // float m_pointSize{100};
  // bool in_comidaDisponivel{};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};
  // glm::vec2 m_foodPosition{0, 0};
  std::vector<glm::vec2> m_positions;
  std::vector<glm::vec3> m_Colors;

  // abcg::ElapsedTimer m_trailBlinkTimer;
  // abcg::ElapsedTimer m_bulletCoolDownTimer;

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

 private:
  void changePosition();
  int quadrante(float rotation);
  // void geraComida();
  // Map m_map;
};

#endif