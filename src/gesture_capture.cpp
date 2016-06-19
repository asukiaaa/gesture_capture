//
// This program does the following process.
// - Gets depth from realsense.
// - Shows the image with using opencv.
// - Sends a request with using libcurl by depth status.
//
// 2016.05.24 Asuki Kono
//
// License: MIT
//
// reference: librealsense tutorial #1(Apache 2.0)
// https://github.com/IntelRealSense/librealsense/blob/master/examples/cpp-tutorial-1-depth.cpp
//
#include <cstdio>
#include <curl/curl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <librealsense/rs.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <unistd.h>

#define TARGET_DEPTH_MIN 1000
#define TARGET_DEPTH_MAX 2500

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

using namespace std;

//
// reference: https://curl.haxx.se/libcurl/c/getinfo.html
//
// If rolle of this class become large, it's better to use the following cpp package.
// https://curl.haxx.se/libcurl/cplusplus/
//
class CurlManager {
  private:
  CURL* curl;
  CURLcode res;

  public:
  CurlManager();

  bool with_printing;

  bool simple_get(string url);
  bool simple_get(char* url);
  bool simple_get(const char* url);
};

CurlManager::CurlManager() {
  curl = curl_easy_init();
  with_printing = false;
}

bool CurlManager::simple_get(string url) {
  simple_get(url.c_str());
}

bool CurlManager::simple_get(char* url) {
  simple_get((const char*)url);
}

bool CurlManager::simple_get(const char* url) {
  if( curl ) {
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // stop printing to stdio
    if (! with_printing) {
      curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    }

    res = curl_easy_perform(curl);

    if ( CURLE_OK == res ) {
      return true;
    } else {
      return false;
    }
  }
}

//
// main process
//
int main() try {

  // CURL
  CurlManager curl_manager;
  //curl_manager.with_printing = true;

  // Create a context object. This object owns the handles to all connected realsense devices.
  rs::context ctx;
  printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
  if(ctx.get_device_count() == 0) return EXIT_FAILURE;

  rs::device * dev = ctx.get_device(0);

  // Configure depth to run at VGA resolution at 30 frames per second
  dev->enable_stream(rs::stream::depth, IMAGE_WIDTH, IMAGE_HEIGHT, rs::format::z16, 30);
  dev->start();

  // Determine depth value corresponding to one meter
  const uint16_t one_meter = static_cast<uint16_t>(1.0f / dev->get_depth_scale());

  // Opencv values
  // IplImage* img = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), IPL_DEPTH_8U, 1);
  // cvNamedWindow("realsense_depth", CV_WINDOW_AUTOSIZE);

  // Gravity values
  int grav_x;
  int grav_y;
  int grav_count;

  // Open a GLFW window
  glfwInit();
  std::ostringstream ss; ss << "Gravity image";
  GLFWwindow * win = glfwCreateWindow(IMAGE_WIDTH, IMAGE_HEIGHT, ss.str().c_str(), 0, 0);
  glfwSetWindowUserPointer(win, &dev);
  glfwMakeContextCurrent(win);
  glfwSwapBuffers(win);

  char buffer[IMAGE_WIDTH * IMAGE_HEIGHT];

  while(!glfwWindowShouldClose(win)) {
    grav_x = 0;
    grav_y = 0;
    grav_count = 0;

    glfwPollEvents();
    dev->wait_for_frames();

    const uint16_t * depth_frame = reinterpret_cast<const uint16_t *>(dev->get_frame_data(rs::stream::depth));

    for(int y=0; y<480; ++y) {
      for(int x=0; x<640; ++x) {
        int depth = *depth_frame++;
        // char* output_pixel = &(img->imageData[(y*img->widthStep) + x]);
        char* target_buffer = &(buffer[x + (IMAGE_HEIGHT - 1 - y)*IMAGE_WIDTH]);

        if ( TARGET_DEPTH_MIN < depth && depth < TARGET_DEPTH_MAX) {
          // *output_pixel = (char)255;
          *target_buffer = (char)255;
          grav_x += x;
          grav_y += y;
          grav_count ++;
        } else {
          // *output_pixel = (char)0;
          *target_buffer = (char)0;
        }
      }
    }

    if ( grav_count != 0 ) {
      grav_x = grav_x / grav_count;
      grav_y = grav_y / grav_count;
    }

    printf("%d, %d, %d\n", grav_x, grav_y, grav_count);

    if ( grav_count > 1000 ) {
      string url;
      string kind;
      //  "tanbarin"
      //  "guitar"
      //  "rappa"
      //  "piano"
      if ( grav_x < 320 ) {
        if ( grav_y < 240 ) {
          kind = "guitar";
        } else {
          kind = "tanbarin";
        }
      } else {
        if ( grav_y > 240 ) {
          kind = "piano";
        } else {
          kind = "rappa";
        }
      }

      url =
        "http://192.168.11.12.:8080/shake/3/" + kind;
      cout << url << endl;

      // float x_accel = ((float)grav_x / 640) - 0.5;
      // url =
      //   "http://192.168.11.21.:8080/shake/1/cube/" +
      //   to_string(x_accel) +
      //   "/0/0";
      // cout << url << endl;

      //curl_manager.simple_get(url);
      usleep(1000000);
    }

    glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
    glfwSwapBuffers(win);

    //cvShowImage("realsense_depth", img);
    //cvWaitKey(10);
  }

  glfwDestroyWindow(win);
  glfwTerminate();
  return EXIT_SUCCESS;
} catch(const rs::error & e) {
  // Method calls against librealsense objects may throw exceptions of type rs::error
  printf("rs::error was thrown when calling %s(%s):\n", e.get_failed_function().c_str(), e.get_failed_args().c_str());
  printf("    %s\n", e.what());
  return EXIT_FAILURE;
}
