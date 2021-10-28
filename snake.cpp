#include "snake.hpp"

#include <fmt/core.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Snake::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");
  // Test Point
  // m_pointSizeLoc = abcg::glGetUniformLocation(m_program, "pointSize");

  restart();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());
}
void Snake::restart() {
  m_positions.clear();
  m_Colors.clear();
  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0.6);
  m_positions.emplace_back(0, 0);
  // m_Colors.emplace_back(1, 1, 1);
  for (int i = 0; i < 10; i++) adicionaPonto();
  // Snake::in_comidaDisponivel = false;

  m_point = 0;
}
void Snake::setRotation(float rotation) { m_rotation = rotation; }

void Snake::paintGL(const GameData &gameData) {
  // fmt::print("Snake: {}\n", m_map.m_teste);
  if (gameData.m_state != State::Playing) return;
  // geraComida();
  paintOnePoint(m_Colors.at(0), m_Colors.at(0), m_positions[0], m_sides);

  for (int i = 1; i < static_cast<int>(m_positions.size()); i++) {
    paintOnePoint(m_Colors.at(i - 1), m_Colors.at(i - 1), m_positions[i],
                  m_sides);
  }
}

void Snake::paintOnePoint(glm::vec3 color1, glm::vec3 color2,
                          glm::vec2 translation, int sides) {
  // Test Point
  // setupModelPoint();
  setupModel(color1, color2, sides);

  abcg::glUseProgram(m_program);
  abcg::glEnable(GL_DEBUG_OUTPUT);
  // Choose a random xy position from (-1,-1) to (1,1)

  abcg::glUniform2fv(m_translationLoc, 1, &translation.x);

  // abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  // Test Point
  // abcg::glUniform1f(m_pointSizeLoc, m_pointSizeLoc);

  // Render
  abcg::glBindVertexArray(m_vao);
  // Test Point
  // abcg::glDrawArrays(GL_POINTS, 0, m_positions.size());

  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, m_sides + 2);
  // abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, m_sides + 2);
  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Snake::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Snake::update(const GameData &gameData, float deltaTime) {
  // Rotate
  /**
  if (m_rotation >= (M_PI * 2)) m_rotation = 0.0f;
  if (gameData.m_input[static_cast<size_t>(Input::Left)]) {
    if (quadrante(m_rotation) == 2) {
      m_rotation = std::min(glm::wrapAngle(m_rotation + 4.0f * deltaTime),
                            static_cast<float>(M_PI / 2));
    }
    if (quadrante(m_rotation) == 1) {
      m_rotation = std::max(glm::wrapAngle(m_rotation - 4.0f * deltaTime),
                            static_cast<float>(M_PI / 2));
    }
  }

  if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
    if (quadrante(m_rotation) == 4) {
      m_rotation = std::min(glm::wrapAngle(m_rotation + 4.0f * deltaTime),
                            static_cast<float>(0.0f));
    }
    if (quadrante(m_rotation) == 1) {
      m_rotation = std::max(glm::wrapAngle(m_rotation - 4.0f * deltaTime),
                            static_cast<float>(0.0f));
    }
  }*/

  if (gameData.m_input[static_cast<size_t>(Input::Left)])
    m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);

  if (gameData.m_input[static_cast<size_t>(Input::Right)])
    m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);

  glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);

  glm::vec2 increment = m_velocity * forward * deltaTime;
  changePosition();

  m_positions.insert(m_positions.begin(), m_positions.at(0) + increment);

  // Wrap-around
  if (m_positions.at(0).x < -1.0f) m_positions.at(0).x += 2.0f;
  if (m_positions.at(0).x > +1.0f) m_positions.at(0).x -= 2.0f;
  if (m_positions.at(0).y < -1.0f) m_positions.at(0).y += 2.0f;
  if (m_positions.at(0).y > +1.0f) m_positions.at(0).y -= 2.0f;
}
int Snake::quadrante(float rotation) {
  if (rotation >= (M_PI * 2)) rotation = 0.0f;
  if (rotation >= 0.0f && rotation <= (M_PI / 2)) return 1;
  if (rotation >= (M_PI / 2) && rotation <= (M_PI)) return 2;
  if (rotation >= M_PI && rotation <= (M_PI * 3 / 2)) return 3;
  if (rotation >= (M_PI * 3 / 2)) return 4;
  return -1;
}

void Snake::adicionaPonto() {
  std::uniform_real_distribution<float> rdC(0.00f, 1.00f);

  glm::vec3 cor = {
      glm::vec3{rdC(m_randomEngine), rdC(m_randomEngine), rdC(m_randomEngine)}};

  for (int i = 0; i < m_tamanhoMinimo; i++) {
    glm::vec2 lastPoint = m_positions.at(m_positions.size() - 1);
    m_positions.push_back(lastPoint + m_raio);
    m_Colors.push_back(cor);
  }
}

void Snake::changePosition() {
  if (static_cast<int>(m_positions.size()) > 1) {
    m_positions.pop_back();
  }
}

void Snake::setupModel(glm::vec3 color1, glm::vec3 color2, int sides) {
  // Release previous resources, if any
  terminateGL();

  std::vector<glm::vec2> positions(0);
  std::vector<glm::vec3> colors(0);

  // Polygon center
  positions.emplace_back(0, 0);
  colors.push_back(color1);

  // Border vertices
  // fmt::print("--------------- Posicao inicial: {},{}\n", positions.at(0).x,
  // positions.at(0).y);
  const auto step{M_PI * 2 / sides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    // auto distance{glm::distance(positions.at(0),
    // positions.back())}; fmt::print("Posicao Adicionada: {},{}
    // Dist: {}\n", std::cos(angle),
    //          std::sin(angle), distance);
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