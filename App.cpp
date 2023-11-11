//
// Created by aliev on 08/11/2023.
//

#include "App.h"

App *App::instance = nullptr;
std::mutex App::mutex;

App::App(const std::string& _title, int _width, int _height) {
  title = _title;
  width = _width;
  height = _height;
  running = false;
}

App *App::GetInstance() {
  std::lock_guard<std::mutex> lock(mutex);
  if (instance == nullptr) {
    instance = new App("Image Convolution", 1000, 1000);
  }
  return instance;
}

App *App::GetInstance(const std::string& title_, int width_, int height_) {
  std::lock_guard<std::mutex> lock(mutex);
  if (instance == nullptr) {
    instance = new App(title_, width_, height_);
  }
  return instance;
}

void App::setImage(cv::Mat& _image) {
  std::lock_guard<std::mutex> lock(mutex);
  image = _image;
}

void App::setBgColor(SDL_Color& _bgColor) {
  std::lock_guard<std::mutex> lock(mutex);
  bgColor = _bgColor;
}

void App::setRenderDelay(unsigned int _renderDelay) {
  std::lock_guard<std::mutex> lock(mutex);
  renderDelay = _renderDelay;
}

bool App::Init() {
  if(image.empty()) {
    std::cerr << "Image is empty!" << std::endl;
    return false;
  }
  if(running) {
    return false;
  }
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  width = std::min(width, DM.w);
  height = std::min(height, DM.h);

  auto window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  window = SDL_CreateWindow(title.c_str(),
                            SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                            SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                            width, height, window_flags);
  SDL_GetWindowSize(window, &width, &height);

  auto image_size = image.size();
  double x_scale = (double)width / image_size.width;
  double y_scale = (double)height / image_size.height;
  scale = std::min(x_scale, y_scale);
  dRect.w = static_cast<int>(image.cols * scale);
  dRect.h = static_cast<int>(image.rows * scale);

  if (window == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
  SDL_RenderClear(renderer);

  texture = SDL_CreateTexture(renderer,
                              SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32,
                              SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING,
                              image.cols, image.rows);
  if (texture == nullptr) {
    std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  OnRender();

  return true;
}

bool App::Quit() {
  if(!running) {
    return false;
  }
  running = false;
  if(texture) SDL_DestroyTexture(texture);
  if(renderer) SDL_DestroyRenderer(renderer);
  if(window) SDL_DestroyWindow(window);
  SDL_Quit();
  return true;
}

void App::OnKeyDown(SDL_Keycode sym) {
  switch(sym) {
    case SDLK_ESCAPE:
      running = false;
      break;
    default:
      break;
  }
}

void App::OnEvent(SDL_Event* event) {
  switch(event->type) {
    case SDL_QUIT:
      running = false;
      break;
    case SDL_KEYDOWN:
      OnKeyDown(event->key.keysym.sym);
      break;
    case SDL_WINDOWEVENT:
      if(event->window.event == SDL_WINDOWEVENT_RESIZED) {
        OnResize(event->window.data1, event->window.data2);
      }
      break;
    default:
      break;
  }
}

void App::OnResize(int _width, int _height) {
  #pragma omp critical
  {
    width = _width;
    height = _height;
    auto image_size = image.size();
    double x_scale = (double) width / image_size.width;
    double y_scale = (double) height / image_size.height;
    scale = std::min(x_scale, y_scale);
    dRect.w = static_cast<int>(image.cols * scale);
    dRect.h = static_cast<int>(image.rows * scale);
  }
  OnRender();
}

void App::OnRender() {
  #pragma omp critical
  SDL_UpdateTexture(texture, NULL, image.data, image.step);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, &dRect);
  SDL_RenderPresent(renderer);
  lastRenderTicks = SDL_GetTicks();
}

void App::Loop() {
  running = true;
  SDL_Event event;
  while(running) {
    while(SDL_PollEvent(&event)) {
      OnEvent(&event);
    }
    if(SDL_GetTicks() - lastRenderTicks > renderDelay) {
      OnRender();
    }
  }
}


