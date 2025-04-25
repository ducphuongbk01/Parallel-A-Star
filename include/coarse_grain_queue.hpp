#ifndef __COARSE_GRAIN_QUEUE_HPP__
#define __COARSE_GRAIN_QUEUE_HPP__

#include <iostream>
#include <vector>
#include "base.hpp"

struct Node {
    long id;
    float h_score;
    float g_score;
    float val;
    std::vector<long> path;
    Node *next;

    // For declare head and tail nodes
    Node (float val, Node* ptr) {
        this->id = -1;
        this->h_score = -1;
        this->g_score = -1;
        this->val = val;
        next = ptr;
    };

    Node (long id, float g_score, float h_score, std::vector<long>& path) {
        this->id = id;
        this->g_score = g_score;
        this->h_score = h_score;
        this->val = h_score + g_score;
        this->path = {path.begin(), path.end()};
        next = nullptr;
    };

    Node (long id, float g_score, float h_score) {
        this->id = id;
        this->g_score = g_score;
        this->h_score = h_score;
        this->val = h_score + g_score;
        this->path.push_back(id);
        next = nullptr;
    };
};

Node* find_node(Node *start, float val) {
    Node *curr = start;
    Node *next = curr->next;
    while(next && (next->val < val)) {
        curr = next;
        next = curr->next;
    }
    return curr;
};

class CoarseGrainQueue {
    public:
        Node *tail = new Node(9e20, nullptr);
        Node *head = new Node(-9e20, tail);
        OpenMPLock lock;

        CoarseGrainQueue() = default;

        ~CoarseGrainQueue() = default;

        void enqueue(Node* node) {
            lock.acquire();
            Node *res = find_node(head, node->val);
            node->next = res->next;
            res->next = node;
            lock.release();
        };

        Node* dequeue() {
            lock.acquire();
            Node *res = head->next;
            // Check if the next node is tail
            if (res->id == -1) {
                lock.release();
                return nullptr;
            }
            head->next = res->next;
            res->next = nullptr;
            lock.release();
            return res;
        };
};

#endif // __COARSE_GRAIN_QUEUE_HPP__
