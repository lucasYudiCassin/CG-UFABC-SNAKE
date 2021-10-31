project(CG-UFABC-SNAKE)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp snake.cpp wall.cpp food.cpp)

enable_abcg(${PROJECT_NAME})
