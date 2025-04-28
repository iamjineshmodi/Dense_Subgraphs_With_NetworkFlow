# Dense Subgraph Detection Algorithms

This repository contains implementations of advanced algorithms for finding densest subgraphs in networks, specifically comparing the Exact Flow-Based approach and the optimized CoreExact algorithm.

## Overview

Dense subgraph detection is a fundamental problem in graph analysis with applications in social network analysis, bioinformatics, and cybersecurity. This project implements and evaluates two state-of-the-art algorithms:

1. **Exact Flow-Based Approach**: A maximum flow-based algorithm that guarantees optimal solutions but faces scalability challenges on large networks.
2. **CoreExact Approach**: An optimized algorithm that combines flow-based techniques with core decomposition to dramatically improve performance while maintaining solution optimality.

## Algorithms

### Exact Flow-Based Approach

- Uses maximum flow techniques to find the mathematically exact densest subgraph
- Employs binary search optimization to converge on optimal density values
- Transforms the density optimization problem into a sequence of maximum flow problems
- Includes clique-based implementation focusing on higher-order connectivity patterns

### CoreExact Approach

- Combines theoretical guarantees of flow-based methods with the efficiency of core decomposition
- Utilizes (k, Ψ)-core decomposition to identify dense regions beforehand
- Reduces search space by focusing flow computations only on relevant dense cores
- Achieves orders of magnitude performance improvement while preserving solution optimality

## Datasets

The algorithms were evaluated on four real-world network datasets:

| Dataset | Nodes | Edges | Density |
|---------|-------|-------|---------|
| Netscience | 1,589 | 2,742 | 0.00217 |
| AS733 | 6,474 | 13,895 | 0.00066 |
| Ca-hepth | 9,877 | 25,998 | 0.00053 |
| AS-caida | 26,475 | 53,381 | 0.00015 |

## Performance Highlights

- CoreExact consistently outperforms the traditional Exact algorithm by orders of magnitude
- Performance improvements range from 12% for small networks to 2200% for large networks
- The advantage increases with graph size and higher h-values (clique size parameter)
- CoreExact maintains solution optimality across all datasets

## Implementation Details

- Efficient implementation of Dinic's algorithm for maximum flow computation
- Optimized clique enumeration techniques
- Cache-efficient data structures for core decomposition
- Early termination strategies and performance optimizations
- Comprehensive complexity analysis for both algorithms


## Key Findings

- The densest subgraphs typically comprise a small fraction of the original graph's vertices
- Density growth with increasing h-value follows approximately exponential patterns
- Networks with similar overall densities can exhibit dramatically different dense subgraph characteristics
- Flow network size is the primary factor affecting algorithm performance

## Future Directions

- Parallelized implementation for further scalability improvement
- Approximation algorithms with theoretical guarantees
- Extensions for dynamic and streaming graphs
- Multi-objective dense subgraph detection

## Contributors

- Om Patil
- Kartik Maheshwari
- Jinesh Modi
- Karan Sethia
- Meghaditya Giri
