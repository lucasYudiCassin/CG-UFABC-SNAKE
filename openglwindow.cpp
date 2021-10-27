#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
  }

  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    glm::vec2 direction{glm::vec2{mousePosition.x - m_viewportWidth / 2,
                                  mousePosition.y - m_viewportHeight / 2}};
    direction.y = -direction.y;
    m_snake.setRotation(std::atan2(direction.y, direction.x) - M_PI_2);
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

  m_objectsProgramMap = createProgramFromFile(getAssetsPath() + "map.vert",
                                              getAssetsPath() + "objects.frag");

  m_objectsProgramSnakePoint = createProgramFromFile(
      getAssetsPath() + "snake2.vert", getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

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
  // Teste Point
  // m_snake.initializeGL(m_objectsProgramSnakePoint);

  m_snake.initializeGL(m_objectsProgram);
  m_food.initializeGL(m_objectsProgram);
  m_map.initializeGL(m_objectsProgramMap);
  m_drawWallWaitTimer.restart();
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_snake.update(m_gameData, deltaTime);
  m_food.update(deltaTime);

  if (m_gameData.m_state == State::Playing) {
    if (m_verifyColisionWaitTimer.elapsed() > 0.5) checkColisionLoseCondition();
    checkCollisions();
    checkWinCondition();
    checkCanDrawWall();
    checkCanDrawFood();
  }
}

void OpenGLWindow::paintGL() {
  // fmt::print("OPen: {}\n", m_map.m_teste);
  update();
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_snake.paintGL(m_gameData);
  m_map.paintGL(m_gameData);
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
  abcg::glDeleteProgram(m_starsProgram);
  abcg::glDeleteProgram(m_objectsProgram);
  abcg::glDeleteProgram(m_objectsProgramMap);

  m_snake.terminateGL();
  m_map.terminateGL();
  m_food.terminateGL();
}

void OpenGLWindow::checkCanDrawFood() {
  if (m_food.in_comidaDisponivel) {
    for (int i = 0; i < (static_cast<int>(m_map.m_Wallpositions.size())); i++) {
      const auto distance{
          glm::distance(m_map.m_Wallpositions.at(i), m_food.m_foodPosition)};
      if (distance < m_map.m_scale + m_food.m_foodScale) {
        m_food.in_comidaDisponivel = false;
      }
    }
  }

  if (m_food.in_comidaDisponivel) {
    for (int i = m_snake.m_tamanhoMinimo;
         i < (static_cast<int>(m_snake.m_positions.size())); i++) {
      const auto distance{
          glm::distance(m_snake.m_positions.at(i), m_food.m_foodPosition)};
      if (distance < m_snake.m_scale + m_food.m_foodScale) {
        m_food.in_comidaDisponivel = false;
      }
    }
  }
}

void OpenGLWindow::checkCanDrawWall() {
  if (m_drawWallWaitTimer.elapsed() > 5) {
    std::uniform_real_distribution<float> rd1(-1.0f, 1.0f);
    const glm::vec2 point{rd1(m_randomEngine), rd1(m_randomEngine)};

    const auto distanceFood{glm::distance(point, m_food.m_foodPosition)};
    if (distanceFood < (m_map.m_scale + m_food.m_foodScale) * 2) {
      return;
    }

    for (int i = 0; i < (static_cast<int>(m_snake.m_positions.size())); i++) {
      const auto distance{glm::distance(m_snake.m_positions.at(i), point)};
      if (distance < (m_snake.m_scale + m_map.m_scale) * 2) {
        return;
      }
    }

    m_drawWallWaitTimer.restart();
    m_map.m_Wallpositions.push_back(point);
  }
}
// TODO:
void OpenGLWindow::checkCollisions() {
  // Check collision between snake and food
  if (m_food.in_comidaDisponivel) {
    const auto distance{
        glm::distance(m_snake.m_positions.at(0), m_food.m_foodPosition)};

    if (distance < m_snake.m_scale * 0.9f + m_food.m_foodScale * 0.85f) {
      m_food.in_comidaDisponivel = false;
      m_snake.adicionaPonto();
      m_verifyColisionWaitTimer.restart();
      m_snake.m_point++;
    }
  }
}

void OpenGLWindow::checkColisionLoseCondition() {
  // Walls e snake
  for (int i = 0; i < (static_cast<int>(m_map.m_Wallpositions.size())); i++) {
    const auto distance{
        glm::distance(m_map.m_Wallpositions.at(i), m_snake.m_positions.at(0))};
    if (distance < (m_map.m_scale + m_snake.m_scale) * 1.2) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
      return;
    }
  }
  for (int i = (m_snake.m_tamanhoMinimo * 8);
       i < (static_cast<int>(m_snake.m_positions.size())); i++) {
    const auto distance{
        glm::distance(m_snake.m_positions.at(i), m_snake.m_positions.at(0))};
    if (distance < (m_snake.m_scale * 2)) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
      return;
    }
  }
}
// TODO:
void OpenGLWindow::checkWinCondition() {
  if (m_snake.m_point >= m_snake.m_pointCondition) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}
