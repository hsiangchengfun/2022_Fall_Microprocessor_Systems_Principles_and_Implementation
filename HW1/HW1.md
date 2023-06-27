# HW1
## Profiling Using CPU Hardware

### HW Goal
* In this homework, you will **add profiling hardware** to the Aquila core to analyze the program execution behavior
* CoreMark benchmark will be used as the target
    *  You will learn 
        1. how to profile a program on the real platform
        2. how to use Xilinx Integrated Logic Analyzer (ILA) for real-time debugging

### Synthesis-Execution Flow
* To run the HW-SW system on an FPGA, you must:
    1.  Generate the HW bit file
    2. Power on the Arty board
    3. Use a terminal program to connect to the FPGA via UART
    4. Program the FPGA
    5. Send an ELF program to the FPGA via UART
    6. Wait for the program to execute and print results


### Analyze the Execution of Aquila SoC
* To analyze the behavior of Aquila, you can use a RTL simulator or the Integrated Logic Analyzer (ILA)
* The problem with a RTL simulator
    *  Simulation of the execution of a program is very slow
    * On-chip memory must be initialized with the program
* Real-time ILA circuit probing:
    * Embed signal probes into your circuit
    * Set a trigger condition to capture signal traces to on-chip RAM
    * Perform a post-mortem analysis on a PC afterwards

### To Do
* Add HW code to Aquila such that it can collect the runtime profiling data for the five hotspots
    *  count the total CPU cycles and compute the CPU ratio of each function
    * calculate the ratio of computation versus memory cycles for each function

