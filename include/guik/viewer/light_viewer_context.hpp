#ifndef GUIK_LIGHT_VIEWER_CONTEXT_HPP
#define GUIK_LIGHT_VIEWER_CONTEXT_HPP

#include <mutex>
#include <deque>
#include <memory>
#include <unordered_map>

#include <glk/drawble.hpp>
#include <glk/colormap.hpp>
#include <guik/gl_canvas.hpp>
#include <guik/camera/camera_control.hpp>
#include <guik/camera/projection_control.hpp>
#include <guik/viewer/shader_setting.hpp>
#include <guik/viewer/anonymous.hpp>

namespace guik {

class LightViewerContext {
public:
  LightViewerContext(const std::string& context_name);
  virtual ~LightViewerContext();

  void draw_ui();
  void draw_gl();

  bool init_canvas(const Eigen::Vector2i& size);
  void set_size(const Eigen::Vector2i& size);
  void set_clear_color(const Eigen::Vector4f& color);
  void set_pos(const Eigen::Vector2i& pos, ImGuiCond cond = ImGuiCond_FirstUseEver);

  virtual void clear();
  virtual void clear_text();
  virtual void append_text(const std::string& text);
  virtual void register_ui_callback(const std::string& name, const std::function<void()>& callback = 0);

  guik::ShaderSetting& shader_setting() {
    return global_shader_setting;
  }
  const guik::ShaderSetting& shader_setting() const {
    return global_shader_setting;
  }

  void set_draw_xy_grid(bool draw_xy_grid);
  void set_colormap(glk::COLORMAP colormap);
  void set_screen_effect(const std::shared_ptr<glk::ScreenEffect>& effect);

  void enable_normal_buffer();
  void enable_info_buffer();

  bool normal_buffer_enabled() const;
  bool info_buffer_enabled() const;

  const glk::Texture& normal_buffer() const;
  const glk::Texture& info_buffer() const;

  void clear_drawables();
  void clear_drawables(const std::function<bool(const std::string&)>& fn);

  std::pair<ShaderSetting::Ptr, glk::Drawable::ConstPtr> find_drawable(const std::string& name);
  void remove_drawable(const std::string& name);
  void update_drawable(const std::string& name, const glk::Drawable::ConstPtr& drawable, const ShaderSetting& shader_setting = ShaderSetting());

  void clear_drawable_filters();
  void add_drawable_filter(const std::string& filter_name, const std::function<bool(const std::string&)>& filter);
  void remove_drawable_filter(const std::string& filter_name);

  const std::shared_ptr<CameraControl>& get_camera_control() const;
  const std::shared_ptr<ProjectionControl>& get_projection_control() const;
  void set_camera_control(const std::shared_ptr<CameraControl>& camera_control);
  void set_projection_control(const std::shared_ptr<ProjectionControl>& projection_control);

  void reset_center();
  void lookat(const Eigen::Vector3f& pt);
  void use_orbit_camera_control(double distance = 80.0, double theta = 0.0, double phi = -60.0f * M_PI / 180.0f);
  void use_orbit_camera_control_xz(double distance = 80.0, double theta = 0.0, double phi = 0.0);
  void use_topdown_camera_control(double distance = 80.0, double theta = 0.0);

  Eigen::Vector2i canvas_size() const {
    return canvas->size;
  }
  Eigen::Matrix4f view_matrix() const {
    return canvas->camera_control->view_matrix();
  }
  Eigen::Matrix4f projection_matrix() const {
    return canvas->projection_control->projection_matrix();
  }

  Eigen::Vector4i pick_info(const Eigen::Vector2i& p, int window = 2) const;
  float pick_depth(const Eigen::Vector2i& p, int window = 2) const;
  Eigen::Vector3f unproject(const Eigen::Vector2i& p, float depth) const;

protected:
  std::string context_name;
  std::unique_ptr<guik::GLCanvas> canvas;
  guik::ShaderSetting global_shader_setting;

  bool draw_xy_grid;

  std::unordered_map<std::string, std::function<bool(const std::string&)>> drawable_filters;
  std::unordered_map<std::string, std::pair<ShaderSetting::Ptr, glk::Drawable::ConstPtr>> drawables;

  std::mutex sub_texts_mutex;
  std::deque<std::string> sub_texts;
  std::unordered_map<std::string, std::function<void()>> sub_ui_callbacks;
};
}  // namespace guik

#endif