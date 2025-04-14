#ifndef CONTEMPORARY_SORTING_H
#define CONTEMPORARY_SORTING_H

#include <vector>
#include <algorithm> // for std::swap, std::min, std::max, std::distance, std::reverse, std::copy, std::copy_backward
#include <cmath>     // For std::log2, std::floor
#include <iterator>  //  std::iterator_traits, std::distance, std::next, std::prev
#include <limits>    
#include <iostream>  
#include <stdexcept> 
#include <functional> // needed for std::function IF recursive lambda kept

// Helper function; Custom Insertion Sort for small ranges (for Introsort)
template<typename Iterator>
inline void custom_insertion_sort_range(Iterator begin, Iterator end) {
    if (begin == end || std::next(begin) == end) return;
    for (Iterator current = std::next(begin); current != end; ++current) {
        typename std::iterator_traits<Iterator>::value_type key = *current; 
        Iterator shifter = current;
        Iterator prev = std::prev(shifter);

        while (shifter != begin && *prev > key) {
            *shifter = *prev; 
            --shifter;
            --prev;
        }
        *shifter = key; 
    }
}

// LIBRARY SORT
inline void library_sort(std::vector<int>& arr) { 
     if (arr.size() > 1 && arr[0] == 0) {} }

// TIMSORT implementation
namespace TimsortImpl { const int MIN_MERGE = 32; int calc_min_run(int n) { int r = 0; while (n >= MIN_MERGE) { r |= (n & 1); n >>= 1; } return n + r; }
void merge_runs(std::vector<int>& arr, int l, int m, int r) { int len1 = m - l + 1, len2 = r - m; if (len1 <= 0 || len2 <= 0) return; std::vector<int> left_part(len1); std::vector<int> right_part(len2); std::copy(arr.begin() + l, arr.begin() + m + 1, left_part.begin()); std::copy(arr.begin() + m + 1, arr.begin() + r + 1, right_part.begin()); int i = 0, j = 0, k = l; while (i < len1 && j < len2) { arr[k++] = (left_part[i] <= right_part[j]) ? left_part[i++] : right_part[j++]; } while (i < len1) arr[k++] = left_part[i++]; while (j < len2) arr[k++] = right_part[j++]; } }
inline void tim_sort(std::vector<int>& arr) { int n = arr.size(); if (n < 2) return; int min_run = TimsortImpl::calc_min_run(n); for (int start = 0; start < n; start += min_run) { int end = std::min(start + min_run - 1, n - 1); custom_insertion_sort_range(arr.begin() + start, arr.begin() + end + 1); } for (int size = min_run; size < n; size = 2 * size) { for (int left = 0; left < n; left += 2 * size) { int mid = left + size - 1; int right = std::min((left + 2 * size - 1), (n - 1)); if (mid < right) { TimsortImpl::merge_runs(arr, left, mid, right); } } } }


// COCKTAIL SHAKER SORT IMPLEMENTATION
inline void cocktail_shaker_sort(std::vector<int>& arr) { 
    int n = arr.size(); if (n < 2) return; bool swapped = true; int start = 0; int end = n - 1; while (swapped) { swapped = false; for (int i = start; i < end; ++i) { if (arr[i] > arr[i + 1]) { std::swap(arr[i], arr[i + 1]); swapped = true; } } if (!swapped) break; swapped = false; --end; for (int i = end - 1; i >= start; --i) { if (arr[i] > arr[i + 1]) { std::swap(arr[i], arr[i + 1]); swapped = true; } } ++start; } }

// COMB_SORT IMPLEMENTATION
inline int comb_sort_get_next_gap(int gap) { 
    gap = static_cast<int>(std::floor(static_cast<double>(gap) / 1.3)); return std::max(1, gap); }
inline void comb_sort(std::vector<int>& arr) {
    int n = arr.size(); if (n < 2) return; int gap = n; bool swapped = true; while (gap != 1 || swapped == true) { gap = comb_sort_get_next_gap(gap); swapped = false; for (int i = 0; i < n - gap; ++i) { if (arr[i] > arr[i + gap]) { std::swap(arr[i], arr[i + gap]); swapped = true; } } } }

