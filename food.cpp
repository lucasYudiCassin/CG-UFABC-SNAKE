#include "food.hpp"

#include <fmt/core.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Food::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  // m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");
  restart();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());
}
void Food::restart() {
  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  Food::in_comidaDisponivel = false;
}
void Food::setRotation(float rotation) { m_rotation = rotation; }

void Food::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;
  geraComida();
}

void Food::update(float deltaTime) {
  // Rotate
  m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
}

void Food::geraComida() {
  const glm::vec3 color1{0, 1, 0};
  if (Food::in_comidaDisponivel) {
    paintOnePoint(color1, color1, m_foodPosition, m_foodScale, 4);
  } else {
    std::uniform_real_distribution<float> rd1(-1.0f, 1.0f);
    const glm::vec2 point{rd1(m_randomEngine), rd1(m_randomEngine)};

    m_foodPosition = point;
    in_comidaDisponivel = true;
  }
}

void Food::paintOnePoint(glm::vec3 color1, glm::vec3 color2,
                         glm::vec2 translation, float scale, int sides) {
  setupModel(color1, color2, sides);
  // Create a regular polygon with a number of sides in the range [3,20]

  abcg::glUseProgram(m_program);

  // Choose a random xy position from (-1,-1) to (1,1)
  abcg::glUniform2fv(m_translationLoc, 1, &translation.x);
  // abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glUniform1f(m_scaleLoc, scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);

  // Render
  abcg::glBindVertexArray(m_vao);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, m_sides + 2);
  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Food::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Food::setupModel(glm::vec3 color1, glm::vec3 color2, int sides) {
  // Release previous resources, if any
  terminateGL();

  std::vector<glm::vec2> positions(0);
  std::vector<glm::vec3> colors(0);

  // Polygon center
  positions.emplace_back(0, 0);
  colors.push_back(color1);

  // Border vertices

  const auto step{M_PI * 2 / sides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));

    colors.push_back(color2);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(1));
  colors.push_back(color2);

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const auto positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  const auto colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}