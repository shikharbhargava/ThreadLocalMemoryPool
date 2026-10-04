#include "Processor.h"
#include "utils.h"

using namespace ImageFilter;

using FilterType = Processor::FilterType;
static FilterType GAUSSIAN = FilterType::GAUSSIAN;
static FilterType MEDIAN = FilterType::MEDIAN;
static FilterType BILATERAL = FilterType::BILATERAL;
static FilterType INVALID = FilterType::INVALID;

template <>
std::string utils::to_string<FilterType>(const FilterType &filterType)
{
  switch (filterType)
  {
  case FilterType::GAUSSIAN:
    return "GAUSSIAN";
  case FilterType::MEDIAN:
    return "MEDIAN";
  case FilterType::BILATERAL:
    return "BILATERAL";
  default:
    return "INVALID";
  }
}

template <>
const FilterType &utils::from_string<FilterType>(const std::string &str)
{
  if (str == "GAUSSIAN")
    return GAUSSIAN;
  if (str == "MEDIAN")
    return MEDIAN;
  if (str == "BILATERAL")
    return BILATERAL;
  return INVALID;
}

template <>
const FilterType &utils::from_string<FilterType>(const char *str)
{
  return utils::from_string<FilterType>(std::string(str));
}

void Processor::Workspace::resize(const cv::Mat &inputImage)
{
  char thread_name[16] = {0};
  pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
  size_t requiredSize = inputImage.total() * inputImage.elemSize();
  if (verbose_)
  {
    std::cout << "[" << thread_name << "]\t" << "Input image size: " << inputImage.size() << ", Required buffer size: " << requiredSize << ", Current buffer capacity: " << buffer_.capacity() << std::endl;
  }
  if (buffer_.capacity() < requiredSize)
  {
    if (verbose_)
    {
      std::cout << "[" << thread_name << "]\t" << "Resizing buffer to match input image size." << std::endl;
    }
    buffer_.reserve(requiredSize);
    ++resizeCount_;
  }
  buffer_.resize(requiredSize);
  image_ = cv::Mat(
      inputImage.rows,
      inputImage.cols,
      inputImage.type(),
      buffer_.data());
}

Processor::Workspace::~Workspace()
{
  if (verbose_)
  {
    char thread_name[16] = {0};
    pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
    std::cout << "[" << thread_name << "]\t" << "Workspace destroyed after " << resizeCount_ << " resizes." << std::endl;
  }
}

void Processor::applyFilter(const cv::Mat &inputImage, const std::string &outFile) const
{
  std::string filterTypeStr = utils::to_string(filterType_);
  auto &workspace = getWorkspace();
  workspace.setVerbose(verbose_);
  workspace.resize(inputImage);
  cv::Mat &outputImageMat = workspace.getImage();
  switch (filterType_)
  {
  case FilterType::GAUSSIAN:
  {
    int kernel_size_gaussian = config_.value("kernel_size", 5);
    int sigma = config_.value("sigma", 0);
    cv::GaussianBlur(inputImage, outputImageMat, cv::Size(kernel_size_gaussian, kernel_size_gaussian), sigma);
    break;
  }
  case FilterType::MEDIAN:
  {
    int kernel_size_median = config_.value("kernel_size", 5);
    cv::medianBlur(inputImage, outputImageMat, kernel_size_median);
    break;
  }
  case FilterType::BILATERAL:
  {
    int kernel_size_bilateral = config_.value("kernel_size", 9);
    int sigma_color = config_.value("sigma_color", 75);
    int sigma_space = config_.value("sigma_space", 75);
    cv::bilateralFilter(inputImage, outputImageMat, kernel_size_bilateral, sigma_color, sigma_space);
    break;
  }
  }
  char thread_name[16] = {0};
  pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
  std::cout << "[" << thread_name << "]\t" << "[" << filterTypeStr << "]\t" << "Saving processed image to: " << outFile << std::endl;
  cv::imwrite(outFile, outputImageMat);
}
