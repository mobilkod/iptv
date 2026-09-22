#include "video_decoder_sdf_sample.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>

#include <GLES2/gl2ext.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include "sample_data.h"

#define assertNoGLError() assert(!glGetError())

using ElementaryMediaStreamSource =
    samsung::wasm::ElementaryMediaStreamSource;

using ElementaryMediaStreamSourceListener =
    samsung::wasm::ElementaryMediaStreamSourceListener;

using ElementaryMediaTrack =
    samsung::wasm::ElementaryMediaTrack;

namespace {

const char kVertexShader[] =
    "varying vec2 v_texCoord;               \n"
    "attribute vec4 a_position;             \n"
    "attribute vec2 a_texCoord;             \n"
    "uniform vec2 v_scale;                  \n"
    "void main()                            \n"
    "{                                      \n"
    "    v_texCoord = v_scale * a_texCoord; \n"
    "    gl_Position = a_position;          \n"
    "}";

const char kFragmentShaderExternal[] =
    "#extension GL_OES_EGL_image_external : require       \n"
    "precision mediump float;                             \n"
    "varying vec2 v_texCoord;                             \n"
    "uniform samplerExternalOES s_texture;                \n"
    "void main()                                          \n"
    "{                                                    \n"
    "    gl_FragColor = texture2D(s_texture, v_texCoord); \n"
    "}                                                    \n";

void CreateShader(
    GLuint program,
    GLenum type,
    const char* source,
    int size) {
  GLuint shader = glCreateShader(type);

  glShaderSource(
      shader,
      1,
      &source,
      &size);

  glCompileShader(shader);

  glAttachShader(program, shader);

  glDeleteShader(shader);
}

int CAPIOnDrawTextureCompleted(
    double /*time*/,
    void* thiz) {
  if (thiz) {
    static_cast<VideoDecoderTrackDataPump*>(thiz)
        ->OnDrawCompleted();
  }

  return 0;
}

}  // namespace

VideoDecoderTrackDataPump::VideoDecoderTrackDataPump(
    ElementaryMediaTrack video_track)
    : TrackDataPump(std::move(video_track)) {
  InitializeGL();
  CreateGLObjects();
  CreateProgram();

  // Intentionally disabled while isolating the
  // "Invalid function pointer" runtime problem.
  //
  // video_track_.RegisterCurrentGraphicsContext();
}

void VideoDecoderTrackDataPump::OnDrawCompleted() {
  video_track_.RecycleTexture(texture_);
  RequestNewVideoTexture();
}

void VideoDecoderTrackDataPump::RequestNewVideoTexture() {
  video_track_.FillTextureWithNextFrame(
      texture_,
      [this](samsung::wasm::OperationResult result) {
        if (result !=
            samsung::wasm::OperationResult::kSuccess) {
          std::cout
              << "Filling texture with next frame failed"
              << std::endl;
          return;
        }

        Draw();
      });
}

void VideoDecoderTrackDataPump::CreateGLObjects() {
  static const float kVertices[] = {
      -1.0f, -1.0f,
      -1.0f,  1.0f,
       1.0f, -1.0f,
       1.0f,  1.0f,

       0.0f,  1.0f,
       0.0f,  0.0f,
       1.0f,  1.0f,
       1.0f,  0.0f,
  };

  GLuint buffer = 0;

  glGenBuffers(1, &buffer);

  glBindBuffer(
      GL_ARRAY_BUFFER,
      buffer);

  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(kVertices),
      kVertices,
      GL_STATIC_DRAW);

  assertNoGLError();
}

