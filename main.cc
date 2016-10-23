#include <iostream>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL_opengles2.h>
#include <memory>
#include <cstdlib>
#include <cassert>

#include <ft2build.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STRINGIFY(x) #x

#include "kolor.vert"
#include "kolor.frag"

///

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

const size_t cube_positions_size = 3 * 24;

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

const size_t cube_indices_size = 36;

///

typedef GLuint shader_id;
typedef GLuint program_id;
typedef GLuint vbo;
typedef GLuint ibo;

struct mesh {
  GLuint vbo;
  GLuint ibo;
};

struct source {
  const GLchar *content;
  GLint length;
};

struct shaders {
  shader_id vert;
  shader_id frag;
};

struct mesh_kolor {
  GLuint vbo;
  GLuint ibo;
  int start_index;
  int end_index;
  size_t stride;
};

struct uniforms_kolor {
  glm::mat4 &mvp;
  glm::vec3 &kolor;
};

struct ref_kolor {
  GLuint program;
  GLint a_position;
  GLint u_mvp;
  GLint u_kolor;
};

///

struct {
    GLuint program;
    GLint a_position;
    GLint u_mvp;
    GLint u_kolor;
} shader_kolor;

mesh cube;

///

class renderer;
class controller;
class timer;
class shader_manager;
class program_manager;
class loader;

///

class app {
public:
  app(renderer &renderer, controller &controller, timer &timer, loader &loader)
  : renderer_(renderer)
  , controller_(controller)
  , timer_(timer)
  , loader_(loader)
  {};
  void init();
  void quit();
  int main();
private:
  renderer &renderer_;
  controller &controller_;
  timer &timer_;
  loader &loader_;
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
  controller()
  : quit_(false)
  {};
  void poll();
  bool quit();
private:
  bool quit_;
};

class timer {
public:
  void delay();
};

class shader_manager {
public:
  shader_id create(const source &source, GLenum type);
  void destroy(shader_id shader_id);
};

class program_manager {
public:
  program_id create(shader_id vert_shader, shader_id frag_shader);
  void destroy(program_id program_id);
};

class mesh_manager {
public:
  mesh create(GLsizeiptr vertex_size, const GLvoid *vertices, GLsizeiptr index_size, const GLvoid *indices);
};

class loader {
public:
  loader(program_manager &program_manager, shader_manager &shader_manager, mesh_manager &mesh_manager)
  : shader_manager_(shader_manager)
  , program_manager_(program_manager)
  , mesh_manager_(mesh_manager)
  {}
  void load();
private:
  shader_manager shader_manager_;
  program_manager program_manager_;
  mesh_manager mesh_manager_;
};

//

program_id make_program(shader_manager &shader_manager, program_manager &program_manager, const source &vert_source, const source &frag_source);
void render_kolor(const ref_kolor &ref, const mesh_kolor &mesh, const uniforms_kolor &uni);
glm::mat4 camera(float translate, glm::vec2 const & rotate);

//

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
  loader_.load();
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

      const ref_kolor ref = {
        .program = shader_kolor.program,
        .a_position = shader_kolor.a_position,
        .u_mvp = shader_kolor.u_mvp,
        .u_kolor = shader_kolor.u_kolor,
      };

      const mesh_kolor mesh = {
        .vbo = cube.vbo,
        .ibo = cube.ibo,
        .start_index = 0,
        .end_index = 36 - 1,
        .stride = 3 * sizeof(float),
      };

      const uniforms_kolor uni = {
        .mvp = mvp,
        .kolor = kolor,
      };

      render_kolor(ref, mesh, uni);
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

shader_id shader_manager::create(const source &source, GLenum type) {
  shader_id shader_id = glCreateShader(type);
  glShaderSource(shader_id, 1, &source.content, source.length == 0 ? NULL : &source.length);
  glCompileShader(shader_id);
  GLint status;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint len;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
    GLchar *info = (GLchar*)calloc(len, sizeof(GLchar));
    glGetShaderInfoLog(shader_id, len, NULL, info);
    fprintf(stderr, "glCompileShader failed:\n%s\n", info);
    free(info);
    return 0;
  }
  return shader_id;
}

