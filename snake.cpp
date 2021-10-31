#include "snake.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Snake::initializeGL(GLuint program) {
  terminateGL();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_snake.clear();
  m_rotation = 0.0f;
  m_velocity = glm::vec2(0.6);
  std::uniform_real_distribution<float> rdC(0.00f, 1.00f);
  m_snakeColor = {
      glm::vec3{rdC(m_randomEngine), rdC(m_randomEngine), rdC(m_randomEngine)}};
  Point point{
      .m_translation = glm::vec2(0, 0),
      .m_color = m_snakeColor,
  };
  m_snake.emplace_back(point);
  m_ateFood = false;

  std::vector<glm::vec2> position(0);
  position.emplace_back(0, 0);
  const auto step{M_PI * 2 / m_sides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    position.emplace_back(std::cos(angle), std::sin(angle));
  }
  position.push_back(position.at(1));

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(glm::vec2),
                     position.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const auto positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Snake::setRotation(float rotation) { m_rotation = rotation; }

void Snake::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  // Render
  abcg::glBindVertexArray(m_vao);

  for (const auto &point : m_snake) {
    abcg::glUniform4fv(m_colorLoc, 1, &point.m_color.r);
    abcg::glUniform1f(m_rotationLoc, m_rotation);
    abcg::glUniform1f(m_scaleLoc, m_scale);
    abcg::glUniform2fv(m_translationLoc, 1, &point.m_translation.x);

    abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, m_sides + 2);
  }
  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Snake::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Snake::update(const GameData &gameData, float deltaTime) {
  if (gameData.m_input[static_cast<size_t>(Input::Left)])
    m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);

  if (gameData.m_input[static_cast<size_t>(Input::Right)])
    m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);

  // Move forward
  glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
  glm::vec2 increment = m_velocity * forward * deltaTime;
  glm::vec2 newHead = m_snake.at(0).m_translation + increment;

  Point point{
      .m_translation = newHead,
      .m_color = m_snakeColor,
  };

  Point pointExtra{
      .m_translation = m_snake.at(m_snake.size() - 1).m_translation,
      .m_color = m_snakeColor,
  };
  m_snake.insert(m_snake.begin(), pointExtra);
  m_snake.insert(m_snake.begin(), pointExtra);
  m_snake.insert(m_snake.begin(), pointExtra);
  m_snake.insert(m_snake.begin(), point);

  if (m_ateFood) {
    m_ateFood = false;
  } else {
    m_snake.pop_back();
    m_snake.pop_back();
    m_snake.pop_back();
    m_snake.pop_back();
  }

  if (m_snake.at(0).m_translation.x < -1.0f)
    m_snake.at(0).m_translation.x += 2.0f;
  if (m_snake.at(0).m_translation.x > +1.0f)
    m_snake.at(0).m_translation.x -= 2.0f;
  if (m_snake.at(0).m_translation.y < -1.0f)
    m_snake.at(0).m_translation.y += 2.0f;
  if (m_snake.at(0).m_translation.y > +1.0f)
    m_snake.at(0).m_translation.y -= 2.0f;
}