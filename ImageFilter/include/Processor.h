#pragma once

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ImageFilter
{
  class Processor
  {
    class Workspace
    {
      cv::Mat image_;
      std::vector<uint8_t> buffer_;
      bool verbose_ = false;
      int resizeCount_ = 0;

    public:
      Workspace() {}
      ~Workspace();
      void resize(const cv::Mat &inputImage);
      cv::Mat &getImage() { return image_; }
      void setVerbose(bool verbose) { verbose_ = verbose; }
      bool isVerbose() const { return verbose_; }
    };
    static Workspace &getWorkspace()
    {
      static thread_local Workspace workspace;
      return workspace;
    }

  public:
    enum class FilterType
    {
      GAUSSIAN,
      MEDIAN,
      BILATERAL,
      // Unknown filter type
      INVALID
    };

    Processor(FilterType filterType, json config)
        : filterType_(filterType), config_(std::move(config))
    {
    }
    void setVerbose(bool verbose) { verbose_ = verbose;}
    bool isVerbose() const { return verbose_; }
    ~Processor()
    {
    }
    Processor(const Processor &) = delete;
    Processor &operator=(const Processor &) = delete;
    Processor(Processor &&other) noexcept
        : filterType_(std::exchange(other.filterType_, FilterType::GAUSSIAN)),
          config_(std::move(other.config_))
    {
    }
    Processor &operator=(Processor &&other) noexcept
    {
      if (this != &other)
      {
        filterType_ = std::exchange(other.filterType_, FilterType::GAUSSIAN);
        config_ = std::move(other.config_);
      }
      return *this;
    }

    void applyFilter(const cv::Mat &inputImage, const std::string &outFile) const;

    const FilterType &getFilterType() const
    {
      return filterType_;
    }

  private:
    FilterType filterType_;
    json config_;
    bool verbose_{false};
  };

} // namespace ImageFilter
