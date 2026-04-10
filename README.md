# CPU–GPU Compatibility CLI (C)

A modular C-based command-line tool that recommends CPU–GPU pairings and analyzes system balance using model-aware scoring and a data-driven hardware database.

## Features

- CPU → GPU recommendations based on performance tier  
- GPU → CPU recommendations with exact model matching  
- Pairing analysis with bottleneck / overkill detection  
- Model-level scoring (e.g., i5-12400F vs i5-12600K)  
- Data-driven GPU database loaded from an external file  
- Robust input parsing and validation  

## Example

Input:  
Intel i5 12400F + RTX 5060 Ti  

Output:  
Mild CPU bottleneck risk  

## How It Works

The system evaluates compatibility using a two-layer model:

1. Tier Classification  
   ENTRY, MID, HIGH, ENTHUSIAST  

2. Model-Based Scoring  
   CPUs and GPUs are assigned scores based on their model numbers  
   Differences in scores determine pairing quality  

diff = CPU_score - GPU_score  

This allows the program to detect:
- Balanced systems  
- CPU bottlenecks  
- GPU underutilization (CPU overkill)  

## Project Structure

.
├── src/
│   ├── main.c  
│   ├── hardware.c  
│   ├── database.c  
│   ├── recommend.c  
│
├── include/
│   ├── hardware.h  
│   ├── database.h  
│   ├── recommend.h  
│
├── gpu_data.txt  
├── README.md  

## Data-Driven Design

GPU data is loaded at runtime from `gpu_data.txt`, allowing the system to:

- Update hardware recommendations without recompiling  
- Separate logic from data  
- Scale easily as new hardware is added  

## Compilation

gcc src/*.c -Iinclude -o pcproject  

## Run

./pcproject  

## Example Inputs

- Intel i5 12400F  
- Intel i5 12600K  
- Ryzen 5 7600  
- RTX 4060  
- RTX 5060 Ti  
- RX 7800 XT  

## Design Highlights

- Modular multi-file C architecture  
- String parsing and normalization for flexible input handling  
- Model-aware scoring system for realistic recommendations  
- File-based data loading for extensibility  

## Future Improvements

- CPU database file loading (similar to GPU system)  
- More granular scoring based on benchmarks  
- Workload-based recommendations (gaming vs productivity)  

## Author

Saba Karaman
