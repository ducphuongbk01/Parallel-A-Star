#ifndef __FINE_GRAIN_QUEUE_HPP__
#define __FINE_GRAIN_QUEUE_HPP__

#include <iostream>
#include <vector>
#include "base.hpp"

struct NodeFineGrain {  
    long id;
    float h_score;
    float g_score;
    float val;
    std::vector<long> path;
    NodeFineGrain *next;
    OpenMPLock lock;

    NodeFineGrain (float val, NodeFineGrain* ptr) {
        this->id = -1;
        this->h_score = -1;
        this->g_score = -1;
        this->val = val;
        next = ptr;
    };

    NodeFineGrain (long id, float g_score, float h_score, std::vector<long>& path) {
        this->id = id;
        this->g_score = g_score;
        this->h_score = h_score;
        this->val = h_score + g_score;
        this->path = {path.begin(), path.end()};
        next = nullptr;
    };

    NodeFineGrain (long id, float g_score, float h_score) {
        this->id = id;
        this->g_score = g_score;
        this->h_score = h_score;
        this->val = h_score + g_score;
        this->path.push_back(id);
        next = nullptr;
    };
    
    void acquire() {
        lock.acquire();
    };

    void release() {
        lock.release();
    };
};

NodeFineGrain* find_node_hoh(NodeFineGrain *start, float val) {
    NodeFineGrain *curr = start;
    curr->acquire();
    NodeFineGrain *next = curr->next;
    next->acquire();
    while(next && (next->val < val)) {
        curr->release();
        curr = next;
        next = curr->next;
        next->acquire();
    }
    return curr;
};

class FineGrainQueue {
    public:
        NodeFineGrain *tail = new NodeFineGrain(9e20, nullptr);
        NodeFineGrain *head = new NodeFineGrain(-9e20, tail);

        FineGrainQueue() = default;

        ~FineGrainQueue() = default;

        void enqueue(NodeFineGrain* node) {
            NodeFineGrain *res = find_node_hoh(head, node->val);
            node->acquire();
            node->next = res->next;
            res->next = node;
            res->release();
            node->release();
            node->next->release();
        };

        NodeFineGrain* dequeue() {
            NodeFineGrain *hptr = head;
            hptr->acquire();
            NodeFineGrain *nptr = hptr->next;
            nptr->acquire();
            // Check if the next node is tail
            if (nptr->id == -1) {
                hptr->release();
                nptr->release();
                return nullptr;
            }
            hptr->next = nptr->next;
            nptr->next = nullptr;
            hptr->release();
            nptr->release();
            return nptr;
        };
};

#endif // __FINE_GRAIN_QUEUE_HPP__
