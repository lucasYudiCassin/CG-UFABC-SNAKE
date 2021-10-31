#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    glm::vec2 direction{glm::vec2{mousePosition.x - m_viewportWidth / 2,
                                  mousePosition.y - m_viewportHeight / 2}};
    direction.y = -direction.y;
  }
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  m_wallsProgram = createProgramFromFile(getAssetsPath() + "wall.vert",
                                         getAssetsPath() + "wall.frag");

  abcg::glClearColor(0.2, 0.2, 0.2, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;
  m_gameData.m_score = 0;

  m_snake.initializeGL(m_objectsProgram);
  m_food.initializeGL(m_objectsProgram);
  m_walls.initializeGL(m_wallsProgram);
  m_drawWallWaitTimer.restart();
  m_verifyColisionWaitTimer.restart();
  m_restartWaitTimer.restart();
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Performance workarround for extremely high frames per second (FPS).
  if (deltaTime == 0) return;

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_snake.update(m_gameData, deltaTime);
  m_food.update(deltaTime);

  if (m_gameData.m_state == State::Playing) {
    if (m_verifyColisionWaitTimer.elapsed() > 0.35)
      checkCollisionLoseCondition();
    checkCollisions();
    checkWinCondition();
    checkDrawFood();
    checkDrawWall();
  }
}

void OpenGLWindow::paintGL() {
  update();
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_snake.paintGL(m_gameData);
  m_walls.paintGL(m_gameData);
  m_food.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    const auto size{ImVec2(330, 85)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Fim de jogo");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("Voce Venceu");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_objectsProgram);
  abcg::glDeleteProgram(m_wallsProgram);

  m_snake.terminateGL();
  m_walls.terminateGL();
  m_food.terminateGL();
}

bool OpenGLWindow::canDrawFoodInTranslation(glm::vec2 foodTranslation) {
  for (const auto &wall : m_walls.m_walls) {
    const auto distance{glm::distance(wall.m_translation, foodTranslation)};
    if (distance < m_walls.m_scale + m_food.m_scale) {
      return false;
    }
  }

  for (const auto &snakeTranslation : m_snake.m_snake) {
    const auto distance{
        glm::distance(snakeTranslation.m_translation, foodTranslation)};
    if (distance < m_snake.m_scale + m_food.m_scale) {
      return false;
    }
  }

  return true;
}

bool OpenGLWindow::canDrawWallInTranslation(glm::vec2 wallTranslation) {
  const auto distanceFood{glm::distance(wallTranslation, m_food.m_translation)};
  if (distanceFood < (m_walls.m_scale + m_food.m_scale) * 3) {
    return false;
  }

  for (const auto &snakeTranslation : m_snake.m_snake) {
    const auto distance{
        glm::distance(snakeTranslation.m_translation, wallTranslation)};
    if (distance < (m_snake.m_scale + m_walls.m_scale) * 3) {
      return false;
    }
  }

  return true;
}

void OpenGLWindow::checkDrawWall() {
  if (m_drawWallWaitTimer.elapsed() > 3) {
    std::uniform_real_distribution<float> rd1(-1.0f, 1.0f);
    const glm::vec2 wallTranslation{rd1(m_randomEngine), rd1(m_randomEngine)};

    if (canDrawWallInTranslation(wallTranslation)) {
      m_drawWallWaitTimer.restart();
      m_walls.m_walls.push_back(m_walls.createWall(wallTranslation));
    }
  }
}

void OpenGLWindow::checkDrawFood() {
  if (m_food.m_foodIsAvailable) return;

  std::uniform_real_distribution<float> rd1(-1.0f, 1.0f);
  const glm::vec2 foodTranslation{rd1(m_randomEngine), rd1(m_randomEngine)};

  if (canDrawFoodInTranslation(foodTranslation)) {
    m_food.createFood(foodTranslation);
  }
}

void OpenGLWindow::checkCollisions() {
  // Check collision between snake and food
  if (m_food.m_foodIsAvailable) {
    const auto distance{glm::distance(m_snake.m_snake.front().m_translation,
                                      m_food.m_translation)};

    if (distance < m_snake.m_scale * 0.9f + m_food.m_scale * 0.85f) {
      m_food.m_foodIsAvailable = false;
      m_snake.m_ateFood = true;
      m_gameData.m_score++;
      m_verifyColisionWaitTimer.restart();
    }
  }
}

void OpenGLWindow::checkCollisionLoseCondition() {
  // Collision with walls.
  for (const auto &wall : m_walls.m_walls) {
    const auto distance{glm::distance(wall.m_translation,
                                      m_snake.m_snake.front().m_translation)};
    if (distance < (m_walls.m_scale + m_snake.m_scale) * 1.1) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
      return;
    }
  }
  // TODO: Collision with snake.
  // for (const auto &point : m_snake.m_snake) {
  //   const auto distance{glm::distance(point.m_translation,
  //                                     m_snake.m_snake.front().m_translation)};
  //   if (distance < (m_snake.m_scale * 2)) {
  //     m_gameData.m_state = State::GameOver;
  //     m_restartWaitTimer.restart();
  //     return;
  //   }
  // }
}

void OpenGLWindow::checkWinCondition() {
  if (m_gameData.m_score >= m_gameData.m_scoreLimit) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}
