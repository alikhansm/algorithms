#ifndef SORTING_H
#define SORTING_H

#include <iostream>
#include <vector>
#include <algorithm> // std::swap, std::min, std::max, std::make_heap, std::sort_heap
#include <numeric>   // std::iota
#include <functional> // std::function
#include <utility>    // std::pair
#include <future>     // Required for std::async, std::future
#include <thread>     // Required for std::thread::hardware_concurrency (optional info)


const int PARALLEL_MERGE_SORT_THRESHOLD = 2000;
const int PARALLEL_QUICKSORT_THRESHOLD = 2000;


// RECURSIVE MERGE SORT IMPLEMENTATION (PARALELLIZED)

// Helper function to merge sorted subarrays (remains sequential)
inline void mergeHelper(std::vector<int>& arr, std::vector<int>& temp_buffer, int left, int mid, int right) {
    for (int i = left; i <= right; ++i) {
        temp_buffer[i] = arr[i];
    }
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        arr[k++] = (temp_buffer[i] <= temp_buffer[j]) ? temp_buffer[i++] : temp_buffer[j++];
    }
    while (i <= mid) {
        arr[k++] = temp_buffer[i++];
    }
}

// RECURSIVE MERGE SORT IMPLEMENTATION (PARALELLIZED)
inline void parallelMergeSortRecursive(std::vector<int>& arr, std::vector<int>& temp_buffer, int left, int right) {
    if (left >= right) { return; } // Base case

    if ((right - left + 1) <= PARALLEL_MERGE_SORT_THRESHOLD) { // Sequential Execution
        int mid = left + (right - left) / 2;
        parallelMergeSortRecursive(arr, temp_buffer, left, mid);
        parallelMergeSortRecursive(arr, temp_buffer, mid + 1, right);
        mergeHelper(arr, temp_buffer, left, mid, right);
    } else { // Parallel Execution
        int mid = left + (right - left) / 2;
        auto future_left = std::async(std::launch::async, [&]() { parallelMergeSortRecursive(arr, temp_buffer, left, mid); });
        parallelMergeSortRecursive(arr, temp_buffer, mid + 1, right); // Run right half synchronously
        future_left.get(); // Wait for left half
        mergeHelper(arr, temp_buffer, left, mid, right); // Merge
    }
}

inline void mergeSort(std::vector<int>& arr, int left, int right) {
    if (left >= right || arr.empty()) { return; }
    std::vector<int> temp_buffer(arr.size());
    parallelMergeSortRecursive(arr, temp_buffer, left, right);
}


// HEAP SORT IMPLEMENTATION
inline void heapify(std::vector<int>& arr, int n, int i) { 
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;
    if (largest != i) { std::swap(arr[i], arr[largest]); heapify(arr, n, largest); }
}
inline void heapSort(std::vector<int>& arr) { 
    int n = arr.size();
    if (n < 2) return;
    for (int i = n / 2 - 1; i >= 0; --i) heapify(arr, n, i);
    for (int i = n - 1; i > 0; --i) { std::swap(arr[0], arr[i]); heapify(arr, i, 0); }
}


// BUBBLE SORT IMPLEMENTATION
inline void bubble_sort(std::vector<int>& arr) { 
    std::size_t n = arr.size();
    if (n < 2) return;
    for (std::size_t i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (std::size_t j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) { std::swap(arr[j], arr[j + 1]); swapped = true; }
        }
        if (!swapped) break;
    }
}


// INSERTION SORT IMPLEMENTATION
inline void insertion_sort(std::vector<int>& arr) { 
    int n = arr.size();
    if (n < 2) return;
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) { arr[j + 1] = arr[j]; j--; }
        arr[j + 1] = key;
    }
}


// SELECTION SORT IMPLEMENTATION
inline void selection_sort(std::vector<int>& arr) { 
    int n = arr.size();
    if (n < 2) return;
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) { if (arr[j] < arr[min_idx]) min_idx = j; }
        if (min_idx != i) std::swap(arr[i], arr[min_idx]);
    }
}


//PARALLEL QUICK SORT IMPLEMENTATION

// Partition
inline int partition(std::vector<int>& arr, int low, int high) { 
    int pivot = arr[high]; int i = (low - 1);
    for (int j = low; j <= high - 1; j++) { if (arr[j] <= pivot) { i++; if (i != j) std::swap(arr[i], arr[j]); } }
    std::swap(arr[i + 1], arr[high]); return (i + 1);
}
// Median-of-three pivot selection 
inline void medianOfThree(std::vector<int>& arr, int low, int high) { 
    if (low >= high) return; int mid = low + (high - low) / 2;
    if (arr[low] > arr[mid]) std::swap(arr[low], arr[mid]); if (arr[low] > arr[high]) std::swap(arr[low], arr[high]); if (arr[mid] > arr[high]) std::swap(arr[mid], arr[high]);
    std::swap(arr[mid], arr[high]);
}
//  Quick Sort function implementation (PARALELLIZED)
inline void quickSort(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        if ((high - low + 1) <= PARALLEL_QUICKSORT_THRESHOLD) { // Sequential Execution
             medianOfThree(arr, low, high); int p = partition(arr, low, high);
             quickSort(arr, low, p - 1); quickSort(arr, p + 1, high);
        } else { // Parallel Execution
             medianOfThree(arr, low, high); int p = partition(arr, low, high);
             auto future_left = std::async(std::launch::async, [&]() { quickSort(arr, low, p - 1); });
             quickSort(arr, p + 1, high); // Run right half synchronously
             future_left.get(); // Wait for left half
        }
    }
}




#endif 