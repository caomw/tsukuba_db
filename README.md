## tsukuba_db

Parser for the New Tsukuba Stereo Dataset.

  * Download: http://www.cvlab.cs.tsukuba.ac.jp/dataset/tsukubastereo.php
  * Video: https://www.youtube.com/watch?v=0rEZcokuQfg
  * Dependency: OpenCV

### How to compile?

    mkdir build && cd build
    cmake ..
    make
    ./parser --path ~/Data/NewTsukubaStereoDataset/

### Notes

  * The code is inside the `tsubuka/` folder. A library will be created by the `CMakeLists.txt`.
  * It will be easy to include this code in external projects, by copying the folder and using `add_subdirectory(tsukuba)`.
  * The Google code style is used (except for the name convention of class members).

