#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <limits>
#include <random>
#include <cmath>
#include <functional>

using TimePoint = std::chrono::high_resolution_clock::time_point;

// Forward declarations
class Graph;
class MaxFlowSolver;

/**
 * Simple timer utility for performance measurement
 */
class Timer {
private:
    TimePoint start_time;
    std::string operation_name;

public:
    Timer(const std::string& name) : operation_name(name) {
        start_time = std::chrono::high_resolution_clock::now();
        std::cout << "⏱️ Starting " << operation_name << "..." << std::endl;
    }

    ~Timer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "✓ " << operation_name << " completed in " 
                  << std::fixed << std::setprecision(2) << duration.count() / 1000.0 
                  << " seconds" << std::endl;
    }
};

/**
 * Edge representation for the graph
 */
struct Edge {
    int source;
    int target;
    double weight;
    
    Edge(int src, int tgt, double w = 1.0) : source(src), target(tgt), weight(w) {}
    
    bool operator==(const Edge& other) const {
        return source == other.source && target == other.target;
    }
};

/**
 * Hash function for Edge to use in unordered containers
 */
namespace std {
    template<>
    struct hash<Edge> {
        size_t operator()(const Edge& e) const {
            return hash<int>()(e.source) ^ hash<int>()(e.target);
        }
    };
}

/**
 * Graph class representing undirected networks
 */
class Graph {
private:
    std::unordered_map<int, std::unordered_set<int>> adjacency_lists;
    std::unordered_set<Edge> edge_set;
    std::unordered_map<int, int> vertex_ids;  // Original ID to sequential ID
    std::unordered_map<int, int> reverse_ids; // Sequential ID to original ID
    int next_id = 0;
    
    // Get or assign a sequential ID for a vertex
    int getVertexId(int original_id) {
        if (vertex_ids.find(original_id) == vertex_ids.end()) {
            vertex_ids[original_id] = next_id;
            reverse_ids[next_id] = original_id;
            next_id++;
        }
        return vertex_ids[original_id];
    }

public:
    // Add an edge to the graph
    void addEdge(int src, int tgt, double weight = 1.0) {
        if (src == tgt) return; // Skip self-loops
        
        int src_id = getVertexId(src);
        int tgt_id = getVertexId(tgt);
        
        adjacency_lists[src_id].insert(tgt_id);
        adjacency_lists[tgt_id].insert(src_id);
        edge_set.insert(Edge(src_id, tgt_id, weight));
    }
    
    // Create a subgraph from a set of vertices
    Graph createSubgraph(const std::unordered_set<int>& vertices) const {
        Graph subgraph;
        
        // Track mapping from original vertex IDs to subgraph IDs
        std::unordered_map<int, int> subgraph_mapping;
        
        // Add vertices and map them
        for (int v : vertices) {
            if (adjacency_lists.find(v) != adjacency_lists.end()) {
                subgraph_mapping[v] = subgraph.getVertexId(reverse_ids.at(v));
            }
        }
        
        // Add edges where both endpoints are in the subgraph
        for (int v : vertices) {
            if (adjacency_lists.find(v) == adjacency_lists.end()) continue;
            
            for (int neighbor : adjacency_lists.at(v)) {
                if (vertices.find(neighbor) != vertices.end() && v < neighbor) {
                    subgraph.addEdge(reverse_ids.at(v), reverse_ids.at(neighbor));
                }
            }
        }
        
        return subgraph;
    }
    
    // Get all vertices in the graph
    std::vector<int> getVertices() const {
        std::vector<int> vertices;
        for (const auto& kv : adjacency_lists) {
            vertices.push_back(kv.first);
        }
        return vertices;
    }
    
    // Get all vertices with original IDs
    std::vector<int> getOriginalVertices() const {
        std::vector<int> vertices;
        for (const auto& kv : adjacency_lists) {
            vertices.push_back(reverse_ids.at(kv.first));
        }
        return vertices;
    }
    
