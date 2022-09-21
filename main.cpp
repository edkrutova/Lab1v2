#include <iostream>
#include <list>
#include <set>
#include "structs.h"
#include "utils.h"
#include <chrono>
#include <valarray>

class BFS //breadth-first-search
{
public:
    Result find_path(Node start, Node goal, Map grid)
    {
        auto time_now = std::chrono::high_resolution_clock::now();
        Result result;
        int steps = 0;
        start.g = 0;
        std::list<Node> OPEN;
        OPEN.push_back(start);
        std::set<Node> CLOSED;
        CLOSED.insert(start);
        bool pathfound = false;
        while(!OPEN.empty() && !pathfound)
        {
           Node current = OPEN.front();
           OPEN.pop_front();
           steps++;
           auto neighbors = grid.get_neighbors(current);
           for(auto n:neighbors) {
               if (CLOSED.find(n) == CLOSED.end())
               {
                   n.g = current.g + 1;
                   n.parent = &(*CLOSED.find(current));
                   OPEN.push_back(n);
                   CLOSED.insert(n);
                   if(n == goal) {
                       result.path = reconstruct_path(n);
                       result.cost = n.g;
                       pathfound = true;
                       break;
                    }
                }
            }
        }
        result.steps = steps;
        result.nodes_created = CLOSED.size();
        result.runtime = (std::chrono::high_resolution_clock::now() - time_now).count()/1e+9;
        return result;
    }
    std::list<Node> reconstruct_path(Node n)
    {
        std::list<Node> path;
        while(n.parent != nullptr)
        {
            path.push_front(n);
            n = *n.parent;
        }
        path.push_front(n);
        return path;
    }
};

class AStar
{
public:
    Result find_path(Node start, Node goal, Map grid, std::string metrictype="Octile", int connections=8, double hweight=1)
    {
        OpenList OPEN;
        ClosedList CLOSED;
    auto t = std::chrono::high_resolution_clock::now();
    Result result;
    start.f=0;
    start.g=0;
    OPEN.addNode(start);

    while (true) {
        auto current = OPEN.getMin();
        CLOSED.addClose(current);
        OPEN.popMin();
        auto tol = grid.getValidMoves(current);
        for (auto b: tol) {
            if (!CLOSED.inClose(b.i, b.j)) {
                b.f = grid.getCost(current, b) + current.f+getHValue(current,b,input.map.diagonal_moves_allowed);
                b.g = current.g+getHValue(current,b, grid.diagonal_moves_allowed);
                b.parent = CLOSED.getPointer(current.x, current.y);
                OPEN.addNode(b);
            }
        }
        if (current.x == input.goal.x and current.y == input.goal.y) {
            CLOSED.addClose(current);
            result.pathfound = true;
            result.path = reconstructPath(current);
            result.cost = current.g;
            break;
        }
    }

    result.createdNodes = CLOSED.getSize() + OPEN.getSize();
    result.steps = CLOSED.getSize();
    result.runtime = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t).count();
    return result;
}
    double count_h_value(Node current, Node goal, std::string metrictype="Octile")
    {
        //TODO - add support of all three metrics
        return((double)sqrt((current.i-goal.i)*(current.i - goal.i) + (current.j - goal.j) * (current.j - goal.j)));
    }
    std::list<Node> reconstruct_path(Node current)
    {
        std::list<Node> path;
        while(current.parent != nullptr)
        {
            path.push_front(current);
            current = *current.parent;
        }
        path.push_front(current);
        return path;
    }
};

int main(int argc, char* argv[]) //argc - argumnet counter, argv - argument values
{
    for(int i=0; i<argc; i++)
        std::cout<<argv[i]<<"\n";
    if(argc<2)
    {
        std::cout << "Name of the input XML file is not specified."<<std::endl;
        return 1;
    }
    Loader loader;
    loader.load_instance(argv[1]);

    Result result;
    if(loader.algorithm == "BFS")
    {
        BFS bfs;
        result = bfs.find_path(loader.start, loader.goal, loader.grid);
    }
    else
    {
        if(loader.algorithm == "Dijkstra")
            loader.hweight = 0;
        AStar astar;
        result = astar.find_path(loader.start, loader.goal, loader.grid, loader.metrictype, loader.connections, loader.hweight);
    }
    loader.grid.print(result.path);
    std::cout<<"Cost: "<<result.cost<<"\nRuntime: "<<result.runtime
    <<"\nSteps: "<<result.steps<<"\nNodes created: "<<result.nodes_created<<std::endl;
    return 0;
}