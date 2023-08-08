# FirmwareChallenge

# Vehicle Positions Optimization Challenge

This repository contains an optimized version of the Vehicle Positions code challenge for MiX Telematics. The code has been optimized for parallel processing, compiler optimization flags, and chunking to achieve better performance.

## Instructions

Follow these steps to compile and run the optimized version of the code on the command window:


### Prerequisites

- Make sure you have a C compiler (such as GCC) installed on your system.

### Compilation

1. Open a command window and navigate to the directory containing the source code (`solution.c`) and the input data file (`VehiclePositions.dat`).

2. Use the following command to compile the code with the necessary optimization flags:
   ```bash
   gcc -o solution solution.c -fopenmp -march=native -ffast-math -funroll-loops -O3
   ```

### Execution

1. After successful compilation, you can run the compiled executable using the following command:
   ```bash
   solution
   ```

### Output

The program will execute and display the closest position IDs for each given coordinate along with the execution time.


## Contact

If you have any questions or need further assistance, please contact Mfundo at mfundohmbambo@gmail.com.

---