    // Get neighbors of a vertex
    const std::unordered_set<int>& getNeighbors(int vertex) const {
        static const std::unordered_set<int> empty_set;
        auto it = adjacency_lists.find(vertex);
        return (it != adjacency_lists.end()) ? it->second : empty_set;
    }
    
    // Check if two vertices are adjacent
    bool areAdjacent(int v1, int v2) const {
        auto it = adjacency_lists.find(v1);
        if (it == adjacency_lists.end()) return false;
        return it->second.find(v2) != it->second.end();
    }
    
    // Get vertex degree
    int getDegree(int vertex) const {
        auto it = adjacency_lists.find(vertex);
        return (it != adjacency_lists.end()) ? it->second.size() : 0;
    }
    
    // Get number of vertices
    int vertexCount() const {
        return adjacency_lists.size();
    }
    
    // Get number of edges
    int edgeCount() const {
        return edge_set.size();
    }
    
    // Find connected components
    std::vector<std::unordered_set<int>> findConnectedComponents() const {
        std::vector<std::unordered_set<int>> components;
        std::unordered_set<int> visited;
        
        for (const auto& kv : adjacency_lists) {
            int vertex = kv.first;
            if (visited.find(vertex) != visited.end()) continue;
            
            // Start a new component
            std::unordered_set<int> component;
            std::queue<int> q;
            q.push(vertex);
            visited.insert(vertex);
            component.insert(vertex);
            
            while (!q.empty()) {
                int current = q.front();
                q.pop();
                
                for (int neighbor : getNeighbors(current)) {
                    if (visited.find(neighbor) == visited.end()) {
                        visited.insert(neighbor);
                        q.push(neighbor);
                        component.insert(neighbor);
                    }
                }
            }
            
            components.push_back(component);
        }
        
        return components;
    }
    
    // Map internal vertex ID to original ID
    int getOriginalId(int internal_id) const {
        auto it = reverse_ids.find(internal_id);
        return (it != reverse_ids.end()) ? it->second : -1;
    }
    
    // Map original ID to internal ID
    int getInternalId(int original_id) const {
        auto it = vertex_ids.find(original_id);
        return (it != vertex_ids.end()) ? it->second : -1;
    }
    
    // Get ID mappings
    const std::unordered_map<int, int>& getIdMappings() const {
        return reverse_ids;
    }
};

/**
 * Flow network edge for max-flow calculations
 */
struct FlowEdge {
    int to;
    int rev_idx;  // Index of reverse edge
    double capacity;
    double flow;
    
    FlowEdge(int t, int r, double cap) 
        : to(t), rev_idx(r), capacity(cap), flow(0.0) {}
};

/**
 * Maximum flow solver using Dinic's algorithm
 */
class MaxFlowSolver {
private:
    std::vector<std::vector<FlowEdge>> graph;
    std::vector<int> level;
    std::vector<int> next_edge;
    int vertices;
    int source;
    int sink;
    
