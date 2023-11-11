#include <iostream>
#include <string>
#include <tuple>
#include "opencv2/opencv.hpp"
#include "omp.h"
#include "App.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP(x) Sleep(x)
#else
#include <unistd.h>
#define SLEEP(x) usleep(x * 1000)
#endif

std::tuple<cv::Mat, int, char, SDL_Color> process_arguments(int argc, char* argv[]);
void prepareImage(cv::Mat& image);
void processImage(cv::Mat& image, int kernel_size, char execution_mode);

int main(int argc, char* argv[]) {
  omp_set_nested(1);
  int num_threads = omp_get_max_threads();
  omp_set_num_threads(std::min(2, num_threads));
  cv::Mat image;
  int kernel_size;
  char execution_mode;
  SDL_Color color;
  std::tie(image, kernel_size, execution_mode, color) = process_arguments(argc, argv);

  prepareImage(image);


  App* app = App::GetInstance("Image Convolution", 1000, 1000);
  app->setImage(image);
  app->setBgColor(color);
  app->setRenderDelay(0);
  if(!app->Init()) {
    std::cerr << "Could not initialize application!" << std::endl;
    return 1;
  }


#pragma omp parallel shared(app, image, kernel_size, execution_mode) default(none)
  {
#pragma omp master
    {
      app->Loop();
    }
#pragma omp single
    {
      processImage(image, kernel_size, execution_mode);
      cv::Mat image_copy = image.clone();
      cv::cvtColor(image_copy, image_copy, cv::COLOR_RGBA2BGRA);
      cv::imwrite("output.png", image_copy);
    }
  }

  app->Quit();
  image.release();

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

  SDL_Color color = {250, 250, 250, 255};
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
  }

  return std::make_tuple(image, kernel_size, execution_mode, color);
}

void prepareImage(cv::Mat& image) {
  if(image.channels() == 3){
    cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);
  } else if(image.channels() == 4) {
    cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
  } else {
    std::cerr << "Invalid image format!" << std::endl;
    exit(1);
  }
}

void processImage(cv::Mat& image, int kernel_size, char execution_mode) {
  App* app = App::GetInstance();
  if(execution_mode == 'S') {
    // Single thread
    for(int i = 0; i < image.rows; i += kernel_size) {
      for(int j = 0; j < image.cols; j += kernel_size) {
        if(!app->running) continue;
        SLEEP(100);
        cv::Rect area = cv::Rect(j, i, std::min(kernel_size, image.cols - j),
                                 std::min(kernel_size, image.rows - i));
        cv::Mat region = image(area);
        region = cv::mean(region);
      }
    }
  } else {
    // Multi thread
    #pragma omp parallel for collapse(2) shared(image, kernel_size, app) default(none)
    for(int i = 0; i < image.rows; i += kernel_size) {
      for(int j = 0; j < image.cols; j += kernel_size) {
        if(!app->running) continue;
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
}