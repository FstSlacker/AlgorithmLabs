#include <iostream>
#include "Array.h"

int main(int argc, char** argv)
{

    int cap;
    std::cout << "Input capacity:";
    std::cin >> cap;

    Array<Array<int>> arr(cap);

    for (int i = 0; i < 12; i++) {
        Array<int> a(cap);
        arr.insert(a);

        for (int j = 0; j < 12; j++) {
            arr[i].insert(j * i);
        }
    }

    std::cout << "Inserts" << std::endl;

    for (int i = 0; i < 12; i++) {
        std::cout << std::endl;
        for (int j = 0; j < 12; j++)
            std::cout << arr[i][j] << " ";
    }

    //for (auto it = arr.iterator(); it.hasNext(); it.next()) {
    //    std::cout << it.get() << std::endl;
    //}



    /*for (int i = 0; i < 12; i++) {
        arr.remove(0);
    }

    std::cout << "Removes" << std::endl;

    for (int i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << std::endl;
    }*/

    //std::cout << "Outs" << std::endl;

    return EXIT_SUCCESS;
}