    // Build level graph for BFS
    bool buildLevelGraph() {
        level.assign(vertices, -1);
        level[source] = 0;
        
        std::queue<int> q;
        q.push(source);
        
        while (!q.empty() && level[sink] == -1) {
            int v = q.front();
            q.pop();
            
            for (const FlowEdge& e : graph[v]) {
                if (level[e.to] == -1 && e.capacity > e.flow) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
        
        return level[sink] != -1;
    }
    
    // DFS to push flow
    double dfs(int v, double flow) {
        if (v == sink || flow == 0) return flow;
        
        for (int& i = next_edge[v]; i < graph[v].size(); i++) {
            FlowEdge& e = graph[v][i];
            
            if (level[e.to] == level[v] + 1 && e.capacity > e.flow) {
                double pushed = dfs(e.to, std::min(flow, e.capacity - e.flow));
                
                if (pushed > 0) {
                    e.flow += pushed;
                    graph[e.to][e.rev_idx].flow -= pushed;
                    return pushed;
                }
            }
        }
        
        return 0;
    }

public:
    MaxFlowSolver(int n) : vertices(n), graph(n), level(n), next_edge(n) {}
    
    // Add a directed edge with capacity
    void addEdge(int from, int to, double capacity) {
        graph[from].push_back(FlowEdge(to, graph[to].size(), capacity));
        graph[to].push_back(FlowEdge(from, graph[from].size() - 1, 0)); // Reverse edge
    }
    
    // Compute maximum flow using Dinic's algorithm
    double computeMaxFlow(int src, int snk) {
        source = src;
        sink = snk;
        double max_flow = 0;
        
        while (buildLevelGraph()) {
            next_edge.assign(vertices, 0);
            double flow;
            while ((flow = dfs(source, std::numeric_limits<double>::max())) > 0) {
                max_flow += flow;
            }
        }
        
        return max_flow;
    }
    
    // Find the min-cut partitions
    std::pair<std::unordered_set<int>, std::unordered_set<int>> findMinCut() {
        std::unordered_set<int> source_side, sink_side;
        std::vector<bool> visited(vertices, false);
        
        std::queue<int> q;
        q.push(source);
        visited[source] = true;
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            source_side.insert(v);
            
            for (const FlowEdge& e : graph[v]) {
                if (!visited[e.to] && e.capacity > e.flow) {
                    visited[e.to] = true;
                    q.push(e.to);
                }
            }
        }
        
        // All unvisited vertices belong to sink side
        for (int i = 0; i < vertices; i++) {
            if (!visited[i]) {
                sink_side.insert(i);
            }
        }
        
        return {source_side, sink_side};
    }
};

/**
 * CliqueFinder class to locate and count cliques of given size
 */
class CliqueFinder {
private:
    const Graph& graph;
    
    // Helper for enumerating cliques of size > 3
    void enumerateCliquesRecursive(std::vector<int>& current, 
                                   std::unordered_set<int>& candidates,
                                   int target_size,
                                   std::vector<std::vector<int>>& result) {
        // Base case: we've found a clique of the right size
        if (current.size() == target_size) {
            result.push_back(current);
            return;
        }
        
        // Copy candidates to avoid modifying during iteration
        std::vector<int> cand_vec(candidates.begin(), candidates.end());
        
        for (int v : cand_vec) {
            // Remove v from candidates
            candidates.erase(v);
            
            // Determine common neighbors (candidates intersect with v's neighbors)
            std::unordered_set<int> new_candidates;
            for (int c : candidates) {
                if (graph.areAdjacent(v, c)) {
                    new_candidates.insert(c);
                }
            }
            
            // If we can still potentially form a clique
            if (current.size() + new_candidates.size() + 1 >= target_size) {
                current.push_back(v);
                enumerateCliquesRecursive(current, new_candidates, target_size, result);
                current.pop_back();
            }
        }
    }

public:
    CliqueFinder(const Graph& g) : graph(g) {}
    
    // Find all cliques of given size containing a specific vertex
    std::vector<std::vector<int>> findCliquesWithVertex(int vertex, int size) {
        std::vector<std::vector<int>> cliques;
        
        // Handle special cases
        if (size == 1) {
            cliques.push_back({vertex});
            return cliques;
        }
        
        if (size == 2) {
            for (int neighbor : graph.getNeighbors(vertex)) {
                cliques.push_back({vertex, neighbor});
            }
            return cliques;
        }
        
        if (size == 3) {
            // Optimized triangle finding
            const auto& neighbors = graph.getNeighbors(vertex);
            std::vector<int> neighbor_vec(neighbors.begin(), neighbors.end());
            
            for (size_t i = 0; i < neighbor_vec.size(); i++) {
                for (size_t j = i + 1; j < neighbor_vec.size(); j++) {
                    if (graph.areAdjacent(neighbor_vec[i], neighbor_vec[j])) {
                        cliques.push_back({vertex, neighbor_vec[i], neighbor_vec[j]});
                    }
                }
            }
            return cliques;
        }
        
        // For larger cliques, use recursive approach
        std::vector<int> current = {vertex};
        std::unordered_set<int> candidates(graph.getNeighbors(vertex).begin(), 
                                         graph.getNeighbors(vertex).end());
        
        enumerateCliquesRecursive(current, candidates, size, cliques);
        return cliques;
    }
    
