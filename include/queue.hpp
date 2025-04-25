#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__

#include "dependency.hpp"

struct Node {
    long id;
    float h_score;
    float g_score;
    std::vector<long> path;

    std::atomic<Node*> next;

    Node(long id, float g_score, float h_score) {
        this->id = id;
        this->h_score = h_score;
        this->g_score = g_score;
        this->path.push_back(id);

        this->next.store(nullptr);
    }

    Node(long id, float g_score, float h_score, std::vector<long>& path) {
        this->id = id;
        this->h_score = h_score;
        this->g_score = g_score;
        this->path = {path.begin(), path.end()};

        this->next.store(nullptr);
    }

    float val() {
        return this->h_score + this->g_score;
    }
};

class Queue {
    protected:
        Node* head;
    
        Node* contain(float val) {
            Node* curr = this->head;
            Node* prev = nullptr;

            while (curr != nullptr) {
                if (val < curr->val()) {
                    break;
                }

                prev = curr;
                curr = curr->next.load();
            }

            return prev;
        }

    public:
        Queue() {
            this->head = new Node(-1, -1, -1);
        }
        
        ~Queue() {
            while (this->head != nullptr) {
                Node* curr = this->head;
                this->head = curr->next;

                delete curr;
            }
        }

        void dump() {
            Node* curr = this->head->next;
            while (curr != nullptr) {
                std::cout << curr->id << " ---\n";
                curr = curr->next;
            }

            std::cout << std::endl;
        }

        virtual void enqueue(Node* node) = 0;
        virtual Node* dequeue() = 0;
};

class SequentialQueue : public Queue {    
    public:
        void enqueue(Node* node) override {
            Node* pivot = this->contain(node->val());

            node->next.store(pivot->next);
            pivot->next = node;
        }

        Node* dequeue() override {
            Node* node = this->head->next;
            if (node != nullptr) {
                this->head->next.store(node->next);
            }

            return node;
        }
};

// class FineGrainQueue : public Queue {
//     public:
//         void enqueue(Node* node) override {
//             // TODO
//         }

//         Node* dequeue() override {
//             // TODO
//             return nullptr;
//         }
// };

// class CoarseGrainQueue : public Queue {
//     public:
//         void enqueue(Node* node) override {
//             // TODO
//         }

//         Node* dequeue() override {
//             // TODO
//             return nullptr;
//         }
// };

class LockFreeQueue : public Queue {    
    public:
        void enqueue(Node* node) override {
            Node* pivot = this->contain(node->val());

            do {
                Node* next = pivot->next.load();
                node->next.store(next);

                if (next != nullptr && next->val() < node->val()) {
                    pivot = next;
                    continue;
                }

                if (pivot->next.compare_exchange_strong(next, node, std::memory_order_acq_rel)) return;
            }
            while (true);
        }

        Node* dequeue() override {
            Node* node = this->head->next.load();

            if (node != nullptr) {
                Node* next = node->next.load();
                while(!this->head->next.compare_exchange_strong(node, next, std::memory_order_acq_rel)) {
                    if (node == nullptr) break;
                    next = node->next.load();
                }
            }

            return node;
        }
};

class ScheduleLockQueue : public Queue {
    private:
        std::mutex mtx;

    public:
        void enqueue(Node* node) override {
            std::lock_guard<std::mutex> lock(mtx);
            Node* pivot = this->contain(node->val());

            node->next.store(pivot->next);
            pivot->next.store(node);
            
        }

        Node* dequeue() override {
            std::lock_guard<std::mutex> lock(mtx);
            Node* node = this->head->next;
            if (node != nullptr) {
                this->head->next.store(node->next);
            }

            return node;
        }
};

class SpinLockQueue : public Queue {
    private:
        std::atomic_flag lock = false;

    public:
        void enqueue(Node* node) override {
            while (this->lock.test_and_set(std::memory_order_acquire));
            
            Node* pivot = this->contain(node->val());
            node->next.store(pivot->next);
            pivot->next.store(node);

            this->lock.clear(std::memory_order_release);
        }

        Node* dequeue() override {
            while (this->lock.test_and_set(std::memory_order_acquire));
            Node* node = this->head->next;
            if (node != nullptr) {
                this->head->next.store(node->next);
            }

            this->lock.clear(std::memory_order_release);
            return node;
        }
};

#endif