#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include "dependency.hpp"

struct Vertex {
    long id;
    float x;
    float y;
    int street_count;

    Vertex(long id, float x, float y, int street_count) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->street_count = street_count;
    }
};

struct Edge {
    Vertex* source;
    Vertex* target;
    float length;

    Edge(Vertex* source, Vertex* target, float length) {
        this->source = source;
        this->target = target;
        this->length = length;
    }
};

class Graph {
    private:
        std::map<long, Vertex*> vertices;
        std::vector<Edge*> edges;
        std::map<long, std::vector<Edge*>> graph; 

        const std::string VERTEX = "data/nodes.csv";
        const std::string EDGE = "data/edges.csv";

        void __load_graph() {
            {
                std::ifstream file(VERTEX);
                std::string line;

                while (std::getline(file, line)) {
                    std::stringstream ss(line);
                    std::string cell;
                    std::getline(ss, cell, ',');
                    long id = std::stol(cell);
                    std::getline(ss, cell, ',');
                    float x = std::stof(cell);
                    std::getline(ss, cell, ',');
                    float y = std::stof(cell);
                    std::getline(ss, cell, ',');
                    int street_count = std::stoi(cell);

                    this->vertices[id] = new Vertex(id, x, y, street_count);
                }
            }

            {
                std::ifstream file(EDGE);
                std::string line;
                
                while (std::getline(file, line)) {
                    std::stringstream ss(line);
                    std::string cell;

                    std::getline(ss, cell, ',');
                    long source_id = std::stol(cell);
                    Vertex* source = vertices[source_id];

                    std::getline(ss, cell, ',');
                    long target_id = std::stol(cell);
                    Vertex* target = vertices[target_id];

                    std::getline(ss, cell, ',');
                    float length = std::stof(cell);
                    
                    this->edges.push_back(new Edge(source, target, length));
                }
            }
        }

    public:
        Graph() {   
            this->__load_graph();         
            for (Edge* edge : this->edges) {
                this->graph[edge->source->id].push_back(edge);
                this->graph[edge->target->id].push_back(edge);
            }
        }

        ~Graph() {
            for (auto it=this->vertices.begin(); it!=this->vertices.end(); it++) 
                delete it->second;
            
            for (Edge* edge : this->edges) delete edge;
        }

        std::vector<std::pair<Vertex*, float>> get_neighbor(long id) {
            std::vector<std::pair<Vertex*, float>> neighbors;
            for (Edge* edge : this->graph[id]) {
                if (edge->source->id == id) {
                    neighbors.push_back(
                        std::make_pair(edge->target, edge->length)
                    );
                }
            }

            return neighbors;
        }

        Vertex* find_vertex(long id) {
            return this->vertices[id];
        }
};

#endif