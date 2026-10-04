# ThreadLocalMemoryPool

A high-performance C++ image processing application that leverages thread-local memory pools for efficient image filtering across multiple threads.

## Overview

ThreadLocalMemoryPool is an image processing framework designed to apply various filters to images using OpenCV. The core innovation is the use of thread-local storage to manage per-thread memory buffers, minimizing memory allocations and improving cache locality in multi-threaded environments.

## Key Features

- **Thread-Local Memory Management**: Each thread maintains its own memory workspace, reducing contention and improving performance
- **Multiple Filter Types**: Supports Gaussian, Median, and Bilateral filtering algorithms
- **Batch Processing**: Process multiple images with various filters in a single configuration
- **JSON-Based Configuration**: Easy-to-use JSON format for specifying input files, output directories, and filter parameters
- **Verbose Logging**: Optional verbose mode to track memory allocations and filter operations
- **Original File Preservation**: Option to copy original images alongside filtered outputs

## Architecture

### Core Components

#### Processor Class
The main image processing engine located in `ImageFilter/include/Processor.h`:
- Manages filter type and configuration
- Applies filters to input images
- Thread-safe filter application

#### Workspace (Thread-Local Storage)
An inner class within `Processor` that manages per-thread resources:
- Maintains thread-local image buffers
- Handles buffer resizing to match input image dimensions
- Tracks the number of memory resizes per thread
- Provides verbose output for memory management diagnostics

#### Supported Filters

1. **Gaussian Blur**: Smooth filtering using a Gaussian kernel
   - Parameters: `kernel_size`, `sigma`

2. **Median Filter**: Non-linear filtering that replaces pixels with the median value
   - Parameters: `kernel_size`

3. **Bilateral Filter**: Edge-preserving smoothing filter
   - Parameters: `kernel_size`, `sigma_color`, `sigma_space`

## Configuration

Images and filters are configured using a JSON file. Example configuration:

```json
{
    "images": ["/path/to/images/"],
    "copy_original": true,
    "thread_on": "p",
    "output": "/path/to/output",
    "wait_before_start_s": 0,
    "filters": [
        {
            "type": "GAUSSIAN",
            "config": {
                "kernel_size": 5,
                "sigma": 0
            },
            "output_postfix": "_gaussian"
        },
        {
            "type": "MEDIAN",
            "config": {
                "kernel_size": 5
            },
            "output_postfix": "_median"
        },
        {
            "type": "BILATERAL",
            "config": {
                "kernel_size": 9,
                "sigma_color": 75,
                "sigma_space": 75
            },
            "output_postfix": "_bilateral"
        }
    ]
}
```

### Threading Modes

The `thread_on` parameter controls how threads are allocated during processing:

- **`image` or `i`**: Create one thread per input image
  - Each image is processed by a dedicated thread
  - All filters are applied to that image sequentially within the thread
  - Useful when you have many images and want parallel image processing

- **`processor` or `p`**: Create one thread per image processor (filter)
  - Each filter runs in its own thread
  - All images are processed sequentially by each filter's thread
  - Useful when you have few images but many filters
  - Better leverages thread-local memory pools when filters process images in sequence

| Parameter | Type | Description |
|-----------|------|-------------|
| `images` | Array | List of directories containing input images |
| `copy_original` | Boolean | Whether to copy original images to output directory |
| `thread_on` | String | Threading mode: `image` or `i` (create thread per input image), or `processor` or `p` (create thread per image processor) |
| `output` | String | Output directory path for filtered images |
| `wait_before_start_s` | Integer | Delay in seconds before processing starts |
| `filters` | Array | Array of filter configurations to apply |

## Usage

```bash
./memoryPool --input <config.json> [--verbose true]
```

### Command-Line Arguments

- `--input, -i`: Path to the JSON configuration file (required)
- `--verbose, -vb`: Enable verbose mode for detailed logging (optional, default: false)

### Example

```bash
./memoryPool -i conf/input.json -vb true
```

## Building

The project uses CMake for building:

```bash
mkdir build
cd build
cmake ..
cmake --build .
cmake --install .
```

The compiled executable will be available in the installation directory.

## Memory Optimization

The thread-local memory pool pattern provides several benefits:

1. **Reduced Allocations**: Buffers are allocated once per thread and reused across images
2. **Improved Cache Locality**: Thread-local data stays in CPU cache
3. **Minimal Contention**: No inter-thread synchronization needed for memory access
4. **Scalability**: Performance scales better with increasing thread count

## Output

Processed images are saved to the configured output directory with naming convention:
- `{original_filename}{output_postfix}.{extension}`

For example, processing `photo.jpg` with a Gaussian filter using postfix `_gaussian` produces `photo_gaussian.jpg`.

## Supported Image Formats

- JPEG (.jpg, .jpeg)
- PNG (.png)
- BMP (.bmp)
- TIFF (.tiff)

## Performance Considerations

- Adjust `kernel_size` for filters to balance quality and performance
- Use verbose mode to monitor memory resize operations
- Larger images may require more memory per thread
- Thread count should typically match the number of available CPU cores

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

## Project Structure

```
ThreadLocalMemoryPool/
├── ImageFilter/           # Main image processing library
│   ├── include/          # Public headers
│   └── src/              # Implementation
├── main/                  # Application entry point
├── conf/                  # Configuration files
├── data/                  # Sample data
├── output/                # Output directory (created at runtime)
├── CMakeLists.txt        # Build configuration
└── README.md             # This file
```

## Notes

- The Workspace class uses `pthread_getname_np` for thread identification in verbose mode
- Thread-local storage is managed via static initialization, ensuring thread safety
- Move semantics are implemented for the Processor class to enable efficient resource transfer
