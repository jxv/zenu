#include <iostream>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL_opengles2.h>
#include <memory>
#include <cstdlib>
#include <cassert>

#include <ft2build.h>
#include <freetype/freetype.h>

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

struct source {
  const GLchar *content;
  GLint length;
};

struct shaders {
  shader_id vert;
  shader_id frag;
};

struct mesh {
  GLuint vbo;
  GLuint ibo;
  size_t stride;
};

struct frame {
  int start_index;
  int end_index;
};

struct consts_kolor {
  glm::mat4 &mvp;
  glm::vec3 &kolor;
};

struct ref_kolor {
  GLuint program;
  GLint a_position;
  GLint u_mvp;
  GLint u_kolor;
};

//

class renderer;
class controller;
class timer;
class shader_manager;
class program_manager;
class compiler;
class render_object;
class compiler;
class mesh_manager;
class ref_manager;
class font_manager;

///

class app {
public:
  app(renderer &renderer, controller &controller, timer &timer, compiler &compiler, mesh_manager &mesh_manager, ref_manager &ref_manager, font_manager &font_manager)
  : renderer_(renderer)
  , controller_(controller)
  , timer_(timer)
  , compiler_(compiler)
  , mesh_manager_(mesh_manager)
  , ref_manager_(ref_manager)
  , font_manager_(font_manager)
  {};
  int main();
private:
  renderer &renderer_;
  controller &controller_;
  timer &timer_;
  compiler &compiler_;
  mesh_manager &mesh_manager_;
  ref_manager &ref_manager_;
  font_manager &font_manager_;

  void load();
  void render();

  mesh mesh_;
  ref_kolor ref_;
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
  mesh create(GLsizeiptr vertex_size, const GLvoid *vertices, GLsizeiptr index_size, const GLvoid *indices, size_t stride);
  void destroy(mesh &mesh);
};

class ref_manager {
public:
  ref_kolor create_kolor(program_id program_id);
};

class compiler {
public:
  compiler(program_manager &program_manager, shader_manager &shader_manager)
  : shader_manager_(shader_manager)
  , program_manager_(program_manager)
  {}
  program_id compile(const source &vert_source, const source &frag_source);
private:
  shader_manager shader_manager_;
  program_manager program_manager_;
};

class render_object {
public:
  render_object(const ref_kolor &ref, const mesh &mesh, const frame &frame, const consts_kolor &consts)
  : ref_(ref)
  , mesh_(mesh)
  , frame_(frame)
  , consts_(consts)
  {};
  void render();
private:
  const ref_kolor &ref_;
  const mesh &mesh_;
  const frame &frame_;
  const consts_kolor &consts_;
};

class font_manager {
public:
  void init();
private:
  FT_Library library_;
};

//

glm::mat4 camera(float translate, glm::vec2 const & rotate);

//

int main() {
  renderer renderer;
  controller controller;
  timer timer;
  program_manager program_manager;
  shader_manager shader_manager;
  compiler compiler(program_manager, shader_manager);
  mesh_manager mesh_manager;
  ref_manager ref_manager;
  font_manager font_manager;
  app app(renderer, controller, timer, compiler, mesh_manager, ref_manager, font_manager);
  return app.main();
}

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

source make_source_from_content(const char *content) {
  return {
    .content = content,
    .length = (GLint)strlen(content),
  };
}

ref_kolor ref_manager::create_kolor(program_id program_id) {
  glUseProgram(program_id);
  return {
    .program = program_id,
    .a_position = glGetAttribLocation(program_id, "a_position"),
    .u_mvp = glGetUniformLocation(program_id, "u_mvp"),
    .u_kolor = glGetUniformLocation(program_id, "u_kolor"),
  };
}

void app::load() {
  const source vertex_source = make_source_from_content(kolor_vert_src);
  const source fragment_source = make_source_from_content(kolor_frag_src);
  const program_id program_id = compiler_.compile(vertex_source, fragment_source);
  ref_ = ref_manager_.create_kolor(program_id);
  mesh_ = mesh_manager_.create(cube_positions_size * sizeof(float), cube_positions, cube_indices_size * sizeof(unsigned short int), cube_indices, 3 * sizeof(float));

}

