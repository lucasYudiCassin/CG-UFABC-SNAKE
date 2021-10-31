#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <random>

#include "abcg.hpp"
#include "food.hpp"
#include "snake.hpp"
#include "wall.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  friend Walls;
  friend Food;
  friend Snake;

  GLuint m_objectsProgram{};
  GLuint m_wallsProgram{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  GameData m_gameData;
  Snake m_snake;
  Walls m_walls;
  Food m_food;

  abcg::ElapsedTimer m_restartWaitTimer;
  abcg::ElapsedTimer m_drawWallWaitTimer;
  abcg::ElapsedTimer m_verifyColisionWaitTimer;

  ImFont* m_font{};
  ImFont* m_fontScore{};

  std::default_random_engine m_randomEngine;

  void checkCollisions();
  void checkWinCondition();
  void checkCollisionLoseCondition();
  void checkDrawWall();
  void checkDrawFood();

  bool canDrawFoodInTranslation(glm::vec2 foodTranslation);
  bool canDrawWallInTranslation(glm::vec2 wallTranslation);

  void restart();
  void update();
};

#endif