#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <cmath>
#include <cstdio> 

#include "set.hpp"
#include "graph.hpp"

#include "queue.hpp"

// #include "coarse_grain_queue.hpp"

// #include "fine_grain_queue.hpp"
// typedef NodeFineGrain Node;

#define NUM_THREADS 4

using namespace std;
using namespace std::chrono;


Set close_set;
// SequentialQueue queue;
// ScheduleLockQueue queue;
SpinLockQueue queue;
// FineGrainQueue queue;
// CoarseGrainQueue queue;
// LockFreeQueue queue;

bool complete = false;
vector<long> path;
Graph* graph = nullptr;
Vertex* src = nullptr;
Vertex* des = nullptr;

// Begin: Test queue (enqueue - dequeue)

// void test_queue(int id, int length) {
//     for (int i=0; i<length; i++) {
//         int num = rand() % length;
//         if (num%10 == 0) {
//             queue.dequeue();
//         }
//         else {
//             queue.enqueue(
//                 new Node(num, 0, num, path)
//             );
//         }
//     }
// }

// int main(int argc, char** argv) {
//     const int LEN = 5000;
//     srand(time(NULL));
//     vector<thread> threads;
//     for (int i = 0; i < NUM_THREADS; ++i) {
//         threads.emplace_back(test_queue, i, LEN);
//     }
//     for (thread& t : threads) t.join();
        
//     queue.dump();

//     return 0;
// }

// End: Test queue

float distance(Vertex* v1, Vertex* v2) {
    return sqrt(
        (v1->x - v2->x) * (v1->x - v2->x) +
        (v1->y - v2->y) * (v1->y - v2->y)
    );
}

void search_path(int id) {
    while (!complete) {
        Node* curr_node = queue.dequeue();
        if (curr_node == nullptr) continue;

        if (curr_node->id == des->id) {
            cout << id << "\n";
            complete = true;
            path = {curr_node->path.begin(), curr_node->path.end()};
            break;
        }

        if (close_set.contain(curr_node->id)) {
            continue;
        }

        close_set.insert(curr_node->id);

        for (pair<Vertex*, float> near : graph->get_neighbor(curr_node->id)) {
            Vertex* neighbor = near.first;
            float length = near.second;
            
            if (close_set.contain(neighbor->id)) continue;
            else {
                float g_score_neighbor = curr_node->g_score + length;
                float h_score_neighbor = distance(neighbor, des);
                vector<long> node_path = {curr_node->path.begin(), curr_node->path.end()};
                node_path.push_back(neighbor->id);

                queue.enqueue(
                    new Node(neighbor->id, g_score_neighbor, h_score_neighbor, node_path)
                );
            }
        }
        
        delete curr_node;
    }
}

void astar() {
    
    if (src == des) {
        path.push_back(src->id);
    }
    else {
        float g_score = 0;
        float h_score = distance(src, des);
        queue.enqueue(new Node(src->id, g_score, h_score));

        vector<thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(search_path, i);
        }
    
        for (thread& t : threads) {
            t.join();
        }
    }
}

// Main entry 
int main(int argc, char** argv) {

    const long SRC = stol(argv[1]);
    const long DES = stol(argv[2]);

    graph = new Graph();
    src = graph->find_vertex(SRC);
    des = graph->find_vertex(DES);
    
    auto start = system_clock::now();
    astar();
    auto end = system_clock::now();

    for (long node : path) cout << node << "\n";
    cout << duration_cast<milliseconds>(end - start).count() << "\n";
    delete graph;

    return 0;
}

// int main() {

//     const long SRC = 3700940837;
//     const long DES = 11346641691;

//     graph = new Graph();
//     src = graph->find_vertex(SRC);
//     des = graph->find_vertex(DES);
    
//     auto start = system_clock::now();
//     astar();
//     auto end = system_clock::now();

//     for (long node : path) cout << node << "\n";
//     cout << duration_cast<milliseconds>(end - start).count() << "\n";
//     delete graph;

//     return 0;
// }