void shader_manager::destroy(shader_id shader_id) {
  glDeleteShader(shader_id);
}

program_id program_manager::create(shader_id vert_shader, shader_id frag_shader) {
  program_id program_id = glCreateProgram();
  glAttachShader(program_id, vert_shader);
  glAttachShader(program_id, frag_shader);
  glLinkProgram(program_id);
  GLint status;
  glGetProgramiv(program_id, GL_LINK_STATUS, &status);
  if (status == false) {
    int len;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
    GLchar *info = (GLchar*)calloc(len, sizeof(GLchar));
    glGetProgramInfoLog(program_id, len, NULL, info);
    fprintf(stderr, "glLinkProgram failed: %s\n", info);
    free(info);
  }
  glDetachShader(program_id, vert_shader);
  glDetachShader(program_id, frag_shader);
  return program_id;
}

void program_manager::destroy(program_id program_id) {
  glDeleteProgram(program_id);
}

mesh mesh_manager::create(GLsizeiptr vertex_size, const GLvoid *vertices, GLsizeiptr index_size, const GLvoid *indices) {
  mesh mesh;
  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_size, vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &mesh.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, indices, GL_STATIC_DRAW);
  return mesh;
}

void loader::load() {
  const source vertex_source = {
    .content = kolor_vert_src,
    .length = (GLint)strlen(kolor_vert_src),
  };
  const source fragment_source = {
    .content = kolor_frag_src,
    .length = (GLint)strlen(kolor_frag_src),
  };
  const program_id program_id = make_program(shader_manager_, program_manager_, vertex_source, fragment_source);
  glUseProgram(program_id);

  shader_kolor.program = program_id;
  shader_kolor.a_position = glGetAttribLocation(shader_kolor.program, "a_position");
  shader_kolor.u_mvp = glGetUniformLocation(shader_kolor.program, "u_mvp");
  shader_kolor.u_kolor = glGetUniformLocation(shader_kolor.program, "u_kolor");

  cube = mesh_manager_.create(cube_positions_size * sizeof(float), cube_positions, cube_indices_size * sizeof(unsigned short int), cube_indices);
}

int main() {
  renderer renderer;
  controller controller;
  timer timer;
  program_manager program_manager;
  shader_manager shader_manager;
  mesh_manager mesh_manager;
  loader loader(program_manager, shader_manager, mesh_manager);
  app app(renderer, controller, timer, loader);
  return app.main();
}

void render_kolor(const ref_kolor &ref, const mesh_kolor &mesh, const uniforms_kolor &uni) {
  glUseProgram(ref.program);
  
  glUniformMatrix4fv(ref.u_mvp, 1, GL_FALSE, (float*)&uni.mvp);
  glUniform3fv(ref.u_kolor, 1, (float*)&uni.kolor);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glEnableVertexAttribArray(ref.a_position);
  glVertexAttribPointer(ref.a_position, 3, GL_FLOAT, GL_FALSE, mesh.stride, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
  glDrawElements(GL_TRIANGLES, 1 + mesh.end_index - mesh.start_index, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * mesh.start_index));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(ref.a_position);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

program_id make_program(shader_manager &shader_manager, program_manager &program_manager, const source &vert_source, const source &frag_source) {
  const shader_id vert = shader_manager.create(vert_source, GL_VERTEX_SHADER);
  const shader_id frag = shader_manager.create(frag_source, GL_FRAGMENT_SHADER);
  const program_id program_id = program_manager.create(vert, frag);
  shader_manager.destroy(vert);
  shader_manager.destroy(frag);
  return program_id;
}


glm::mat4 camera(float translate, glm::vec2 const & rotate) {
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -translate));
  view = glm::rotate(view, rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  return projection * view * model;
}
