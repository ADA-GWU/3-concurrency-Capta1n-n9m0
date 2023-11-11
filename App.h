#ifndef ADA_A3_IMAGE_CONV_APP_H
#define ADA_A3_IMAGE_CONV_APP_H

#include <iostream>
#include <mutex>
#include <string>
#include <SDL.h>
#include <opencv2/opencv.hpp>
#include <vector>

class App {
private:
  static App *instance;
  static std::mutex mutex;

protected:
  std::string title;
  int width;
  int height;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
  cv::Mat image = cv::Mat();
  SDL_Color bgColor = {0, 0, 0, 255};
  unsigned int lastRenderTicks = 0;
  unsigned int renderDelay = 0;
  double scale = 1.0;
  SDL_Rect dRect = {0, 0, 0, 0};


public:
  bool running;

protected:
  App(const std::string& _title, int _width, int _height);
  ~App()= default;

public:
  App(const App&) = delete;
  void operator=(const App&) = delete;

  static App *GetInstance();
  static App *GetInstance(const std::string& title_, int width_, int height_);

public:
  void setImage(cv::Mat& _image);
  void setBgColor(SDL_Color& _bgColor);
  void setRenderDelay(unsigned int _renderDelay);
  bool Init();
  bool Quit();
  void OnEvent(SDL_Event* event);
  void OnKeyDown(SDL_Keycode sym);
  void OnResize(int width, int height);
  void OnRender();
  void Loop();
};



#endif //ADA_A3_IMAGE_CONV_APP_H