    // Count cliques of given size containing a specific vertex
    int countCliquesWithVertex(int vertex, int size) {
        // Optimized direct counting for common cases
        if (size == 1) return 1;
        if (size == 2) return graph.getDegree(vertex);
        
        if (size == 3) {
            // Fast triangle counting
            int count = 0;
            const auto& neighbors = graph.getNeighbors(vertex);
            std::vector<int> neighbor_vec(neighbors.begin(), neighbors.end());
            
            for (size_t i = 0; i < neighbor_vec.size(); i++) {
                for (size_t j = i + 1; j < neighbor_vec.size(); j++) {
                    if (graph.areAdjacent(neighbor_vec[i], neighbor_vec[j])) {
                        count++;
                    }
                }
            }
            return count;
        }
        
        // For other sizes, use full enumeration
        return findCliquesWithVertex(vertex, size).size();
    }
    
    // Count all cliques of given size in the graph
    int countTotalCliques(int size) {
        if (size == 1) return graph.vertexCount();
        if (size == 2) return graph.edgeCount();
        
        int total = 0;
        std::vector<int> vertices = graph.getVertices();
        
        for (int v : vertices) {
            if (size == 3) {
                // For triangles, count directly
                total += countCliquesWithVertex(v, 3);
            } else {
                // For larger cliques, ensure we count each clique only once
                std::vector<std::vector<int>> cliques = findCliquesWithVertex(v, size);
                for (const auto& clique : cliques) {
                    if (*std::min_element(clique.begin(), clique.end()) == v) {
                        total++;
                    }
                }
            }
        }
        
        // Each triangle is counted 3 times (once per vertex)
        if (size == 3) total /= 3;
        
        return total;
    }
};

/**
 * DenseSubgraphFinder class using flow-based techniques
 */
class DenseSubgraphFinder {
private:
    const Graph& graph;
    int clique_size;
    
    // Helper to build a flow network for min-cut calculations
    MaxFlowSolver buildFlowNetwork(const std::unordered_set<int>& vertices, double threshold) {
        int n = vertices.size();
        MaxFlowSolver solver(n + 2);  // +2 for source and sink
        int source = n;
        int sink = n + 1;
        
        // Create mapping for vertices
        std::unordered_map<int, int> vertex_mapping;
        int idx = 0;
        for (int v : vertices) {
            vertex_mapping[v] = idx++;
        }
        
        // Create subgraph to compute clique counts
        Graph subgraph = graph.createSubgraph(vertices);
        CliqueFinder finder(subgraph);
        
        // Connect source to vertices
        for (int v : vertices) {
            int flow_idx = vertex_mapping[v];
            // Map v to corresponding vertex in subgraph
            int subgraph_v = subgraph.getInternalId(graph.getOriginalId(v));
            if (subgraph_v != -1) {
                double clique_count = finder.countCliquesWithVertex(subgraph_v, clique_size);
                solver.addEdge(source, flow_idx, clique_count);
            }
        }
        
        // Connect vertices to sink
        for (int v : vertices) {
            int flow_idx = vertex_mapping[v];
            solver.addEdge(flow_idx, sink, threshold * clique_size);
        }
        
        // Connect adjacent vertices with infinite capacity
        for (int u : vertices) {
            for (int v : graph.getNeighbors(u)) {
                if (vertices.find(v) != vertices.end() && u < v) {
                    solver.addEdge(vertex_mapping[u], vertex_mapping[v], std::numeric_limits<double>::max());
                    solver.addEdge(vertex_mapping[v], vertex_mapping[u], std::numeric_limits<double>::max());
                }
            }
        }
        
        return solver;
    }
    
