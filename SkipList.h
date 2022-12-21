
#ifndef SKIPLIST_SKIPLIST_H
#define SKIPLIST_SKIPLIST_H

#include <memory>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <deque>
#include <ctime>
#include <unistd.h>

using namespace std;

pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

template<typename K, typename V>
class Node;

template<typename K, typename V>
class SkipList;

template<typename K, typename V>
using node_ptr = std::shared_ptr<Node<K, V>>;

// NODO
template<typename K, typename V>
struct Node{
    using node_ptr = std::shared_ptr<Node<K, V>>;
    Node();
    Node(const K&, const V&, int level);
    ~Node();
    K get_key() const;
    V get_value() const;
    int get_level() const;
    friend SkipList<K, V>;
    int level_;
    const K key_;
    V value_;
    std::vector<node_ptr> forward_;
};


template<typename K, typename V>
Node<K, V>::Node() = default;

template<typename K, typename V>
Node<K, V>::Node(const K& k, const V& v, int level): key_(k), value_(v), level_(level), forward_(level_, nullptr) {}

template<typename K, typename V>
Node<K, V>::~Node() = default;

template<typename K, typename V>
K Node<K, V>::get_key() const { return key_; }

template<typename K, typename V>
V Node<K, V>::get_value() const { return value_; }

template<typename K, typename V>
int Node<K, V>::get_level() const {
    return level_;
}



// SKIP LIST

template<typename K, typename V>
class SkipList {

public:

    SkipList(int level);

    node_ptr<K, V> create_node(const K&, const V&, int level);

    bool search(const K&) const;
    void insert( K,  V);
    int index;
    int nt;

    node_ptr<K, V> erase(const K&);


    void print_list() const ;
    void print_level_size() const ;
    int size() const;
    int get_max_level() const;
private:

    int current_key;
    node_ptr<K, V> search_ptr(const K&) const;

    std::vector<node_ptr<K, V>> get_updates(K k);

    int  random_level();

    int current_level_;

    int max_level_;

    std::atomic<int> element_count_;

    node_ptr<K, V> head_;

    /* concurrency support */
    mutable std::shared_mutex m;
};

template<typename K, typename V>
int SkipList<K, V>::get_max_level() const {
    return this->max_level_;
}


template<typename K, typename V>
node_ptr<K, V> SkipList<K, V>::create_node(const K &k, const V &v, int level) {
    return node_ptr<K, V>(new Node<K, V>(k, v, level));
}

template<typename K, typename V>
SkipList<K, V>::SkipList(int level): max_level_(level), element_count_(0) {
    K k;
    V v;
    index = 0;
    head_ = create_node(k, v, max_level_);
    current_key = -1;
}


template<typename K, typename V>
node_ptr<K, V> SkipList<K, V>::search_ptr(const K &k) const {
//    std::shared_lock<std::shared_mutex> sharedLock(m);
    node_ptr<K, V> prev = this->head_;
    for (int i = this->max_level_ - 1; i >= 0; --i) {
        while (prev->forward_[i] != nullptr && prev->forward_[i]->key_ < k) {
            prev = prev->forward_[i];
        }
        if (prev->forward_[i] != nullptr && prev->forward_[i]->key_ == k) {
            return prev;
        }
    }
    return nullptr;
}

template<typename K, typename V>
std::vector<node_ptr<K, V>> SkipList<K, V>::get_updates(K k) {
    std::vector<node_ptr<K, V>> update(this->max_level_, this->head_);
    node_ptr<K, V> curr = this->head_;
    for (int i = this->max_level_ - 1; i >= 0; --i) {
        while (curr->forward_[i]  && curr->forward_[i]->key_ < k) {
            curr = curr->forward_[i];
        }
        update[i] = curr;
    }
    return update;
}

template<typename K, typename V>
bool SkipList<K, V>::search(const K &k) const {

    return search_ptr(k) != nullptr;
}


template<typename K, typename V>
void SkipList<K, V>::print_list() const {

    std::shared_lock<std::shared_mutex> sharedLock(m);
    for (int i = this->max_level_ - 1; i >= 0; --i) {
         printf("Level %d : ", i);
         auto p = this->head_->forward_[i];
         while (p != nullptr) {
             std::cout << p->get_value() << ' ';
             p = p->forward_[i];
         }
         std::cout << std::endl;
     }
}

template<typename K, typename V>
void SkipList<K, V>::insert(K k,  V v) {

    auto updates = get_updates(k);
    node_ptr<K, V> node = create_node(k, v, random_level());

    //updates
    for (int i = node->get_level() - 1; i >= 0; --i)
    {
        node->forward_[i] = updates[i]->forward_[i];
        updates[i]->forward_[i] = node;
    }
    current_key = -1;
    ++element_count_;
    index++;
}


template<typename K, typename V>
int SkipList<K, V>::size() const {
    return this->element_count_;
}

template<typename K, typename V>
node_ptr<K, V> SkipList<K, V>::erase(const K& k) {

//    std::unique_lock<std::shared_mutex> uniqueLock(m);

    auto update = get_updates(k);

    // not found key
    if (update[0]->forward_[0] == nullptr || update[0]->forward_[0]->key_ != k) {
        return nullptr;
    }


    auto return_ptr = update[0]->forward_[0];
    for (int i = 0; i < update.size(); ++i) {
        if (update[i]->forward_[i] != return_ptr) {
            break;
        }
        update[i]->forward_[i] = return_ptr->forward_[i];
    }
    --element_count_;
    return return_ptr;
}


template<typename K, typename V>
int SkipList<K, V>::random_level() {
    int k = 1;
    while (rand() % 2) {
        k++;
    }
    return std::min(k, this->max_level_);
}


template<typename  K, typename V>
void SkipList<K, V>::print_level_size() const {
    /* Read lock */
//    std::shared_lock<std::shared_mutex> sharedLock(m);
    for (int i = this->max_level_ - 1; i >= 0; --i) {
        int level_size = 0;
        auto p = head_;
        while (p->forward_[i] != nullptr) {
            ++level_size;
            p = p->forward_[i];
        }
        printf("Level %d: size-%d\n", i, level_size);
    }
}

#endif //SKIPLIST_SKIPLIST_H