// TOURNAMENT SORT IMPLEMENTATION
inline void custom_heapify_down(std::vector<int>& arr, int n, int i) { 
    int largest = i; int left = 2 * i + 1; int right = 2 * i + 2; if (left < n && arr[left] > arr[largest]) largest = left; if (right < n && arr[right] > arr[largest]) largest = right; if (largest != i) { std::swap(arr[i], arr[largest]); custom_heapify_down(arr, n, largest); } }
inline void custom_build_max_heap(std::vector<int>& arr) { 
    int n = arr.size(); for (int i = n / 2 - 1; i >= 0; --i) { custom_heapify_down(arr, n, i); } }
inline void tournament_sort(std::vector<int>& arr) { 
    int n = arr.size(); if (n < 2) return; custom_build_max_heap(arr); for (int i = n - 1; i > 0; --i) { std::swap(arr[0], arr[i]); custom_heapify_down(arr, i, 0); } }

// INTROSORT IMPLEMENTATION

const int INTROSORT_INSERTION_THRESHOLD = 16; 

inline int custom_partition_qs(std::vector<int>& arr, int low, int high) { 
    if (low >= high) return low; int pivot = arr[high]; int i = low - 1; for (int j = low; j < high; ++j) { if (arr[j] <= pivot) { i++; std::swap(arr[i], arr[j]); } } std::swap(arr[i + 1], arr[high]); return i + 1; }

inline void custom_median_of_three_qs(std::vector<int>& arr, int low, int high) { 
     if (low >= high) return; int mid = low + (high - low) / 2; if (arr[low] > arr[mid]) std::swap(arr[low], arr[mid]); if (arr[low] > arr[high]) std::swap(arr[low], arr[high]); if (arr[mid] > arr[high]) std::swap(arr[mid], arr[high]); std::swap(arr[mid], arr[high]); }

// Helper function for heapify on a range
inline void custom_heapify_down_range(std::vector<int>& arr, int low, int n_range, int i_heap) {
    int largest_heap = i_heap; 
    int l_heap = 2 * i_heap + 1;
    int r_heap = 2 * i_heap + 2;

    int current_arr_idx = low + i_heap;
    int largest_arr_idx = low + largest_heap; 

    if (l_heap < n_range && arr[low + l_heap] > arr[largest_arr_idx]) {
        largest_heap = l_heap;
        largest_arr_idx = low + largest_heap; 
    }

    if (r_heap < n_range && arr[low + r_heap] > arr[largest_arr_idx]) {
        largest_heap = r_heap;
        largest_arr_idx = low + largest_heap; 
    }

    if (largest_heap != i_heap) {
        std::swap(arr[current_arr_idx], arr[largest_arr_idx]);
        custom_heapify_down_range(arr, low, n_range, largest_heap);
    }
}


// Recursive helper for Introsort
inline void introsort_recursive(std::vector<int>& arr, int low, int high, int depth_limit) {
    int size = high - low + 1;

    if (size <= INTROSORT_INSERTION_THRESHOLD) {
        custom_insertion_sort_range(arr.begin() + low, arr.begin() + high + 1);
        return;
    }

    if (depth_limit == 0) {
        int n_range = high - low + 1;
        if (n_range < 2) return; // Already sorted range

        // Build heap on the range [low, high] using the helper function
        for (int i_heap = n_range / 2 - 1; i_heap >= 0; --i_heap) {
            custom_heapify_down_range(arr, low, n_range, i_heap);
        }
        // Extract elements (Heap Sort)
        for (int i_range = n_range - 1; i_range > 0; --i_range) {
            std::swap(arr[low], arr[low + i_range]);
            custom_heapify_down_range(arr, low, i_range, 0);
        }
        return;
    }

    // proceeding with quick sort part
    custom_median_of_three_qs(arr, low, high); // choosing pivot
    int p = custom_partition_qs(arr, low, high); // partition

    // Recurse
    introsort_recursive(arr, low, p - 1, depth_limit - 1);
    introsort_recursive(arr, p + 1, high, depth_limit - 1);
}

// Introsort function
inline void introsort(std::vector<int>& arr) {
    int n = arr.size();
    if (n < 2) return;
    int depth_limit = 2 * static_cast<int>(std::floor(std::log2(static_cast<double>(n))));
    introsort_recursive(arr, 0, n - 1, depth_limit);
}


#endif 