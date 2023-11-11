#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "omp.h"
#include <format>

// macro for sleep for Windows, Linux and macOS
#ifdef _WIN32
#include <windows.h>
#define SLEEP(x) Sleep(x)
#else
#include <unistd.h>
#define SLEEP(x) usleep(x * 1000)
#endif


const int IMAGE_CHANGE_DELAY = 500;

bool running = true;
int num_threads = 0;

void processImage(cv::Mat& image, int kernel_size) {
  omp_set_num_threads(num_threads);
  #pragma omp parallel for shared(image, kernel_size, std::cout, std::cerr, running) default(none) schedule(dynamic) collapse(2)
  for(int i = 0; i < image.rows; i += kernel_size) {
    for(int j = 0; j < image.cols; j += kernel_size) {
      if(!running) continue;
      #pragma omp critical
      std::cout << "Thread " << omp_get_thread_num() << " is processing region (" << i << ", " << j << ")" << std::endl;
      SLEEP(100);
      cv::Rect area = cv::Rect(j, i, std::min(kernel_size, image.cols - j),
                               std::min(kernel_size, image.rows - i));
      #pragma omp critical
      {
        cv::Mat region = image(area);
        region = cv::mean(region);
      }
    }
  }
}



int main(int argc, char* argv[]) {
  omp_set_nested(1);
  num_threads = omp_get_max_threads();
  omp_set_num_threads(2);
  SDL_Color bgColor = {250, 250, 250, 255};
  int kernel_size = 100;
  cv::Mat image = cv::imread("../input/Mona Lisa.jpg", cv::IMREAD_UNCHANGED);

  if(image.channels() == 3){
    cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);
  } else if(image.channels() == 4) {
    cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
  } else {
    std::cerr << "Invalid image format!" << std::endl;
    return 1;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  int width = 1000;
  int height = 1000;
  auto image_size = image.size();
  double x_scale = (double)width / image_size.width;
  double y_scale = (double)height / image_size.height;
  double scale = x_scale < y_scale ? x_scale : y_scale;

  auto window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
  SDL_Window* window = SDL_CreateWindow("Image Convolution",SDL_WINDOWPOS_CENTERED_DISPLAY(1),SDL_WINDOWPOS_CENTERED_DISPLAY(1),width, height, window_flags);
  if (window == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
  SDL_RenderClear(renderer);


  SDL_Texture *texture = SDL_CreateTexture(renderer,
                                           SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32,
                                           SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING,
                                           image.cols, image.rows);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  if (texture == nullptr) {
    std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }
  SDL_UpdateTexture(texture, NULL, image.data, image.step);

  SDL_RenderClear(renderer);
  SDL_Rect dRect = {0, 0, static_cast<int>(image.cols * scale), static_cast<int>(image.rows * scale)};
  SDL_RenderCopy(renderer, texture, NULL, &dRect);
  SDL_RenderPresent(renderer);

  #pragma omp parallel shared(renderer, texture, kernel_size, width, height, image, std::cout, std::cerr, scale, dRect, running) default(none)
  {
    #pragma omp master
    {
      #pragma omp critical
      std::cout << "Master thread is running on thread " << omp_get_thread_num() << std::endl;
      SDL_Event event;
      unsigned int startTicks = SDL_GetTicks();
      running = true;
      while (running) {
        while (SDL_PollEvent(&event)) {
          switch (event.type) {
            case SDL_QUIT:
              running = false;
              break;
            case SDL_KEYDOWN:
              if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
              }
              break;
            case SDL_WINDOWEVENT:
              if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                #pragma omp critical
                {
                  width = event.window.data1;
                  height = event.window.data2;
                  scale = (double) width / image.cols < (double) height / image.rows ? (double) width / image.cols
                                                                                     : (double) height / image.rows;
                  dRect.w = static_cast<int>(image.cols * scale);
                  dRect.h = static_cast<int>(image.rows * scale);

                  SDL_UpdateTexture(texture, NULL, image.data, image.step);
                }
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, &dRect);
                SDL_RenderPresent(renderer);
              }
              break;
          }
        }
          unsigned int currentTicks = SDL_GetTicks();
        if (currentTicks - startTicks > IMAGE_CHANGE_DELAY) {
          #pragma omp critical
          SDL_UpdateTexture(texture, NULL, image.data, image.step);
          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer, texture, NULL, &dRect);
          SDL_RenderPresent(renderer);
          startTicks = currentTicks;
        }
      }
    }
    #pragma omp single
    {
      #pragma omp critical
      std::cout << "Single thread is running on thread " << omp_get_thread_num() << std::endl;
      processImage(image, kernel_size);
      cv::Mat image_copy = image.clone();
      cv::cvtColor(image_copy, image_copy, cv::COLOR_RGBA2BGRA);
      cv::imwrite("../output/Diamond.png", image_copy);
    }
  }


  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}