    // Calculate density of a subgraph
    double calculateDensity(const std::unordered_set<int>& vertices) {
        if (vertices.empty()) return 0.0;
        
        Graph subgraph = graph.createSubgraph(vertices);
        CliqueFinder finder(subgraph);
        int clique_count = finder.countTotalCliques(clique_size);
        
        return static_cast<double>(clique_count) / vertices.size();
    }
    
    // Find core of graph where each vertex belongs to at least threshold cliques
    std::unordered_set<int> findCore(double threshold) {
        std::vector<int> all_vertices = graph.getVertices();
        std::unordered_set<int> remaining(all_vertices.begin(), all_vertices.end());
        bool changed = true;
        
        CliqueFinder finder(graph);
        
        while (changed) {
            changed = false;
            std::vector<int> to_remove;
            
            for (int v : remaining) {
                if (finder.countCliquesWithVertex(v, clique_size) < threshold) {
                    to_remove.push_back(v);
                    changed = true;
                }
            }
            
            for (int v : to_remove) {
                remaining.erase(v);
            }
            
            // If core gets too small, break
            if (remaining.size() < clique_size) break;
            
            // If changes made, update finder with subgraph
            if (changed && !remaining.empty()) {
                Graph subgraph = graph.createSubgraph(remaining);
                CliqueFinder finder(subgraph);
            }
        }
        
        return remaining;
    }

public:
    DenseSubgraphFinder(const Graph& g, int size) : graph(g), clique_size(size) {}
    
