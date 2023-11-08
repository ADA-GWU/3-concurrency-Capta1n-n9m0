#ifndef ADA_A3_IMAGE_CONV_APP_H
#define ADA_A3_IMAGE_CONV_APP_H

#include <iostream>
#include <mutex>
#include <string>
#include <SDL.h>
#include <opencv2/opencv.hpp>

class App {
private:
  static App *instance_;
  static std::mutex mutex_;

protected:
  std::string title_;
  int width_;
  int height_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;

public:
  bool running;

protected:
  App(const std::string& title, int width, int height);
  ~App()= default;

public:
  App(const App&) = delete;
  void operator=(const App&) = delete;

  static App *GetInstance(const std::string& title, int width, int height);

public:
  bool Init();
  bool Quit();
  void OnEvent(SDL_Event* event);
  void OnResize(int width, int height);
  void OnRender(cv::Mat& image);
  void Loop();
};



#endif //ADA_A3_IMAGE_CONV_APP_H
