#include <iostream>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL_opengles2.h>
#include <memory>
#include <cstdlib>
#include <cassert>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STRINGIFY(x) #x

#include "kolor.vert"
#include "kolor.frag"

const float cube_positions[3*24] = {
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
  0.5f, -0.5f,  0.5f,
  0.5f, -0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f,  0.5f,
  0.5f,  0.5f,  0.5f,
  0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,
  0.5f,  0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, 0.5f,
  -0.5f,  0.5f, 0.5f,
  0.5f,  0.5f, 0.5f, 
  0.5f, -0.5f, 0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,
  0.5f, -0.5f,  0.5f,
  0.5f,  0.5f,  0.5f,
  0.5f,  0.5f, -0.5f,
};

const unsigned short int cube_indices[36] = {
  0, 2, 1,
  0, 3, 2, 
  4, 5, 6,
  4, 6, 7,
  8, 9, 10,
  8, 10, 11, 
  12, 15, 14,
  12, 14, 13, 
  16, 17, 18,
  16, 18, 19, 
  20, 23, 22,
  20, 22, 21
};

GLuint cube_vbo;
GLuint cube_ibo;

struct shader_kolor {
    GLuint program;
    GLint a_position;
    GLint u_mvp;
    GLint u_kolor;
};

shader_kolor shader_kolor;

GLuint make_shader(const char *src, GLenum type, int src_len) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, src_len == 0 ? NULL : &src_len);
  glCompileShader(shader);
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    GLchar *info = (GLchar*)calloc(len, sizeof(GLchar));
    glGetShaderInfoLog(shader, len, NULL, info);
    fprintf(stderr, "glCompileShader failed:\n%s\n", info);
    free(info);
    return 0;
  }
  return shader;
}

GLuint make_program(GLuint vert_shader, GLuint frag_shader) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == false) {
    int len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    GLchar *info = (GLchar*)calloc(len, sizeof(GLchar));
    glGetProgramInfoLog(program, len, NULL, info);
    fprintf(stderr, "glLinkProgram failed: %s\n", info);
    free(info);
  }
  glDetachShader(program, vert_shader);
  glDetachShader(program, frag_shader);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);
  return program;
}

void setup_kolor_shader() {
  const GLuint vert = make_shader(kolor_vert_src, GL_VERTEX_SHADER, strlen(kolor_vert_src));
  const GLuint frag = make_shader(kolor_frag_src, GL_FRAGMENT_SHADER, strlen(kolor_frag_src));
  shader_kolor.program = make_program(vert, frag);
  glUseProgram(shader_kolor.program);

  shader_kolor.a_position = glGetAttribLocation(shader_kolor.program, "a_position");
  shader_kolor.u_mvp = glGetUniformLocation(shader_kolor.program, "u_mvp");
  shader_kolor.u_kolor = glGetUniformLocation(shader_kolor.program, "u_kolor");

  glGenBuffers(1, &cube_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  glBufferData(GL_ARRAY_BUFFER, 24 * 3 * sizeof(float), cube_positions, GL_STATIC_DRAW);

  glGenBuffers(1, &cube_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * 3 * sizeof(unsigned short int), cube_indices, GL_STATIC_DRAW);
}

void render_kolor_cube(const glm::mat4 &mvp, glm::vec3 &kolor) {
  const int start_idx = 0;
  const int end_idx = 36 - 1;

  glUseProgram(shader_kolor.program);
  // Set uniforms
  glUniformMatrix4fv(shader_kolor.u_mvp, 1, GL_FALSE, (float*)&mvp);
  glUniform3fv(shader_kolor.u_kolor, 1, (float*)&kolor);
  // Set texture
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  glEnableVertexAttribArray(shader_kolor.a_position);
  glVertexAttribPointer(shader_kolor.a_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
  glDrawElements(GL_TRIANGLES, 1 + end_idx - start_idx, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * start_idx));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(shader_kolor.a_position);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


class renderer;
class controller;
class timer;

class app {
public:
  app(renderer &renderer, controller &controller, timer &timer)
  : renderer_(renderer)
  , controller_(controller)
  , timer_(timer)
  {};
  void init();
  void quit();
  int main();
private:
  renderer &renderer_;
  controller &controller_;
  timer &timer_;
};

class renderer {
public:
  void init();
  void quit();
  void clear();
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

class timer {
public:
  void delay();
};

glm::mat4 model_view(float translate, glm::vec2 const & rotate) {
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -translate));
  view = glm::rotate(view, rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  return view * model;
}

glm::mat4 camera(float translate, glm::vec2 const & rotate) {
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -translate));
  view = glm::rotate(view, rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  return projection * view * model;
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
  setup_kolor_shader();
  while (!controller_.quit()) {
    controller_.poll();
    renderer_.clear();
    {
      float translate = 1.6;
      static glm::vec2 rotate(0.3, 0);
      rotate.x += 0.01;
      rotate.y += 0.03;
      glm::mat4 mvp = camera(translate, rotate);
      glm::vec3 kolor(1.f,1.f,1.f);
      render_kolor_cube(mvp, kolor);
    }
    renderer_.render();
    timer_.delay();
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

void renderer::clear() {
  glViewport(0, 0, 320, 240);
  glClearColor(0.2, 0.3, 0.3, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer::render() {
  SDL_GL_SwapWindow(window_);
}

void renderer::quit() {
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_DestroyWindow(window_);
}

void timer::delay() {
  SDL_Delay(16);
}


int main() {
  renderer renderer;
  controller controller;
  timer timer;
  app app(renderer, controller, timer);
  return app.main();
}
