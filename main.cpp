
/* skip list */

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <algorithm>
#include "SkipList.h"
#include <climits>
#include <cassert>
#include <bits/stdc++.h>
using namespace std;

//random numbers
deque<int> randomNums;


void insert_task(SkipList<int, int>& skipList, int size, int init) {
    for (int i = init; i < size + init; ++i) {
        skipList.insert(randomNums[i], randomNums[i]);
    }
}
void search_task(SkipList<int, int>& skipList, int size, int init) {
    for (int i = init; i < size + init; ++i) {
        assert(skipList.search(randomNums[i]));
    }
}
void erase_task(SkipList<int, int>& skipList, int size, int init) {
    for (int i = init; i < size + init; ++i) {
        skipList.erase(randomNums[i]);
    }
}


bool test_multi_thread( int thread_count , int list_level, int element_count ) {
    SkipList<int, int> skipList(list_level);

    /*INFO THREAD */
    if(element_count < thread_count)
        thread_count = element_count;
    auto every_thread_size = element_count / thread_count;
    auto total_size = every_thread_size * thread_count;

    /* INSERT */
    vector<thread> insert_threads; insert_threads.reserve(thread_count);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_count; ++i) {
        insert_threads.emplace_back(insert_task, std::ref(skipList), every_thread_size, i * every_thread_size);
    }
    for_each(insert_threads.begin(), insert_threads.end(), [] (auto& t) {
        t.join();
    });
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> insert_duration = end - start;
    assert(skipList.size() == total_size);


    /* SEARCH */
    vector<thread> search_treads; search_treads.reserve(thread_count);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_count; ++i) {
        search_treads.emplace_back(search_task, std::ref(skipList), every_thread_size, i * every_thread_size);
    }
    for_each(search_treads.begin(), search_treads.end(), [] (auto& t) {
        t.join();
    });
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> search_duration = end - start;
    assert(skipList.size() == total_size);


    /* ERASE */
    vector<thread> erase_threads; erase_threads.reserve(thread_count);
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread_count; ++i) {
        erase_threads.emplace_back(erase_task, std::ref(skipList), every_thread_size, i * every_thread_size);
    }
    for_each(erase_threads.begin(), erase_threads.end(), [] (auto& t) {
        t.join();
    });
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> erase_duration = end - start;
    assert(skipList.size() == 0);

    cout << "\nTest Concurrente de "<<element_count<<" datos con "<<thread_count<<" threads"<< endl;
    cout << "insert duration: " << insert_duration.count() << endl;
    cout << "search duration: " << search_duration.count() << endl;
    cout << "erase  duration: " << erase_duration.count() << endl;
}


template<class T>
void generate_data(long long size)
{
//    cout<<"\nrandom array : \n";
    for (long long i = 0; i < size; ++i) {
        randomNums.push_back(rand() % size + 1) ;
    }
}


int main() {

    srand(time(NULL));


    int n = 1000;
    int level = log(n);
    int nt = std::thread::hardware_concurrency();

    generate_data<int>(n);

    test_multi_thread(1,level,n);
    test_multi_thread(2,level,n);
    test_multi_thread(4,level,n);
    test_multi_thread(8,level,n);

}