void app::render()
{
  float translate = 1.6;
  static glm::vec2 rotate(0.3, 0);
  rotate.x += 0.01;
  rotate.y += 0.03;
  glm::mat4 mvp = camera(translate, rotate);
  glm::vec3 kolor(1.f,1.f,1.f);

  const frame frame = {
    .start_index = 0,
    .end_index = 36 - 1,
  };

  const consts_kolor consts = {
    .mvp = mvp,
    .kolor = kolor,
  };

  render_object render_kolor(ref_, mesh_, frame, consts);
  render_kolor.render();
}

int app::main() {
  renderer_.init();
  font_manager_.init();
  this->load();
  while (!controller_.quit()) {
    controller_.poll();
    renderer_.clear();
    this->render();
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

mesh mesh_manager::create(GLsizeiptr vertex_size, const GLvoid *vertices, GLsizeiptr index_size, const GLvoid *indices, size_t stride) {
  mesh mesh;
  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_size, vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &mesh.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, indices, GL_STATIC_DRAW);

  mesh.stride = stride;
  return mesh;
}

void mesh_manager::destroy(mesh &mesh) {
  glDeleteBuffers(1, &mesh.vbo);
  glDeleteBuffers(1, &mesh.ibo);
}

void render_object::render() {
  glUseProgram(ref_.program);
  
  glUniformMatrix4fv(ref_.u_mvp, 1, GL_FALSE, (float*)&consts_.mvp);
  glUniform3fv(ref_.u_kolor, 1, (float*)&consts_.kolor);

  glBindBuffer(GL_ARRAY_BUFFER, mesh_.vbo);
  glEnableVertexAttribArray(ref_.a_position);
  glVertexAttribPointer(ref_.a_position, 3, GL_FLOAT, GL_FALSE, mesh_.stride, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_.ibo);
  glDrawElements(GL_TRIANGLES, 1 + frame_.end_index - frame_.start_index, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * frame_.start_index));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(ref_.a_position);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

program_id compiler::compile(const source &vert_source, const source &frag_source) {
  const shader_id vert = shader_manager_.create(vert_source, GL_VERTEX_SHADER);
  const shader_id frag = shader_manager_.create(frag_source, GL_FRAGMENT_SHADER);
  const program_id program_id = program_manager_.create(vert, frag);
  shader_manager_.destroy(vert);
  shader_manager_.destroy(frag);
  return program_id;
}

void font_manager::init() {
  if(FT_Init_FreeType(&library_)) {
    fprintf(stderr, "Could not init freetype library\n");
    exit(1);
  }

  FT_Face face;
  if(FT_New_Face(library_, "data/Fiery_Turk.ttf", 0, &face)) {
    fprintf(stderr, "Could not open font\n");
    exit(1);
  }
  FT_Set_Pixel_Sizes(face, 0, 48);

  {
    const FT_ULong ch = 'A';
    if(FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
      fprintf(stderr, "Could not load character '%lu'\n", ch);
      exit(1);
    }
    FT_GlyphSlot g = face->glyph;

    std::cout << "glyph: " << ch << std::endl;
    std::cout << "  bitmap.buffer " << (void*)g->bitmap.buffer << std::endl;
    std::cout << "  bitmap.width " << g->bitmap.width << std::endl;
    std::cout << "  bitmap.rows " << g->bitmap.rows << std::endl;
    std::cout << "  bitmap_left " << g->bitmap_left << std::endl;
    std::cout << "  bitmap_top " << g->bitmap_top << std::endl;
    std::cout << "  advance.x " << g->advance.x / 64 << std::endl;
    std::cout << "  advance.y " << g->advance.y / 64 << std::endl;
  }
}

//

glm::mat4 camera(float translate, glm::vec2 const & rotate) {
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -translate));
  view = glm::rotate(view, rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
  return projection * view * model;
}