void VideoDecoderTrackDataPump::CreateProgram() {
  program_ = glCreateProgram();

  CreateShader(
      program_,
      GL_VERTEX_SHADER,
      kVertexShader,
      static_cast<int>(strlen(kVertexShader)));

  CreateShader(
      program_,
      GL_FRAGMENT_SHADER,
      kFragmentShaderExternal,
      static_cast<int>(strlen(kFragmentShaderExternal)));

  glLinkProgram(program_);

  glUseProgram(program_);

  glUniform1i(
      glGetUniformLocation(
          program_,
          "s_texture"),
      0);

  assertNoGLError();

  texcoord_scale_location_ =
      glGetUniformLocation(
          program_,
          "v_scale");

  assertNoGLError();

  GLint pos_location =
      glGetAttribLocation(
          program_,
          "a_position");

  GLint tc_location =
      glGetAttribLocation(
          program_,
          "a_texCoord");

  glEnableVertexAttribArray(
      pos_location);

  glVertexAttribPointer(
      pos_location,
      2,
      GL_FLOAT,
      GL_FALSE,
      0,
      0);

  glEnableVertexAttribArray(
      tc_location);

  glVertexAttribPointer(
      tc_location,
      2,
      GL_FLOAT,
      GL_FALSE,
      0,
      static_cast<float*>(0) + 8);

  glUseProgram(0);
}

void VideoDecoderTrackDataPump::Draw() {
  glUseProgram(program_);

  glUniform2f(
      texcoord_scale_location_,
      1.0f,
      1.0f);

  glActiveTexture(GL_TEXTURE0);

  glBindTexture(
      GL_TEXTURE_EXTERNAL_OES,
      texture_);

  glDrawArrays(
      GL_TRIANGLE_STRIP,
      0,
      4);

  assertNoGLError();

  emscripten_request_animation_frame(
      &CAPIOnDrawTextureCompleted,
      this);
}

void VideoDecoderTrackDataPump::InitializeSDL() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_MAJOR_VERSION,
      2);

  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_MINOR_VERSION,
      0);

  SDL_GL_SetSwapInterval(1);

  SDL_GL_SetAttribute(
      SDL_GL_DOUBLEBUFFER,
      1);

  SDL_GL_SetAttribute(
      SDL_GL_DEPTH_SIZE,
      24);

  SDL_GL_SetAttribute(
      SDL_GL_MULTISAMPLEBUFFERS,
      1);

  SDL_GL_SetAttribute(
      SDL_GL_MULTISAMPLESAMPLES,
      4);
}

void VideoDecoderTrackDataPump::InitializeGL() {
  InitializeSDL();

  int width = 0;
  int height = 0;

  emscripten_get_canvas_element_size(
      "#canvas",
      &width,
      &height);

  window_ = SDL_CreateWindow(
      "VideoTexture",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      width,
      height,
      SDL_WINDOW_OPENGL |
          SDL_WINDOW_SHOWN);

  gl_context_ =
      SDL_GL_CreateContext(window_);

  SDL_GL_MakeCurrent(
      window_,
      gl_context_);

  glGenTextures(
      1,
      &texture_);

  glViewport(
      0,
      0,
      width,
      height);

  glClearColor(
      1.0f,
      0.0f,
      0.0f,
      0.0f);

  glClear(GL_COLOR_BUFFER_BIT);

  assertNoGLError();
}

void VideoDecoderSamplePlayer::OnCanPlay() {
  if (!media_element_->IsPaused()) {
    return;
  }

  media_element_->Play(
      [this](samsung::wasm::OperationResult result) {
        if (result !=
            samsung::wasm::OperationResult::kSuccess) {
          std::cout
              << "Cannot play."
              << std::endl;
          return;
        }

        VideoDecoderTrackDataPump*
            video_decoder_track_data_pump =
                static_cast<VideoDecoderTrackDataPump*>(
                    track_data_pump_.get());

        video_decoder_track_data_pump
            ->RequestNewVideoTexture();
      });
}

std::unique_ptr<TrackDataPump>
VideoDecoderSamplePlayer::CreateTrackDataPump(
    ElementaryMediaTrack&& video_track) {
  return std::unique_ptr<TrackDataPump>(
      new VideoDecoderTrackDataPump(
          std::move(video_track)));
}