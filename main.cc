#include <iostream>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL_opengles2.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class renderer;
class controller;

class app {
public:
  app(renderer &renderer, controller &controller)
  : renderer_(renderer)
  , controller_(controller)
  {};
  void init();
  void quit();
  int main();
private:
  renderer &renderer_;
  controller &controller_;
};

class renderer {
public:
  void init();
  void quit();
  void render();
private:
  SDL_Window *window_;
  SDL_GLContext context_;
};

class controller {
public:
  controller() : quit_(false) {};
  void poll();
  bool quit();
private:
  bool quit_;
};

glm::mat4 model_view(float Translate, glm::vec2 const & Rotate) {
  glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
  View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
  View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  return View * Model;
}

glm::mat4 camera(float Translate, glm::vec2 const & Rotate) {
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
  glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
  View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
  View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  return Projection * View * Model;
}

void controller::poll() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE: quit_ = true; break;
        default: break;
      }
    }
  }
}

bool controller::quit() {
  return quit_;
}

int app::main() {
  renderer_.init();
  while (!controller_.quit()) {
    controller_.poll();

    glViewport(0, 0, 320, 240);
    glClearColor(0.2, 0.3, 0.3, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer_.render();
    SDL_Delay(16);
  }
  renderer_.quit();
  return 0;
}

void renderer::init() {
  { // video
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
      fprintf(stderr, "couldn't init ren driver: %s\n", SDL_GetError());
      exit(1);
    }
  }
  { // gl attributes
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  }
  { // create sdl window
    window_ = SDL_CreateWindow("zenu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, SDL_WINDOW_OPENGL);
  }
  { // create sdl gl context
    context_ = SDL_GL_CreateContext(window_);
    if (context_ == nullptr) {
      SDL_Log("SDL_GL_CreateContext(): %s\n", SDL_GetError());
      SDL_DestroyWindow(window_);
      exit(1);
    }
    // vsync
    SDL_GL_SetSwapInterval(1);
    // assign as context to window
    const int status = SDL_GL_MakeCurrent(window_, context_);
    if (status) {
      SDL_Log("SDL_GL_MakeCurrent(): %s\n", SDL_GetError());
      SDL_DestroyWindow(window_);
      exit(1);
    }
  }
}

void renderer::render() {
  SDL_GL_SwapWindow(window_);
}

void renderer::quit() {
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_DestroyWindow(window_);
}

int main() {
  renderer renderer;
  controller controller;
  app app(renderer, controller);
  return app.main();
}
