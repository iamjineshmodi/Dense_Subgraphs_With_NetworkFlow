# Dense Subgraph Detection Algorithms

This repository contains implementations of advanced algorithms for finding densest subgraphs in networks, specifically comparing the Exact Flow-Based approach and the optimized CoreExact algorithm.

## Overview

Dense subgraph detection is a fundamental problem in graph analysis with applications in:
- Social network analysis (identifying communities)
- Bioinformatics (protein interaction networks)
- Cybersecurity (botnet detection)
- Web graph analysis (link farms)

This project implements and evaluates two state-of-the-art algorithms:

1. **Exact Flow-Based Approach (daa1.cpp)**: A maximum flow-based algorithm that guarantees optimal solutions but faces scalability challenges on large networks.
2. **CoreExact Approach (daa2.cpp)**: An optimized algorithm that combines flow-based techniques with core decomposition to dramatically improve performance while maintaining solution optimality.

## Algorithms

### Exact Flow-Based Approach

- Uses maximum flow techniques to find the mathematically exact densest subgraph
- Employs binary search optimization to converge on optimal density values
- Transforms the density optimization problem into a sequence of maximum flow problems
- Includes clique-based implementation focusing on higher-order connectivity patterns
- Time complexity: O(mn²) where m is the number of edges and n is the number of vertices

### CoreExact Approach

- Combines theoretical guarantees of flow-based methods with the efficiency of core decomposition
- Utilizes (k, Ψ)-core decomposition to identify dense regions beforehand
- Reduces search space by focusing flow computations only on relevant dense cores
- Achieves orders of magnitude performance improvement while preserving solution optimality
- Time complexity: O(k·m·log n) where k is the core number, significantly faster for large graphs


## Compilation Instructions

### Exact Algorithm (daa1.cpp)

```bash
# Compile with optimization flags
g++ -O3 daa1.cpp -o daa1
```

### CoreExact Algorithm (daa2.cpp)

```bash
# Compile with optimization flags
g++ -O3 daa2.cpp -o daa2
```

## Execution Instructions

### Exact Algorithm (daa1)

```bash
# Run the algorithm
./daa1

# When prompted, enter the dataset name WITHOUT the .txt extension:
# Example: Netscience (not Netscience.txt)
```

### CoreExact Algorithm (daa2)

```bash
# Run the algorithm
./daa2

# When prompted, enter the dataset name WITH the .txt extension:
# Example: Netscience.txt
```

## Datasets

The algorithms were evaluated on four real-world network datasets:

| Dataset | Nodes | Edges | Density | Description |
|---------|-------|-------|---------|-------------|
| Netscience | 1,589 | 2,742 | 0.00217 | Collaboration network of network science researchers |
| AS733 | 6,474 | 13,895 | 0.00066 | Autonomous systems network |
| Ca-hepth | 9,877 | 25,998 | 0.00053 | Collaboration network for high energy physics theory |
| AS-caida | 26,475 | 53,381 | 0.00015 | CAIDA AS relationship dataset |

### Dataset Download Links

- [AS-caida](https://drive.google.com/file/d/1ZOsB_nsFdwWK4FDbBEF_7S6DfYiML2FI/view?usp=drive_link)
- [AS733](https://drive.google.com/file/d/1UPQD69f7VLQTl-e3kI3jUH1ecM7lOhVO/view?usp=drive_link)
- [Ca-hepth](https://drive.google.com/file/d/1ebJ0e3PHX_KI3vmsYKFToi1kTdsAxQ8x/view?usp=drive_link)
- [Netscience](https://drive.google.com/file/d/1PJCkBiPKrLOA7_TTzKsKSnbnAR9F7EIY/view?usp=drive_link)

### Important Note on Dataset Format

- For daa1 (Exact Algorithm): Place datasets in the same directory without the `.txt` extension
- For daa2 (CoreExact Algorithm): Place datasets in the same directory with the `.txt` extension

### Expected Dataset Format

```
# First line: number of nodes (n) and edges (m)
n m
# Following m lines: edge pairs (node1 node2)
node1 node2
node1 node3
...
```

## Performance Highlights

- CoreExact consistently outperforms the traditional Exact algorithm by orders of magnitude
- Performance improvements range from 12% for small networks to 2200% for large networks
- The advantage increases with graph size and higher h-values (clique size parameter)
- CoreExact maintains solution optimality across all datasets

| Dataset | Exact Runtime (s) | CoreExact Runtime (s) | Improvement (%) |
|---------|-------------------|----------------------|-----------------|
| Netscience | 18.2 | 16.0 | 12% |
| AS733 | 147.8 | 58.3 | 153% |
| Ca-hepth | 465.2 | 102.4 | 354% |
| AS-caida | 2,892.5 | 126.2 | 2,193% |

## Implementation Details

- Efficient implementation of Dinic's algorithm for maximum flow computation
- Optimized clique enumeration techniques
- Cache-efficient data structures for core decomposition
- Early termination strategies and performance optimizations
- Comprehensive complexity analysis for both algorithms

### Output Format

Both algorithms output:
- The densest subgraph (list of vertices)
- The density of the subgraph
- Runtime statistics

## Key Findings

- The densest subgraphs typically comprise a small fraction of the original graph's vertices
- Density growth with increasing h-value follows approximately exponential patterns
- Networks with similar overall densities can exhibit dramatically different dense subgraph characteristics
- Flow network size is the primary factor affecting algorithm performance

## Visualization

The repository includes visualization tools for:
- Comparing algorithm performance across datasets
- Visualizing the detected dense subgraphs
- Analyzing the relationship between core number and density

## Future Directions

- Parallelized implementation for further scalability improvement
- Approximation algorithms with theoretical guarantees
- Extensions for dynamic and streaming graphs
- Multi-objective dense subgraph detection
- GPU-accelerated implementations for massive graphs

## Contributors

- **Om Patil**: Implementation of Exact Algorithm and Website Developer
- **Kartik Maheshwari**: Implementation of Exact and CoreExact Algorithm
- **Jinesh Modi**: Implementation of CoreExact Algorithm and Website UI Designer
- **Meghaditya Giri**: Algorithm visualization, CoreExact debugging, and compiled the project report
- **Karan Sethia**: Debugging of Exact Algorithm and documentation

