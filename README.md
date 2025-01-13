# Image Quilting

This project was an exam submission for the *Tool- und Plugin-Programmierung* module, and I received a grade of **1.0**.

This project implements advanced **image quilting techniques** to generate large, visually coherent textures from small input patches. The method minimizes visible seams and repetitive patterns using algorithms such as **Minimum Error Boundary Cut** and **Mean Squared Error (MSE)**-based block selection.

## Features
- Seamless integration of texture patches from input images.
- Optimization of texture coherence using overlap regions and minimal cuts.
- Flexible parameters for block size, overlap, and tolerance, enabling diverse texture outputs.
- Applications in **computer graphics**, **game design**, and **digital art**.

## Project Structure
- **`include/`**: Header files defining core interfaces (e.g., `TextureQuilter.h`, `SeamCarving.h`, `ErrorMetrics.h`).
- **`src/`**: Implementation of core algorithms (e.g., quilting logic, seam carving, and error metrics).
- **`gallery/`**: Input and output images demonstrating the quilting process.
- **`CMakeLists.txt`**: Build configuration file.

## Technologies
- **C++** 
- **CMake** For build automation.
- **Image Processing:** Implementation of Efros and Freeman’s Image Quilting Algorithm.

## Documentation

For detailed project documentation, please refer to the `Ausarbeitung` folder.

## Usage
Compile the project using CMake and run the executable with specified parameters:

```bash
./ImageQuilting_main --input <input_image> \
                     --output <output_image> \
                     --width <output_width> \
                     --height <output_height> \
                     --tileW <tile_width> \
                     --tileH <tile_height> \
                     --seamW <seam_width> \
                     --seamH <seam_height> \
                     --mseSelect true \
                     --minCut true \
                     --tolerance 0.1



