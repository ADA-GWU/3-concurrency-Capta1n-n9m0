#include <iostream>
#include <string>
#include <tuple>
#include "omp.h"
#include "App.h"
#include "opencv2/opencv.hpp"
#include <vector>


std::tuple<cv::Mat, int, char, SDL_Color> process_arguments(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  cv::Mat image;
  int kernel_size;
  char execution_mode;
  SDL_Color color;
  std::tie(image, kernel_size, execution_mode, color) = process_arguments(argc, argv);



  App* app = App::GetInstance("Image Convolution", 640, 480);
  if(!app->Init(image)) {
    std::cerr << "Could not initialize application!" << std::endl;
    return 1;
  }


#pragma omp parallel sections shared(app, image, kernel_size, execution_mode) default(none)
  {
#pragma omp section
    {
      app->Loop();
    }
#pragma omp section
    {
      /*
       * This section should be responsible for the image processing.
       * It should be able to receive the image from the main thread and process it.
       * Idea is to run one or several convolutions or "convolutors" that would emmit and SDL event to signal
       * rerendering
       */
    }
  }


  return 0;
}


std::tuple<cv::Mat, int, char, SDL_Color> process_arguments(int argc, char* argv[]) {
  if(argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <input_image> <kernel_size> <execution_mode(S|M)> <color(RGB)>" << std::endl;
    exit(1);
  }
  char *input_image = argv[1];
  cv::Mat image;
  try{
    image = cv::imread(input_image, cv::IMREAD_UNCHANGED);
  } catch(cv::Exception& e) {
    std::cerr << "Could not read image!" << std::endl;
    exit(1);
  }
  std::string kernel_size_str = std::string(argv[2]);
  int kernel_size;
  try{
    kernel_size = std::stoi(kernel_size_str);
  } catch(std::invalid_argument& e) {
    std::cerr << "Invalid kernel size!" << std::endl;
    exit(1);
  }
  char execution_mode = argv[3][0];
  if(execution_mode != 'S' && execution_mode != 'M') {
    std::cerr << "Invalid execution mode!" << std::endl;
    exit(1);
  }
  SDL_Color color;
  if(argc > 4) {
    std::string color_str = std::string(argv[4]);
    if(color_str.length() != 6) {
      std::cerr << "Invalid color!" << std::endl;
      exit(1);
    }
    try{
      color.r = std::stoi(color_str.substr(0, 2), nullptr, 16);
      color.g = std::stoi(color_str.substr(2, 2), nullptr, 16);
      color.b = std::stoi(color_str.substr(4, 2), nullptr, 16);
    } catch(std::invalid_argument& e) {
      std::cerr << "Invalid color!" << std::endl;
      exit(1);
    }
  } else {
    color.r = 250;
    color.g = 250;
    color.b = 250;
  }
  color.a = 255;
  return std::make_tuple(image, kernel_size, execution_mode, color);
}