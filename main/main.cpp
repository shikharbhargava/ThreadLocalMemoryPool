#include <iostream>
#include <fstream>
#include <thread>
#include <filesystem>
#include <set>
#include <unordered_set>
#include <vector>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <ArgParser.h>
#include <utils.h>

#include <Processor.h>
using Processor = ImageFilter::Processor;
using FilterType = Processor::FilterType;

static std::unordered_set<std::string> imagesExtensions = {".jpg", ".jpeg", ".png", ".bmp", ".tiff"};

static std::unordered_set<size_t> getRandomIndices(const size_t &count)
{
  std::unordered_set<size_t> indices;
  // Set up modern random number generation
  std::uniform_int_distribution<size_t> dist(0, count - 1);

  // Keep generating until we have enough unique indices
  while (indices.size() < count)
  {
    indices.insert(dist(gen));
  }

  return indices;
}

static std::string getOutputFilePath(const std::string &outputDir,
                                     const std::string &outputPostfix,
                                     const std::string &inputFileName)
{
  // construct the output file path by combining as follows: the output directory + input file name without extension + output postfix + the original file extension
  std::filesystem::path inputPath(inputFileName);
  std::string inputFileNameWithoutExtension = inputPath.stem().string();
  std::string inputFileExtension = inputPath.extension().string();
  std::filesystem::path outputPath = std::filesystem::path(outputDir) / (inputFileNameWithoutExtension + outputPostfix + inputFileExtension);
  return outputPath.string();
}

