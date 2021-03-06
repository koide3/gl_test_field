#ifndef GUIK_PROJECTION_CONTROL_HPP
#define GUIK_PROJECTION_CONTROL_HPP

#include <Eigen/Core>

namespace guik {

class ProjectionControl {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  ProjectionControl(const Eigen::Vector2i& size);
  ~ProjectionControl();

  void set_size(const Eigen::Vector2i& size) {
    this->size = size;
  }

  int get_projection_mode() const {
    return projection_mode;
  }

  void set_projection_mode(int mode) {
    projection_mode = mode;
  }

  float get_fov() const {
    return fovy;
  }

  void set_fov(float fov) {
    fovy = fov;
  }

  void set_depth_range(const Eigen::Vector2f& range);

  Eigen::Matrix4f projection_matrix() const;

  void draw_ui();

  void show();

private:
  bool show_window;
  Eigen::Vector2i size;

  int projection_mode;

  float fovy;
  float width;
  float near;
  float far;
};

}  // namespace guik

#endif