    // Find densest subgraph using flow-based algorithm
    std::unordered_set<int> findDensestSubgraph() {
        Timer timer("Densest subgraph algorithm");
        std::cout << "Finding densest subgraph with " << clique_size << "-clique density..." << std::endl;
        
        // Find k-core decomposition first
        std::vector<int> vertices = graph.getVertices();
        std::unordered_map<int, int> core_numbers;
        int max_core = 0;
        
        // Compute k-cores
        for (int k = 1; ; k++) {
            std::unordered_set<int> candidates(vertices.begin(), vertices.end());
            std::vector<int> degree_list(graph.vertexCount());
            
            // Setup initial degrees
            for (int v : candidates) {
                int deg = 0;
                for (int u : graph.getNeighbors(v)) {
                    if (candidates.find(u) != candidates.end()) {
                        deg++;
                    }
                }
                degree_list[v] = deg;
            }
            
            bool removed_any = false;
            do {
                removed_any = false;
                std::vector<int> to_remove;
                
                for (int v : candidates) {
                    if (degree_list[v] < k) {
                        to_remove.push_back(v);
                    }
                }
                
                for (int v : to_remove) {
                    candidates.erase(v);
                    removed_any = true;
                    
                    // Update degrees of neighbors
                    for (int u : graph.getNeighbors(v)) {
                        if (candidates.find(u) != candidates.end()) {
                            degree_list[u]--;
                        }
                    }
                }
            } while (removed_any);
            
            if (candidates.empty()) {
                break;
            }
            
            for (int v : candidates) {
                core_numbers[v] = k;
            }
            
            max_core = k;
        }
        
        std::cout << "Core decomposition completed. Maximum core: " << max_core << std::endl;
        
        // Track best solution
        double best_density = 0.0;
        std::unordered_set<int> best_subgraph;
        
        // Get max-core vertices
        std::unordered_set<int> max_core_vertices;
        for (const auto& entry : core_numbers) {
            if (entry.second == max_core) {
                max_core_vertices.insert(entry.first);
            }
        }
        
        // Find connected components in max-core
        Graph max_core_graph = graph.createSubgraph(max_core_vertices);
        auto components = max_core_graph.findConnectedComponents();
        
        std::cout << "Found " << components.size() << " component(s) in the " << max_core << "-core" << std::endl;
        
        // Process each component
        for (size_t i = 0; i < components.size(); i++) {
            if (components[i].empty()) continue;
            
            // Map component vertices back to original graph
            std::unordered_set<int> component;
            for (int v : components[i]) {
                int orig_v = max_core_graph.getOriginalId(v);
                int internal_v = graph.getInternalId(orig_v);
                component.insert(internal_v);
            }
            
            std::cout << "Processing component " << i+1 << " with " << component.size() << " vertices" << std::endl;
            
            // Set initial bounds
            double lower_bound = max_core;
            double upper_bound = max_core * 10;  // Generous upper bound
            
            // Try component as initial solution
            double density = calculateDensity(component);
            if (density > best_density) {
                best_density = density;
                best_subgraph = component;
                std::cout << "  Initial component density: " << best_density << std::endl;
            }
            
            // Binary search for optimal threshold
            std::unordered_set<int> current_set = component;
            int iterations = 0;
            const int MAX_ITERATIONS = 25;
            
            while (upper_bound - lower_bound > 0.5 && current_set.size() > clique_size && iterations < MAX_ITERATIONS) {
                iterations++;
                double threshold = (lower_bound + upper_bound) / 2;
                
                auto start_time = std::chrono::high_resolution_clock::now();
                MaxFlowSolver flow = buildFlowNetwork(current_set, threshold);
                flow.computeMaxFlow(current_set.size(), current_set.size() + 1);
                auto cut = flow.findMinCut();
                auto source_side = cut.first;
                
                auto iter_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - start_time);
                
                // If source side contains only source vertex, no valid cut
                if (source_side.size() <= 1) {
                    upper_bound = threshold;
                    std::cout << "  Iteration " << iterations << ": No valid cut found, upper = " 
                              << upper_bound << std::endl;
                } else {
                    // Extract vertices (excluding source)
                    std::unordered_set<int> candidate_set;
                    std::unordered_map<int, int> reverse_mapping;
                    int idx = 0;
                    for (int v : current_set) {
                        reverse_mapping[idx++] = v;
                    }
                    
                    for (int idx : source_side) {
                        if (idx != current_set.size()) {  // Skip source
                            candidate_set.insert(reverse_mapping[idx]);
                        }
                    }
                    
                    lower_bound = threshold;
                    
                    // Update working set for next iteration
                    if (!candidate_set.empty()) {
                        current_set = candidate_set;
                        
                        // Calculate density
                        density = calculateDensity(candidate_set);
                        std::cout << "  Iteration " << iterations << ": density = " << density 
                                  << ", vertices = " << candidate_set.size()
                                  << ", time = " << iter_duration.count() / 1000.0 << "s" << std::endl;
                        
                        if (density > best_density) {
                            best_density = density;
                            best_subgraph = candidate_set;
                            std::cout << "  New best solution with density " << best_density << std::endl;
                        }
                    }
                }
            }
        }
        
        std::cout << "Final solution: " << best_subgraph.size() << " vertices with density " 
                  << best_density << std::endl;
        
        return best_subgraph;
    }
};

/**
 * GraphLoader class for reading graph data from files
 */
class GraphLoader {
public:
    // Load a graph from a file
    static Graph loadFromFile(const std::string& filename) {
        Timer timer("Graph loading");
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        
        Graph graph;
        int num_vertices, num_edges;
        file >> num_vertices >> num_edges;
        
        std::string line;
        getline(file, line);  // Skip header line
        
        int count = 0;
        while (getline(file, line)) {
            std::istringstream iss(line);
            int src, tgt;
            if (iss >> src >> tgt) {
                graph.addEdge(src, tgt);
                count++;
            }
        }
        
        std::cout << "Loaded graph with " << graph.vertexCount() << " vertices and " 
                  << graph.edgeCount() << " edges" << std::endl;
        
        return graph;
    }
};

/**
 * Result handler for processing and saving results
 */