int main(int argc, const char **argv)
{
  Arguments::ArgParser parser;
  std::string inputPath;
  bool verbose = false;
  try
  {
    parser.addOption("input", "i", Arguments::ArgTypes::FILE, "Input json file.", true, true);
    parser.addOption("verbose", "vb", Arguments::ArgTypes::BOOL, "Verbose mode.", false, false, "false");
    parser.parse(argc, argv);
    inputPath = parser.get<std::string>("input");
    verbose = parser.get<bool>("verbose");
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    parser.printHelp();
    return 1;
  }

  json config;
  std::ifstream inputFile(inputPath);
  std::vector<std::string> inputImages;
  std::vector<std::pair<std::shared_ptr<Processor>, std::string>> processors;
  std::string outputDir;
  bool copyOriginal = false;
  if (inputFile.is_open())
  {
    inputFile >> config;
  }
  else
  {
    std::cerr << "Failed to open input file: " << inputPath << std::endl;
    return 1;
  }

  try
  {
    copyOriginal = config.value("copy_original", false);
    json images = config.value("images", json::array());
    if (images.empty())
    {
      std::cerr << "No images specified in the input configuration." << std::endl;
      return 1;
    }
    json filters = config.value("filters", json::array());
    if (filters.empty())
    {
      std::cerr << "No filters specified in the input configuration." << std::endl;
      return 1;
    }

    for (const auto &image : images)
    {
      std::string imagePath = image.get<std::string>();
      // if imagePath is a directory, find all image files within it recursively
      if (std::filesystem::is_directory(imagePath))
      {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(imagePath))
        {
          // only add files with valid image extensions
          if (entry.is_regular_file())
          {
            std::string extension = entry.path().extension().string();
            if (imagesExtensions.find(extension) != imagesExtensions.end())
            {
              inputImages.push_back(entry.path().string());
            }
          }
        }
      }
      else if (std::filesystem::is_regular_file(imagePath))
      {
        std::string extension = std::filesystem::path(imagePath).extension().string();
        if (imagesExtensions.find(extension) != imagesExtensions.end())
        {
          inputImages.push_back(imagePath);
        }
      }
    }

    outputDir = config.value("output", "");
    // use filesystem to ensure output directory exists
    if (!outputDir.empty() && !std::filesystem::exists(outputDir))
    {
      std::filesystem::create_directories(outputDir);
    }

    processors.reserve(filters.size());
    for (const auto &filter : filters)
    {
      std::string typeStr = filter.value("type", "INVALID");
      FilterType filterType = utils::from_string<FilterType>(typeStr);
      if (filterType == FilterType::INVALID)
        continue;
      if (filter.count("config") == 0 || !filter["config"].is_object())
        continue;
      const json &config = filter["config"];
      std::string outputPostfix = filter.value("output_postfix", typeStr + "_");
      auto processorPtr = std::make_shared<Processor>(filterType, config);
      processorPtr->setVerbose(verbose);
      processors.emplace_back(std::make_pair(processorPtr, outputPostfix));
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  int waitBeforeStart = config.value("wait_before_start_s", 0);
  if (waitBeforeStart > 0)
  {
    std::cout << "Waiting for " << waitBeforeStart << " seconds before starting..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(waitBeforeStart));
  }
  std::string thread_on = config.value("thread_on", "image");
  std::transform(thread_on.begin(), thread_on.end(), thread_on.begin(), ::tolower);

  std::vector<std::thread> threads;
  if (thread_on == "image" || thread_on == "i")
  {
    if (verbose)
      std::cout << "Launching threads for images..." << std::endl;
    threads.reserve(inputImages.size());
    std::unordered_set<size_t> imageIndices = getRandomIndices(inputImages.size());
    for (const auto &index : imageIndices)
    {
      if (verbose)
        std::cout << "Launching thread for image index: " << index << std::endl;
      threads.emplace_back([&processors, &inputImages, &outputDir, copyOriginal, index]()
                           {
        const auto &inputFile = inputImages[index];
        std::string threadName = "image#" + std::to_string(index);
        pthread_setname_np(pthread_self(), threadName.c_str());
        std::unordered_set<size_t> processorIndices = getRandomIndices(processors.size());
        for (const auto &index : processorIndices)
        {
          const auto &processor = processors[index];
          std::string outDir = outputDir;
          if (outDir.empty())
          {
            outDir = std::filesystem::path(inputFile).parent_path().string();
          }
          std::string outFile = getOutputFilePath(outDir, processor.second, inputFile);
          processor.first->applyFilter(cv::imread(inputFile), outFile);
          if (copyOriginal)
          {
            std::string originalOutFile = getOutputFilePath(outDir, "", inputFile);
            std::filesystem::copy_file(inputFile, originalOutFile, std::filesystem::copy_options::overwrite_existing);
          }
        } });
    }
  }
  else if (thread_on == "processor" || thread_on == "p")
  {
    if (verbose)
      std::cout << "Launching threads for processors..." << std::endl;
    threads.reserve(processors.size());
    std::unordered_set<size_t> processorIndices = getRandomIndices(processors.size());
    for (const auto &index : processorIndices)
    {
      if (verbose)
        std::cout << "Launching thread for processor index: " << index << std::endl;
      threads.emplace_back([&processor = processors[index], &inputImages, &outputDir, copyOriginal]()
                           {
        std::string threadName = utils::to_string(processor.first->getFilterType());
        pthread_setname_np(pthread_self(), threadName.c_str());
        std::unordered_set<size_t> processedIndices = getRandomIndices(inputImages.size());
        for (const auto &index : processedIndices)
        {
          const auto &inputFile = inputImages[index];
          std::string outDir = outputDir;
          if (outDir.empty())
          {
            outDir = std::filesystem::path(inputFile).parent_path().string();
          }
          std::string outFile = getOutputFilePath(outDir, processor.second, inputFile);
          processor.first->applyFilter(cv::imread(inputFile), outFile);
          if (copyOriginal)
          {
            std::string originalOutFile = getOutputFilePath(outDir, "", inputFile);
            // using filesystem to copy the original file to the output directory
            std::filesystem::copy_file(inputFile, originalOutFile, std::filesystem::copy_options::overwrite_existing);
          }
        } });
    }
  }
  else
  {
    std::cerr << "Invalid thread_on value: " << thread_on << std::endl;
    return 1;
  }
  for (auto &t : threads)
  {
    if (t.joinable())
      t.join();
  }

  return 0;
}