#ifndef TSUKUBA_DB_H_
#define TSUKUBA_DB_H_

#include <opencv2/core/core.hpp>

#include <string>
#include <vector>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

namespace tsukuba {

class TsukubaStereoData {
 public:
  TsukubaStereoData(const int size,
                    const std::string &db_path,
                    const std::string &type,
                    const std::string &subtype_folder,
                    const std::string &subtype);

  ~TsukubaStereoData();

  cv::Mat left(int idx);
  cv::Mat right(int idx);

  void show_left(int idx, int delay = 0);
  void show_right(int idx, int delay =0);

  int size() { return m_size; }

 protected:
  virtual cv::Mat get_file(int idx, const std::string &prefix);
  virtual void display(cv::Mat image, int delay);

 private:
  const int m_size;

  const std::string m_folder;
  const std::string m_left_prefix;
  const std::string m_right_prefix;

  const std::string m_winname;

  DISALLOW_COPY_AND_ASSIGN(TsukubaStereoData);
};

class TsukubaStereoDepth : public TsukubaStereoData {
 public:
  TsukubaStereoDepth(const int size,
                     const std::string &db_path,
                     const std::string &type,
                     const std::string &subtype_folder,
                     const std::string &subtype);

  ~TsukubaStereoDepth();

  // By default the 'm_colormap' value is GRAYSCALE (-1). Examples of colormap
  // are: cv::COLORMAP_AUTUMN, cv::COLORMAP_BONE, COLORMAP_JET, (in contrib.hpp)
  void set_colormap(int colormap) { m_colormap = colormap; }

 protected:
  virtual cv::Mat get_file(int idx, const std::string &prefix);
  virtual void display(cv::Mat image, int delay);

 private:
  int m_colormap;

  DISALLOW_COPY_AND_ASSIGN(TsukubaStereoDepth);
};

class TsukubaCameraTrack {
 public:
  TsukubaCameraTrack(const int size,
                     const std::string &db_path);

  ~TsukubaCameraTrack();

  const cv::Vec3d   &pos(int idx) { return m_pos[idx]; }
  const cv::Matx33d &rot(int idx) { return m_rot[idx]; }

 protected:
  void convert(int idx, const std::vector<double> &number);

 private:
  std::vector<cv::Vec3d>   m_pos;
  std::vector<cv::Matx33d> m_rot;

  DISALLOW_COPY_AND_ASSIGN(TsukubaCameraTrack);
};

class DB {
 public:
  DB(const std::string &path,
     const int size =1800);

  ~DB();

  // Illumination
  TsukubaStereoData daylight;
  TsukubaStereoData flashlight;
  TsukubaStereoData fluorescent;
  TsukubaStereoData lamps;

  // Groundtruth
  TsukubaStereoData discontinuity;
  TsukubaStereoData disparity;
  TsukubaStereoData occlusion;

  // Groundtruth - Depth map
  TsukubaStereoDepth depth;

  // Camera tracks
  TsukubaCameraTrack cam;

  int size() { return m_size; }
  const std::string &path() { return m_path; }

 private:
  const int m_size;
  const std::string m_path;
};

}  // namespace tsukuba

#endif  // TSUKUBA_DB_H_
