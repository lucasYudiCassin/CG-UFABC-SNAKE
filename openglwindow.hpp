#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <random>

#include "abcg.hpp"
#include "food.hpp"
#include "map.hpp"
#include "snake.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_starsProgram{};
  GLuint m_objectsProgram{};
  GLuint m_objectsProgramMap{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  GameData m_gameData;
  Snake m_snake;
  Map m_map;
  Food m_food;

  abcg::ElapsedTimer m_restartWaitTimer;
  abcg::ElapsedTimer m_drawWallWaitTimer;
  abcg::ElapsedTimer m_verifyColisionWaitTimer;

  ImFont* m_font{};

  std::default_random_engine m_randomEngine;

  void checkCollisions();
  void checkWinCondition();
  void checkCanDrawFood();
  void checkColisionLoseCondition();
  void checkCanDrawWall();

  void restart();
  void update();
};

#endif