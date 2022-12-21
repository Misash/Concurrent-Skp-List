
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


void insert_parallel(SkipList<int, int>& skipList, int size, int init) {
    for (int i = init; i < size + init; ++i) {
        skipList.insert(randomNums[i], randomNums[i]);
    }
}
void search_parallel(SkipList<int, int>& skipList, int size, int init) {
    for (int i = init; i < size + init; ++i) {
        assert(skipList.search(randomNums[i]));
    }
}
void erase_parallel(SkipList<int, int>& skipList, int size, int init) {
    for (int i = init; i < size + init; ++i) {
        skipList.erase(randomNums[i]);
    }
}



void test_insert( int level, int elements )
{


    cout << "\nTEST INSERT " << endl;
    for (int threads = 1; threads <= 8; threads*=2)
    {
        SkipList<int, int> skipList(level);

        if(threads == 2) sort(randomNums.begin(),randomNums.end());

        /*INFO THREAD */
        if(elements < threads)
            threads = elements;

        auto every_thread_size = elements / threads;
        auto total_size = every_thread_size * threads;


        /* INSERT */

        vector<thread> insert_threads;
        insert_threads.reserve(threads);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < threads; ++i) {
            insert_threads.emplace_back(insert_parallel, std::ref(skipList), every_thread_size, i * every_thread_size);
        }
        for_each(insert_threads.begin(), insert_threads.end(), [] (auto& t) {
            t.join();
        });

//    skipList.print_list();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> insert_duration = end - start;


        cout << "Insert de " <<elements<<" datos con "<<threads<<" threads "<< insert_duration.count() <<" ms"<< endl;
    }
}


void test_search(int level, int elements)
{
    SkipList<int, int> skipList(level);

    for (auto elems : randomNums) {
        skipList.insert(elems,elems);
    }

    cout << "\nTEST SEARCH " << endl;
    for (int threads = 1; threads <= 8; threads*=2)
    {


        /*INFO THREAD */
        if(elements < threads)
            threads = elements;

        auto every_thread_size = elements / threads;
        auto total_size = every_thread_size * threads;

        /* SEARCH */
        vector<thread> search_treads;
        search_treads.reserve(threads);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < threads; ++i) {
            search_treads.emplace_back(search_parallel, std::ref(skipList), every_thread_size, i * every_thread_size);
        }
        for_each(search_treads.begin(), search_treads.end(), [] (auto& t) {
            t.join();
        });
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> search_duration = end - start;


        cout << "Search de " <<elements<<" datos con "<<threads<<" threads "<< search_duration.count() <<" ms"<< endl;
    }

}

void test_delete(int level, int elements)
{


    cout << "\nTEST DELETE " << endl;
    for (int threads = 1; threads <= 8; threads*=2)
    {

        SkipList<int, int> skipList(level);

        for (auto elems : randomNums) {
            skipList.insert(elems,elems);
        }

        /*INFO THREAD */
        if(elements < threads)
            threads = elements;

        auto every_thread_size = elements / threads;
        auto total_size = every_thread_size * threads;

        /* ERASE */
        vector<thread> erase_threads; erase_threads.reserve(threads);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < threads; ++i) {
            erase_threads.emplace_back(erase_parallel, std::ref(skipList), every_thread_size, i * every_thread_size);
        }
        for_each(erase_threads.begin(), erase_threads.end(), [] (auto& t) {
            t.join();
        });
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> erase_duration = end - start;
        assert(skipList.size() == 0);

        cout << "Delete de " <<elements<<" datos con "<<threads<<" threads "<< erase_duration.count() <<" ms"<< endl;
    }

}


template<class T>
void generate_data(long long size)
{
//    cout<<"\nrandom array : \n";
    for (long long i = 0; i < size; ++i) {
        randomNums.push_back(rand() % size + 1) ;
//        cout<<randomNums[i]<<" ";
    }
}


int main() {

    srand(time(NULL));


    int n = 100000;
    int level = log(n);
    int nt = std::thread::hardware_concurrency();

    generate_data<int>(n);


    test_insert(level,n);
    test_search(level,n);
    test_delete(level,n);

}




