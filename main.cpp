#include <iostream>
#include <string>

#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "tsukuba/db.h"

using std::string;
using std::cout;

// Command line parser
bool command_line_parser(int argc, char **argv, string *path);

int main(int argc, char **argv) {
  // Parsing command line
  string path;
  bool status_ok = command_line_parser(argc, argv, &path);
  if (!status_ok)
    return 1;

  tsukuba::DB db(path);

  db.depth.set_colormap(cv::COLORMAP_BONE);
  db.depth.show_left(24);

  cv::Mat tmp;
  tmp = db.daylight.left(24);
//   tmp = db.flashlight.right(24);
//   tmp = db.fluorescent.right(24);
//   tmp = db.lamps.left(24);
//   tmp = db.discontinuity.left(300);
//   tmp = db.disparity.right(25);
//   tmp = db.occlusion.right(25);
  cv::imshow("test", tmp);
  cv::waitKey(0);

  cout << "pos: " << cv::Mat(db.cam.pos(1799)) << std::endl;
  cout << "rot: " << cv::Mat(db.cam.rot(1799)) << std::endl;

  return 0;
}

bool command_line_parser(int argc, char **argv, string *path) {
  const string commandline_usage =
      "Usage: " + string(argv[0]) + " -h[--help] | -p[--path} <dataset>\n";

  if (argc == 1) {
    cout << commandline_usage;
    return false;
  }

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      cout << commandline_usage;
      return false;
    }

    if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0) {
      if (argc > i + 1) {
        *path = argv[i + 1];
      } else {
        cout << commandline_usage;
        return false;
      }
    }
  }

  return true;
}
