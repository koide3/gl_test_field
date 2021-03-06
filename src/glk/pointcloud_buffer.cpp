#include <glk/pointcloud_buffer.hpp>

#include <iostream>
#include <glk/colormap.hpp>

namespace glk {

PointCloudBuffer::PointCloudBuffer(int stride, int num_points) {
  this->stride = stride;
  this->num_points = num_points;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, stride * num_points, nullptr, GL_STATIC_DRAW);
}

PointCloudBuffer::PointCloudBuffer(const float* data, int stride, int num_points) {
  this->stride = stride;
  this->num_points = num_points;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, stride * num_points, data, GL_STATIC_DRAW);
}

PointCloudBuffer::PointCloudBuffer(const Eigen::Matrix<float, 3, -1>& points) : PointCloudBuffer(points.data(), sizeof(Eigen::Vector3f), points.cols()) {}

PointCloudBuffer::PointCloudBuffer(const Eigen::Matrix<double, 3, -1>& points) : PointCloudBuffer(points.cast<float>().eval()) {}

PointCloudBuffer::PointCloudBuffer(const std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>& points) : PointCloudBuffer(Eigen::Map<const Eigen::Matrix<float, 3, -1>>(points.front().data(), 3, points.size()).eval()) {}

PointCloudBuffer::PointCloudBuffer(const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>>& points) : PointCloudBuffer(Eigen::Map<const Eigen::Matrix<double, 3, -1>>(points.front().data(), 3, points.size()).eval()) {}

PointCloudBuffer::~PointCloudBuffer() {
  glDeleteVertexArrays(1, &vao);
  for(const auto& aux : aux_buffers) {
    glDeleteBuffers(1, &aux.buffer);
  }
  glDeleteBuffers(1, &vbo);
}

void PointCloudBuffer::add_normals(const std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>& normals) {
  add_normals(normals[0].data(), sizeof(Eigen::Vector3f), normals.size());
}

void PointCloudBuffer::add_color(const std::vector<Eigen::Vector4f, Eigen::aligned_allocator<Eigen::Vector4f>>& colors) {
  add_color(colors[0].data(), sizeof(Eigen::Vector4f), colors.size());
}

void PointCloudBuffer::add_intensity(glk::COLORMAP colormap, const std::vector<float>& intensities, float scale) {
  add_intensity(colormap, intensities.data(), sizeof(float), intensities.size(), scale);
}

void PointCloudBuffer::add_intensity(glk::COLORMAP colormap, const std::vector<double>& intensities, float scale) {
  std::vector<float> intensities_(intensities.size());
  std::copy(intensities.begin(), intensities.end(), intensities_.begin());
  add_intensity(colormap, intensities_, scale);
}

void PointCloudBuffer::add_normals(const float* data, int stride, int num_points) {
  add_buffer("vert_normal", 3, data, stride, num_points);
}

void PointCloudBuffer::add_color(const float* data, int stride, int num_points) {
  add_buffer("vert_color", 4, data, stride, num_points);
}

void PointCloudBuffer::add_intensity(glk::COLORMAP colormap, const float* data, int stride, int num_points, float scale) {
  std::vector<Eigen::Vector4f, Eigen::aligned_allocator<Eigen::Vector4f>> colors(num_points);
  for(int i = 0; i < num_points; i++) {
    colors[i] = glk::colormapf(colormap, scale * data[(stride / sizeof(float)) * i]);
  }

  add_color(colors[0].data(), sizeof(Eigen::Vector4f), num_points);
}

void PointCloudBuffer::add_buffer(const std::string& attribute_name, int dim, const float* data, int stride, int num_points) {
  assert(this->num_points == num_points);

  glBindVertexArray(vao);

  GLuint buffer_id;
  glGenBuffers(1, &buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, stride * num_points, data, GL_STATIC_DRAW);

  aux_buffers.push_back(AuxBufferData{attribute_name, dim, stride, buffer_id});
}

void PointCloudBuffer::draw(glk::GLSLShader& shader) const {
  if(num_points == 0) {
    return;
  }

  GLint position_loc = shader.attrib("vert_position");

  glBindVertexArray(vao);
  glEnableVertexAttribArray(position_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, stride, 0);

  for(const auto& aux : aux_buffers) {
    GLint attrib_loc = shader.attrib(aux.attribute_name);
    glEnableVertexAttribArray(attrib_loc);
    glBindBuffer(GL_ARRAY_BUFFER, aux.buffer);
    glVertexAttribPointer(attrib_loc, aux.dim, GL_FLOAT, GL_FALSE, aux.stride, 0);
  }

  glDrawArrays(GL_POINTS, 0, num_points);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(position_loc);
  for(const auto& aux : aux_buffers) {
    glDisableVertexAttribArray(shader.attrib(aux.attribute_name));
  }
}

GLuint PointCloudBuffer::vba_id() const {
  return vao;
}
GLuint PointCloudBuffer::vbo_id() const {
  return vbo;
}

int PointCloudBuffer::get_aux_size() const {
  return aux_buffers.size();
}

const AuxBufferData& PointCloudBuffer::get_aux_buffer(int i) const {
  return aux_buffers[i];
}

}  // namespace glk
