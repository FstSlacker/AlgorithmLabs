#include <iostream>
#include "Sort.h"
#include <chrono>
#include <iomanip>
#include <fstream>

template<typename T>
bool check_array(T* arr, int size, bool disp = false) {
    if (disp) {
        for (int i = 0; i < size; i++) {
            std::cout << arr[i] << " ";
        }
    }

    bool sorted = true;
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            sorted = false;
            break;
        }
    }

    if(disp)
        std::cout << std::endl << "Sorted:" << sorted;

    return sorted;
}

void fill_array_quick_sort(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        int val = rand() % size;
        arr[i] = val;
    }
}

void fill_array_insert_sort(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        int val = size - i;
        arr[i] = val;
    }
}

void print_results(double* avrgs1, double* avrgs2, int start, int step, int count) {
    
    std::cout << std::setw(15) << "Quick sort" << std::setw(15) << "Insert sort" << std::setw(10) << "Size" << std::endl;
    
    for (int i = 0; i < count; i++) {

        int s = start + (step * i);
        char c = ' ';

        if (avrgs1[i] < avrgs2[i])
            c = '*';

        std::cout << std::setw(15) << avrgs1[i];
        std::cout << std::setw(15) << avrgs2[i];
        std::cout << std::setw(10) << s << " " << c << std::endl;

    }
}

void save_results(double* avrgs1, double* avrgs2, int start, int step, int count) {
    
    std::ofstream fout("save.csv");

    char sep = ';';

    fout << "Quick sort" << sep << "Insert sort" << sep << "Size" << std::endl;

    for (int i = 0; i < count; i++) {

        int s = start + (step * i);
        char c = ' ';

        if (avrgs1[i] < avrgs2[i])
            c = '*';

        fout << avrgs1[i] << sep;
        fout << avrgs2[i] << sep;
        fout << s << sep << (avrgs1[i] < avrgs2[i] ? "*" : "") << std::endl;
    }

    fout.close();
}

void perfomance_test(int start, int step, int count, int repeat_count) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    using std::chrono::steady_clock;

    double* avrgs1 = new double[count];
    double* avrgs2 = new double[count];

    for (int i = 0; i < count; i++) {
        avrgs1[i] = 0;
        avrgs2[i] = 0;
    }

    for (int k = 0; k < repeat_count; k++) {

        for (int i = 0; i < count; i++) {
            int s = start + (step * i);

            int* arr1 = new int[s];
            int* arr2 = new int[s];

            fill_array_quick_sort(arr1, s);
            fill_array_insert_sort(arr2, s);

            steady_clock::time_point t1;
            steady_clock::time_point t2;
            duration<double, std::milli> duration1_ms;
            duration<double, std::milli> duration2_ms;

            t1 = high_resolution_clock::now();
            quick_sort(arr1, arr1 + (s - 1), [](int a, int b) {return a < b; });
            t2 = high_resolution_clock::now();
            duration1_ms = t2 - t1;

            avrgs1[i] = avrgs1[i] + duration1_ms.count();

            t1 = high_resolution_clock::now();
            insertion_sort(arr2, arr2 + (s - 1), [](int a, int b) {return a < b; });
            t2 = high_resolution_clock::now();
            duration2_ms = t2 - t1;

            avrgs2[i] = avrgs2[i] + duration2_ms.count();

            bool check1, check2;
            check1 = check_array(arr1, s);
            check2 = check_array(arr2, s);

            if (!check1 || !check2) {
                std::cout << "Sort not correct at " << i << " " << check1 << " " << check2;
                return;
            }

            delete[] arr1;
            delete[] arr2;
        }
    }

    for (int i = 0; i < count; i++) {
        avrgs1[i] /= (double)repeat_count;
        avrgs2[i] /= (double)repeat_count;
    }

    print_results(avrgs1, avrgs2, start, step, count);
    save_results(avrgs1, avrgs2, start, step, count);

    delete[] avrgs1;
    delete[] avrgs2;
}

int main()
{
    perfomance_test(2, 1, 30, 10000);
}