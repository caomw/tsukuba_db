#include "db.h"

#include <opencv2/core/version.hpp>

#if CV_MAJOR_VERSION == 3
  #include <opencv2/core.hpp>
  #include <opencv2/highgui.hpp>
  #include <opencv2/contrib.hpp>
#else
  #include <opencv2/core/core.hpp>
  #include <opencv2/highgui/highgui.hpp>
  #include <opencv2/contrib/contrib.hpp>
#endif

#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#define PREFIX_L "left/tsukuba_"
#define PREFIX_R "right/tsukuba_"
#define POSTFIX_L "_L_"
#define POSTFIX_R "_R_"

#define EXT ".png"
#define EXT_DEPTH ".xml"

#define GRAYSCALE (-1)

using std::string;

namespace tsukuba {

/**
 ** TsukubaStereoData Contructor
 **/
TsukubaStereoData::TsukubaStereoData(const int size,
                                     const string &db_path,
                                     const string &type,
                                     const string &subtype_folder,
                                     const string &subtype)
    : m_size(size),
      m_folder(db_path + type + "/" + subtype_folder + "/"),
      m_left_prefix(m_folder + PREFIX_L + subtype + POSTFIX_L),
      m_right_prefix(m_folder + PREFIX_R + subtype + POSTFIX_R),
      m_winname("tsukuba::" + type + "::" + subtype) {}

TsukubaStereoData::~TsukubaStereoData() {}

cv::Mat TsukubaStereoData::left(int idx) {
  return get_file(idx, m_left_prefix);
}

cv::Mat TsukubaStereoData::right(int idx) {
  return get_file(idx, m_right_prefix);
}

void TsukubaStereoData::show_left(int idx, int delay) {
  display(left(idx), delay);
}

void TsukubaStereoData::show_right(int idx, int delay) {
  display(right(idx), delay);
}

cv::Mat TsukubaStereoData::get_file(int idx, const string &prefix) {
  // Generate a 5 character index: "%.5d"
  char number[6];
  if (idx < 0 || idx >= size()) {
    fprintf(stderr, "Index (%d) out of range.\n", idx);
    return cv::Mat();
  }
  snprintf(number, sizeof(number), "%.5d", idx + 1);

  // Full path
  string file = prefix + number + EXT;

  // Load the image
  cv::Mat image = cv::imread(file);

  // Check for invalid input
  if (!image.data) {
    fprintf(stderr, "Could not open file: '%s'\n", file.c_str());
    return cv::Mat();
  }

  return image;
}

void TsukubaStereoData::display(cv::Mat image, int delay) {
  // Display image. Press 'q' or ESC (27) to exit, in case of 'delay==0'
  cv::imshow(m_winname, image);
  char k;
  while (delay == 0 && (k = cv::waitKey(0)) != 'q' && k != 27) {
  }
}

/**
 ** TsukubaStereoDepth Contructor
 **/
TsukubaStereoDepth::TsukubaStereoDepth(const int size,
                                       const string &db_path,
                                       const string &type,
                                       const string &subtype_folder,
                                       const string &subtype)
    : m_colormap(GRAYSCALE),
      TsukubaStereoData(size, db_path, type, subtype_folder, subtype) {}

TsukubaStereoDepth::~TsukubaStereoDepth() {}

cv::Mat TsukubaStereoDepth::get_file(int idx, const string &prefix) {
  // Generate a 5 character index: "%.5d"
  char number[6];
  if (idx < 0 || idx >= size()) {
    fprintf(stderr, "Index (%d) out of range.\n", idx);
    return cv::Mat();
  }
  snprintf(number, sizeof(number), "%.5d", idx + 1);

  // Full path
  string file = prefix + number + EXT_DEPTH;

  // Load the image
  cv::FileStorage fs(file, cv::FileStorage::READ);
  cv::Mat image;
  fs["depth"] >> image;
  fs.release();

  // Check for invalid input
  if (!image.data) {
    fprintf(stderr, "Could not open file: '%s'\n", file.c_str());
    return cv::Mat();
  }

  return image;
}

void TsukubaStereoDepth::display(cv::Mat map, int delay) {
  double min, max;
  cv::minMaxIdx(map, &min, &max);

  // Expand your range to 0..255. Similar to histEq()
  cv::Mat adj_map;
  map.convertTo(adj_map, CV_8UC1, 255 / (max - min), -min);

  // It converts the grayscale image into a tone-mapped one
  if (m_colormap != GRAYSCALE) {
    cv::Mat color_img;
    applyColorMap(adj_map, color_img, m_colormap);
    adj_map = color_img;
  }

  TsukubaStereoData::display(adj_map, delay);
}

/**
 ** TsukubaCameraTrack Contructor
 **/
TsukubaCameraTrack::TsukubaCameraTrack(const int size,
                                       const string &db_path)
    : m_pos(size), m_rot(size) {
  // load tracks
  std::ifstream file(string(db_path + "groundtruth/camera_track.txt").c_str());
  if (!file.is_open()) {
    fprintf(stderr, "Error: file could not be opened\n");
    return;
  }

  // read csv format
  int i = 0;
  string line, token;
  std::vector<double> number(6);

  // read by line
  while (std::getline(file, line)) {
    number.clear();
    std::stringstream linestream(line);

    // read by token
    while (std::getline(linestream, token, ',')) {
      number.push_back(atof(token.c_str()));
      convert(i, number);
      // printf("%3.6f ", atof(token.c_str()));
    }
    i++;
  }

  // close file
  file.close();
}

TsukubaCameraTrack::~TsukubaCameraTrack() {}

/**
 ** number is a vector of 6 float values: X Y Z A B C.
 ** Where (X, Y, Z) is the 3D position of the camera and,
 ** (A, B, C) are the Euler angles (in degrees) that represent the orientation.
 **/
void TsukubaCameraTrack::convert(int i, const std::vector<double> &number) {
  // Position
  m_pos[i](0) = number[0];
  m_pos[i](1) = number[1];
  m_pos[i](2) = number[2];

  // Orientation (XYZ order)
  const double a = number[0] * M_PI / 180;
  const double b = number[1] * M_PI / 180;
  const double c = number[2] * M_PI / 180;

  const double Sa = sin(a), Ca = cos(a);
  const double Sb = sin(b), Cb = cos(b);
  const double Sc = sin(c), Cc = cos(c);

  m_rot[i](0, 0) =  Cb*Cc;
  m_rot[i](0, 1) = -Cb*Sc;
  m_rot[i](0, 2) =  Sb;

  m_rot[i](1, 0) =  Ca*Sc + Sa*Sb*Cc;
  m_rot[i](1, 1) =  Ca*Cc - Sa*Sb*Sc;
  m_rot[i](1, 2) = -Sa*Cb;

  m_rot[i](2, 0) =  Sa*Sc - Ca*Sb*Cc;
  m_rot[i](2, 1) =  Sa*Cc + Ca*Sb*Sc;
  m_rot[i](2, 2) =  Ca*Cb;
}

/**
 ** DB Contructor
 **/
DB::DB(const string &path, const int size)
    : m_path(path),
      m_size(size),
      daylight(size, path, "illumination", "daylight", "daylight"),
      flashlight(size, path, "illumination", "flashlight", "flashlight"),
      fluorescent(size, path, "illumination", "fluorescent", "fluorescent"),
      lamps(size, path, "illumination", "lamps", "lamps"),
      disparity(size, path, "groundtruth", "disparity_maps", "disparity"),
      discontinuity(size, path, "groundtruth", "discontinuity_maps", "discontinuity"),
      occlusion(size, path, "groundtruth", "occlusion_maps", "occlusion"),
      depth(size, path, "groundtruth", "depth_maps", "depth"),
      cam(size, path) {}

DB::~DB() {}

}  // namespace tsukuba
