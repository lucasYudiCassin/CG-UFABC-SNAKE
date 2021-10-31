#include "food.hpp"

#include <fmt/core.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Food::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  Food::m_foodIsAvailable = false;
}

void Food::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;
  if (!m_foodIsAvailable) return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);

  abcg::glUniform4f(m_colorLoc, 1, 0, 0, 1);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, m_sides + 2);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Food::update(float deltaTime) {
  // Rotate
  m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);

  // Wrap-around
  if (m_translation.x < -1.0f) m_translation.x += 2.0f;
  if (m_translation.x > +1.0f) m_translation.x -= 2.0f;
  if (m_translation.y < -1.0f) m_translation.y += 2.0f;
  if (m_translation.y > +1.0f) m_translation.y -= 2.0f;
}

void Food::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Food::removeFood() { m_foodIsAvailable = false; }

void Food::createFood(glm::vec2 translation) {
  m_foodIsAvailable = true;
  m_translation = translation;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);

  const auto step{M_PI * 2 / m_sides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}
