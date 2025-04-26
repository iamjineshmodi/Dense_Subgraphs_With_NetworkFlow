#include<bits/stdc++.h>
using namespace std;
using namespace std::chrono;

struct Edge
{
    int to, rev;
    double cap, flow;
};

class Dinic
{
    public:
    Dinic(int n) : n(n)
    {
        adj.resize(n);
        dist.resize(n);
        ptr.resize(n);
    }

    void addEdge(int u, int v, double capacity)
    {
        adj[u].push_back({v, static_cast<int>(adj[v].size()), capacity, 0});
        adj[v].push_back({u, static_cast<int>(adj[u].size() - 1), 0, 0});
    }

    void updateEdge(int u, int v, double capacity)
    {
        for(auto& edge : adj[u])
        {
            if(edge.to == v)
            {
                edge.cap = capacity;
                return;
            }
        }
    }

    void resetFlow()
    {
        for(int u=0; u<n; u++)
        {
            for(auto& edge : adj[u])
            {
                edge.flow = 0;
            }
        }
    }

    double maxFlow(int source, int sink)
    {
        double flow = 0.0;
        while(BFS(source, sink))
        {
            fill(ptr.begin(), ptr.end(), 0);
            while(double pushed = DFS(source, sink, INF))
            {
                flow += pushed;
            }
        }
        return flow;
    }

