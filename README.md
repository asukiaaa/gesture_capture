# gesture_capture
A project to deal depth image from realsense with using opencv and send curl request.

# Requirements
- librealsense
- libcurl

# Useage

## Ubuntu
```
cd [gesture_capture_dir]
make
sudo ./bin/gesture_capture
```

## Mac
You need a librealsense directory.

Copy program to a librealsense src dir.
```
cp [gesture_capture_dir]/src/gesture_capture.cpp \
   [librealsesnse_dir]/src/cpp-gesture_capture.cpp
```

Add LIBCURL_FLAGS to [librealsense_dir]/Makefile
```
LIBCURL_FLAGS := -lcurl
...
bin/cpp-%: examples/cpp-%.cpp library
#	$(CXX) $< -std=c++11 $(REALSENSE_FLAGS) $(GLFW3_FLAGS) -o $@
$(CXX) $< -std=c++11 $(REALSENSE_FLAGS) $(GLFW3_FLAGS) $(CURL_FLAGS) -o $@
```

Compile
```
cd [librealsense_dir]
make
```

Run
```
cd [librealsense_dir]
sudo bin/cpp-gesture_capture
```

# Options
You can set target url for curl.
```
sudo bin/gesture_capture.cpp --url http://localhost:1234
```

# License
MIT

# References
https://github.com/IntelRealSense/librealsense