// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

/////////////////////////////////////////////////////
// librealsense tutorial #1 - Accessing depth data //
/////////////////////////////////////////////////////

// First include the librealsense C++ header file
#include <librealsense/rs.hpp>
#include <cstdio>
#include <opencv2/highgui/highgui.hpp>

#define TARGET_DEPTH_MIN 1000
#define TARGET_DEPTH_MAX 2500



int main() try
{
    // Create a context object. This object owns the handles to all connected realsense devices.
    rs::context ctx;
    printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
    if(ctx.get_device_count() == 0) return EXIT_FAILURE;

    // This tutorial will access only a single device, but it is trivial to extend to multiple devices
    rs::device * dev = ctx.get_device(0);
    printf("\nUsing device 0, an %s\n", dev->get_name());
    printf("    Serial number: %s\n", dev->get_serial());
    printf("    Firmware version: %s\n", dev->get_firmware_version());

    // Configure depth to run at VGA resolution at 30 frames per second
    dev->enable_stream(rs::stream::depth, 640, 480, rs::format::z16, 30);
    dev->start();

    // Determine depth value corresponding to one meter
    const uint16_t one_meter = static_cast<uint16_t>(1.0f / dev->get_depth_scale());

    // Opencv values
    //IplImage* img = cvLoadImage("/home/asuki/Downloads/nano.png", CV_LOAD_IMAGE_COLOR);
    IplImage* img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    cvNamedWindow("opencvtest", CV_WINDOW_AUTOSIZE);
    cvShowImage("opencvtest", img);

    int grav_x;
    int grav_y;
    int grav_count;

    while(true)
    {
      grav_x = 0;
      grav_y = 0;
      grav_count = 0;

        // This call waits until a new coherent set of frames is available on a device
        // Calls to get_frame_data(...) and get_frame_timestamp(...) on a device will return stable values until wait_for_frames(...) is called
        dev->wait_for_frames();

        // Retrieve depth data, which was previously configured as a 640 x 480 image of 16-bit depth values
        const uint16_t * depth_frame = reinterpret_cast<const uint16_t *>(dev->get_frame_data(rs::stream::depth));

        // Print a simple text-based representation of the image, by breaking it into 10x20 pixel regions and and approximating the coverage of pixels within one meter
        char buffer[(640/10+1)*(480/20)+1];
        char * out = buffer;
        int coverage[64] = {};
        for(int y=0; y<480; ++y)
        {
            for(int x=0; x<640; ++x)
            {
                int depth = *depth_frame++;
                if(depth > 0 && depth < one_meter) ++coverage[x/10];

                // if(x%20 == 19 && y%20 == 19) {
                //   printf("%.4d ", depth);
                // }

                //img->imageData[(y*img->widthStep) + x] = depth/50;
                char* target_pixel = &(img->imageData[(y*img->widthStep) + x]);
                if ( TARGET_DEPTH_MIN < depth &&
                     depth < TARGET_DEPTH_MAX) {
                  //*target_pixel = depth/50;
                  *target_pixel = 255;
                  grav_x += x;
                  grav_y += y;
                  grav_count ++;
                } else {
                  *target_pixel = 0;
                }
            }

        }

        printf("%d, %d, %d\n", grav_x, grav_y, grav_count);
        cvShowImage("opencvtest", img);
        cvWaitKey(10);
    }
    
    return EXIT_SUCCESS;
}
catch(const rs::error & e)
{
    // Method calls against librealsense objects may throw exceptions of type rs::error
    printf("rs::error was thrown when calling %s(%s):\n", e.get_failed_function().c_str(), e.get_failed_args().c_str());
    printf("    %s\n", e.what());
    return EXIT_FAILURE;
}