    vector<bool> getReachable(int source)
    {
        vector<bool> result(n, false);
        queue<int> q;
        q.push(source);
        result[source] = true;
        while(!q.empty())
        {
            int u = q.front();
            q.pop();
            for(auto& edge : adj[u])
            {
                if(!result[edge.to] && (edge.cap - edge.flow) > 0)
                {
                    result[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }
        return result;
    }

    private:
    int n;
    vector<vector<Edge>> adj;
    vector<int> dist, ptr;
    const double INF = numeric_limits<double>::max() / 2.0;

    bool BFS(int source, int sink)
    {
        fill(dist.begin(), dist.end(), -1);
        dist[source] = 0;
        queue<int> q;
        q.push(source);
        while(!q.empty())
        {
            int u = q.front();
            q.pop();
            for(auto& edge : adj[u])
            {
                if(edge.cap > edge.flow && dist[edge.to] == -1)
                {
                    dist[edge.to] = dist[u] + 1;
                    q.push(edge.to);
                }
            }
        }
        return dist[sink] != -1;
    }

    double DFS(int u, int sink, double flow)
    {
        if(u == sink)
        {
            return flow;
        }
        for(int& i = ptr[u]; i < adj[u].size(); i++)
        {
            Edge& edge = adj[u][i];
            if(edge.cap > edge.flow && dist[edge.to] == dist[u] + 1)
            {
                double pushed = DFS(edge.to, sink, min(flow, edge.cap - edge.flow));
                if(pushed > 0)
                {
                    edge.flow += pushed;
                    adj[edge.to][edge.rev].flow -= pushed;
                    return pushed;
                }
            }
        }
        return 0.0;
    }
};

vector<unordered_set<int>> EnumerateCliques(const vector<vector<int>>& adj, int h)
{
    vector<unordered_set<int>> result;

    function<void(vector<int>&, int, unordered_set<int>&)> BronKerbosch = [&](vector<int>& R, int v, unordered_set<int>& P)
    {
        if(R.size() == h-1)
        {
            result.push_back(unordered_set<int>(R.begin(), R.end()));
            return;
        }
        vector<int> P_copy(P.begin(), P.end());
        for(int u : P_copy)
        {
            if(u <= v)
            {
                continue;
            }
            R.push_back(u);
            P.erase(u);
            unordered_set<int> Pv;
            for(int w : adj[u])
            {
                if(P.find(w) != P.end())
                {
                    Pv.insert(w);
                }
            }
            BronKerbosch(R, u, Pv);
            R.pop_back();
            P.insert(u);
        }
    };

    for(int u=0; u<adj.size(); u++)
    {
        vector<int> R = {u};
        unordered_set<int> P;
        for(int v : adj[u])
        {
            if(v > u)
            {
                P.insert(v);
            }
        }
        BronKerbosch(R, u, P);
    }
    return result;
}

vector<int> ComputeCliqueDegree(int n, const vector<unordered_set<int>>& Lambda, const vector<vector<int>>& adj)
{
    vector<int> result(n, 0);
    for(const auto& clique : Lambda)
    {
        unordered_set<int> common_neighbours;
        bool flag = true;
        for(int v : clique)
        {
            if(flag)
            {
                for(int neighbour : adj[v])
                {
                    if(clique.find(neighbour) == clique.end())
                    {
                        common_neighbours.insert(neighbour);
                    }
                }
                flag = false;
            }
            else
            {
                unordered_set<int> new_common;
                for(int neighbour : adj[v])
                {
                    if(clique.find(neighbour) == clique.end() && common_neighbours.find(neighbour) != common_neighbours.end())
                    {
                        new_common.insert(neighbour);
                    }
                }
                common_neighbours = new_common;
            }
        }
        for(int v : common_neighbours)
        {
            result[v]++;
        }
    }
    return result;
}

pair<double, vector<int>> EXACT(int n, int h, const vector<unordered_set<int>>& Lambda, const vector<vector<int>>& adj, int maxCliqueDegree, vector<int>& CliqueDegree)
{
    vector<int> D;
    double l = 0.0, u = static_cast<double>(maxCliqueDegree);
    int iterations = 0;
    double epsilon = 1.0 / (n * (n - 1));
    while(u - l > epsilon)
    {
        iterations++;
        double alpha = l + (u - l) / 2.0;
        cout << "Iteration " << iterations << ": l = " << l << ", u = " << u << ", alpha = " << alpha << endl;
        int nodes = n + 2 + Lambda.size();
        int source = 0, sink = 1;
        int vertexoffset = 2;
        int cliqueoffset = vertexoffset + n;
        Dinic dinic(nodes);
        for(int v=0; v<n; v++)
        {
            if(CliqueDegree[v])
            {
                dinic.addEdge(source, vertexoffset + v, CliqueDegree[v]);
            }
        }
        for(int v=0; v<n; v++)
        {
            dinic.addEdge(vertexoffset + v, sink, h * alpha);
        }
        for(int i=0; i < Lambda.size(); i++)
        {
            const auto& clique = Lambda[i];
            int cliqueID = cliqueoffset + i;
            for(int v : clique)
            {
                dinic.addEdge(cliqueID, vertexoffset + v, numeric_limits<double>::infinity());
            }
            unordered_set<int> common_neighbours;
            bool flag = true;
            for(int u : clique)
            {
                if(flag)
                {
                    for(int v : adj[u])
                    {
                        if(clique.find(v) == clique.end() && common_neighbours.find(v) == common_neighbours.end())
                        {
                            common_neighbours.insert(v);
                        }
                    }
                    flag = false;
                }
                else
                {
                    unordered_set<int> new_common;
                    for(int v : adj[u])
                    {
                        if(clique.find(v) == clique.end() && common_neighbours.find(v) != common_neighbours.end())
                        {
                            new_common.insert(v);
                        }
                    }
                    common_neighbours = new_common;
                }
            }
            for(int v : common_neighbours)
            {
                dinic.addEdge(vertexoffset + v, cliqueID, 1.0);
            }
        }
        double flow = dinic.maxFlow(source, sink);
        vector<bool> mincut = dinic.getReachable(source);
        bool flag = true;
        for(int i=1; i<nodes; i++)
        {
            if(mincut[i])
            {
                flag = false;
                break;
            }
        }
        if(flag)
        {
            u = alpha;
        }
        else
        {
            l = alpha;
            D.clear();
            for(int v=0; v<n; v++)
            {
                if(mincut[vertexoffset + v])
                {
                    D.push_back(v);
                }
            }
        }
        if(abs(u - l) < 1e-12)
        {
            break;
        }
    }
    return {l, D};
}

int main()
{
    string filename;
    cout << "Enter the filename: ";
    cin >> filename;
    int h;
    cout << "Enter size of clique h: ";
    cin >> h;
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    ifstream infile(filename + ".txt");
    if(!infile)
    {
        cout << "Error opening file" << endl;
        return 0;
    }
    int n, e;
    infile >> n >> e;
    vector<pair<int, int>> edges;
    set<int> vertices;
    int self_loops = 0, duplicate_edges = 0;
    for(int i=0; i<e; i++)
    {
        int u, v;
        infile >> u >> v;
        if(u == v)
        {
            self_loops++;
            continue;
        }
        if(count(edges.begin(), edges.end(), make_pair(u, v)) || count(edges.begin(), edges.end(), make_pair(v, u)))
        {
            duplicate_edges++;
            continue;
        }
        edges.push_back({u, v});
        vertices.insert(u);
        vertices.insert(v);
    }
    infile.close();
    cout << "Self loops ignored: " << self_loops << endl;
    cout << "Duplicate edges ignored: " << duplicate_edges << endl;
    cout << "Number of edges: " << edges.size() << endl;

    unordered_map<int, int> vertextoID, IDtovertex;
    int id = 0;
    for(int v : vertices)
    {
        vertextoID[v] = id;
        IDtovertex[id] = v;
        id++;
    }
    cout << "Mapped vertices" << endl;

    vector<vector<int>> adj(n);
    vector<int> degree(n, 0);
    for(const auto& edge : edges)
    {
        int uidx = vertextoID[edge.first];
        int vidx = vertextoID[edge.second];
        adj[uidx].push_back(vidx);
        adj[vidx].push_back(uidx);
        degree[uidx]++;
        degree[vidx]++;
    }
    cout << "Adjacency and degree list created" << endl;

    vector<unordered_set<int>> Lambda = EnumerateCliques(adj, h);
    cout << "Lambda set created" << endl;
    cout << "Number of " << h-1 << "-cliques: " << Lambda.size() << endl;

    vector<int> CliqueDegree = ComputeCliqueDegree(n, Lambda, adj);
    cout << "Clique degree computed" << endl;

    int maxCliqueDegree = 0, positiveCliqueDegree = 0;
    for(int degree : CliqueDegree)
    {
        if(degree > maxCliqueDegree)
        {
            maxCliqueDegree = degree;
        }
        if(degree > 0)
        {
            positiveCliqueDegree++;
        }
    }
    cout << "Maximum clique degree: " << maxCliqueDegree << endl;
    cout << "Number of vertices with positive clique degree: " << positiveCliqueDegree << endl;

    auto start = high_resolution_clock::now();
    auto exact = EXACT(n, h, Lambda, adj, maxCliqueDegree, CliqueDegree);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    auto alpha = exact.first;
    auto D = exact.second;
    vector<int> originalD;
    for(int v : D)
    {
        originalD.push_back(IDtovertex[v]);
    }
    sort(originalD.begin(), originalD.end());

    cout << "---------------------------------------------" << endl;
    cout << "Dataset: " << filename << endl;
    cout << "Nodes: " << n << " Edges: " << e << endl;
    cout << "H: " << h << endl;
    cout << "Alpha: " << alpha << endl;
    cout << "Nodes in CDS: " << originalD.size() << endl << "CDS: ";
    for(int v : originalD)
    {
        cout << v << " ";
    }
    cout << endl;
    cout << "Time taken: " << duration.count() << " ms" << endl;
    cout << "---------------------------------------------" << endl;

    return 0;
}