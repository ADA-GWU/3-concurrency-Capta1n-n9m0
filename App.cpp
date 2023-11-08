//
// Created by aliev on 08/11/2023.
//

#include "App.h"

App *App::instance_ = nullptr;
std::mutex App::mutex_;

App::App(const std::string& title, int width, int height) {
  title_ = title;
  width_ = width;
  height_ = height;
  running = false;
}

App *App::GetInstance(const std::string& title, int width, int height) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) {
    instance_ = new App(title, width, height);
  }
  return instance_;
}

bool App::Init() {
  if(running) {
    return false;
  }
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  window_ = SDL_CreateWindow(title_.c_str(),
                             SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                             SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                             width_, height_, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window_ == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_ == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_SetRenderDrawColor(renderer_, 250, 250, 250, 255);
  SDL_RenderClear(renderer_);

  return true;
}

bool App::Quit() {
  if(!running) {
    return false;
  }
  running = false;
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
  return true;
}

void App::OnEvent(SDL_Event* event) {
  switch(event->type) {
    case SDL_QUIT:
      running = false;
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

void App::OnResize(int width, int height) {
  width_ = width;
  height_ = height;
}

void App::OnRender(cv::Mat& image) {
  cv::Mat image_copy = image.clone();

  if(image.channels() == 3) {
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
  } else if (image.channels() == 4) {
    cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
  } else {
    std::cerr << "Image has unsupported number of channels!" << std::endl;
    return;
  }

  Uint32 rmask, gmask, bmask, amask;
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = (image.channels() == 4) ? 0x000000ff : 0;
  #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (image.channels() == 4) ? 0xff000000 : 0;
  #endif

  SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(image.data, image.cols, image.rows,
                                                  image.channels() * 8, image.step,
                                                  rmask, gmask, bmask, amask);
  if (surface == nullptr) {
    std::cerr << "Surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
  if (texture == nullptr) {
    std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return;
  }

  SDL_FreeSurface(surface);

  SDL_RenderCopy(renderer_, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer_);

  SDL_DestroyTexture(texture);
}

void App::Loop() {
  running = true;
  SDL_Event event;
  while(running) {
    while(SDL_PollEvent(&event)) {
      OnEvent(&event);
    }
  }
}