class ResultHandler {
public:
    static void printAndSaveResults(const Graph& graph, 
                                  const std::unordered_set<int>& solution,
                                  int clique_size,
                                  double density) {
        // Map solution back to original IDs
        std::vector<int> original_ids;
        for (int v : solution) {
            original_ids.push_back(graph.getOriginalId(v));
        }
        std::sort(original_ids.begin(), original_ids.end());
        
        // Create subgraph for statistics
        // Create subgraph for statistics
        Graph solution_graph = graph.createSubgraph(solution);
        CliqueFinder finder(solution_graph);
        int clique_count = finder.countTotalCliques(clique_size);
        
        std::stringstream output;
        output << "Densest Subgraph Results\n";
        output << "------------------------\n";
        output << "Nodes (original IDs): [";
        for (size_t i = 0; i < original_ids.size(); i++) {
            output << original_ids[i];
            if (i < original_ids.size() - 1) output << ", ";
        }
        output << "]\n";
        output << clique_size << "-clique Density: " << density << "\n";
        output << "Number of vertices: " << solution.size() << "\n";
        output << "Number of " << clique_size << "-cliques: " << clique_count << "\n";
        
        // Add specialized info for edges and triangles
        if (clique_size == 2) {
            output << "Number of edges: " << solution_graph.edgeCount() << "\n";
        }
        
        if (clique_size == 3) {
            output << "Number of triangles: " << clique_count << "\n";
        }
        
        std::cout << "\nResults:" << std::endl;
        std::cout << output.str();
        
        // Save results to file
        std::string output_file = "dense_subgraph_results.txt";
        std::ofstream outfile(output_file);
        if (outfile) {
            outfile << output.str();
            outfile.close();
            std::cout << "Results saved to " << output_file << std::endl;
        } else {
            std::cout << "Error: Could not write to output file." << std::endl;
        }
    }
};

/**
 * Main application class
 */
class DenseSubgraphApp {
private:
    Graph graph;
    int clique_size;
    
    void printBanner() {
        std::cout << "╔════════════════════════════════════════════╗" << std::endl;
        std::cout << "║        DENSE SUBGRAPH FINDER v2.0          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════╝" << std::endl;
    }
    
    void validateCliqueSize() {
        if (clique_size < 1) {
            throw std::runtime_error("Clique size must be at least 1");
        }
        
        // Check if graph contains cliques of specified size
        CliqueFinder finder(graph);
        int total_cliques = finder.countTotalCliques(clique_size);
        
        if (total_cliques == 0) {
            throw std::runtime_error("Graph does not contain any cliques of size " + 
                                     std::to_string(clique_size));
        }
        
        std::cout << "Graph contains " << total_cliques << " cliques of size " 
                  << clique_size << std::endl;
    }

public:
    DenseSubgraphApp() : clique_size(3) {}
    
    void run() {
        // printBanner();
        
        // Get input filename
        std::string filename;
        std::cout << "Enter graph file path: ";
        std::cin >> filename;
        
        // Load graph data
        try {
            graph = GraphLoader::loadFromFile(filename);
        } catch (const std::exception& e) {
            std::cout << "Error loading graph: " << e.what() << std::endl;
            return;
        }
        
        // Get clique size
        std::cout << "Enter clique size (h): ";
        std::cin >> clique_size;
        
        try {
            validateCliqueSize();
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            return;
        }
        
        // Find densest subgraph
        DenseSubgraphFinder finder(graph, clique_size);
        std::unordered_set<int> solution = finder.findDensestSubgraph();
        
        // Calculate final density
        Graph solution_graph = graph.createSubgraph(solution);
        CliqueFinder clique_finder(solution_graph);
        int clique_count = clique_finder.countTotalCliques(clique_size);
        double density = static_cast<double>(clique_count) / solution.size();
        
        // Display and save results
        ResultHandler::printAndSaveResults(graph, solution, clique_size, density);
    }
};

/**
 * Entry point
 */
int main() {
    try {
        DenseSubgraphApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}