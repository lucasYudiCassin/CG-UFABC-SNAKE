#include "wall.hpp"

#include <fmt/core.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Walls::initializeGL(GLuint program) {
  terminateGL();

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_walls.clear();
  m_walls.resize(0);
}

void Walls::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  for (const auto &wall : m_walls) {
    abcg::glBindVertexArray(wall.m_vao);

    abcg::glUniform4f(m_colorLoc, 0, 0.5, 1, 1);
    abcg::glUniform1f(m_scaleLoc, m_scale);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        abcg::glUniform2f(m_translationLoc, wall.m_translation.x + j,
                          wall.m_translation.y + i);

        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, wall.m_polygonSides + 2);
      }
    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Walls::terminateGL() {
  for (auto wall : m_walls) {
    abcg::glDeleteBuffers(1, &wall.m_vbo);
    abcg::glDeleteVertexArrays(1, &wall.m_vao);
  }
}

Walls::Wall Walls::createWall(glm::vec2 translation) {
  Wall wall;

  auto &re{m_randomEngine};  // Shortcut

  // Randomly choose the number of sides
  std::uniform_int_distribution<int> randomSides(6, 10);
  wall.m_polygonSides = randomSides(re);
  wall.m_translation = translation;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);

  const auto step{M_PI * 2 / wall.m_polygonSides};
  std::uniform_real_distribution<float> randomRadius(0.8f, 1.0f);
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    const auto radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &wall.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, wall.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &wall.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(wall.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, wall.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return wall;
}