#include <iostream>
#include <SDL.h>
#include <opencv2/opencv.hpp>


int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <image_file>" << std::endl;
    return 1;
  }

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Create an SDL window
  SDL_Window* window = SDL_CreateWindow("OpenCV and SDL2 Image Display",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        640, 480, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Create an SDL renderer
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Fill the background with white (255, 255, 255)
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_RenderClear(renderer);

  // Load and display the image specified in the command-line argument using OpenCV
  cv::Mat image = cv::imread(argv[1], cv::IMREAD_UNCHANGED);
  if (image.empty()) {
    std::cerr << "Could not open or find the image!" << std::endl;
    return 1;
  }

  if(image.channels() == 3) {
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
  } else if (image.channels() == 4) {
    cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
  } else {
    std::cerr << "Image has unsupported number of channels!" << std::endl;
    return 1;
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
  // Create an SDL surface from the OpenCV image
  SDL_Surface* sdlSurface = SDL_CreateRGBSurfaceFrom(
      (void*)image.data,
      image.cols,
      image.rows,
      image.channels() * 8,
      image.step[0],
      rmask, gmask, bmask, amask
  );

  if (sdlSurface == nullptr) {
    std::cerr << "Failed to create SDL surface! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Create an SDL texture from the SDL surface
  SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, sdlSurface);
  if (sdlTexture == nullptr) {
    std::cerr << "Failed to create SDL texture! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Copy the texture to the renderer
  SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);

  // Update the renderer
  SDL_RenderPresent(renderer);

  // Wait for a key press or window close event
  bool quit = false;
  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN) {
        quit = true;
      }
    }
  }

  // Cleanup and exit
  SDL_DestroyTexture(sdlTexture);
  SDL_FreeSurface(sdlSurface);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
