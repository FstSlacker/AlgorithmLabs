#include "pch.h"
#include "../../QuickSort/QuickSort/Sort.h"
#include <string.h>

bool comp_int(int a, int b) { return a < b; }
bool comp_str(std::string a, std::string b) { return a < b; }

int* generate_array_int(int size, int (*generate_value)(int)) {
	int* arr = new int[size];

	for (int i = 0; i < size; i++)
		arr[i] = generate_value(i);

	return arr;
}

std::string* generate_array_str(int size, std::string(*generate_value)(int)) {
	std::string* arr = new std::string[size];

	for (int i = 0; i < size; i++)
		arr[i] = generate_value(i);

	return arr;
}

bool check_sort_int(int* arr, int size) {
	for (int i = 0; i < size - 1; i++) {
		if (arr[i] > arr[i + 1])
			return false;
	}
	return true;
}

bool check_sort_str(std::string* arr, int size) {
	for (int i = 0; i < size - 1; i++) {
		if (arr[i] > arr[i + 1])
			return false;
	}
	return true;
}



TEST(SortInt, SortOneItem) {
	int arr[1] = { 1 };

	sort(arr, arr, &comp_int);

	EXPECT_EQ(arr[0], 1);
}

TEST(SortInt, SortTwoItems) {
	int size = 2;
	//increase
	int* arr1 = generate_array_int(size, [](int ind) { return ind; });
	//decrease
	int* arr2 = generate_array_int(size, [](int ind) { return -ind; });
	//equal
	int* arr3 = generate_array_int(size, [](int ind) { return 1; });

	sort(arr1, arr1 + (size - 1), &comp_int);
	sort(arr2, arr2 + (size - 1), &comp_int);
	sort(arr3, arr3 + (size - 1), &comp_int);

	EXPECT_EQ(check_sort_int(arr1, size), true);
	EXPECT_EQ(check_sort_int(arr2, size), true);
	EXPECT_EQ(check_sort_int(arr3, size), true);

	delete[] arr1;
	delete[] arr2;
	delete[] arr3;
}

TEST(SortInt, SortMoreItems) {
	int size = 100;
	//increase
	int* arr1 = generate_array_int(size, [](int ind) { return ind; });
	//decrease
	int* arr2 = generate_array_int(size, [](int ind) { return -ind; });
	//equal
	int* arr3 = generate_array_int(size, [](int ind) { return 1; });
	//alternate
	int* arr4 = generate_array_int(size, [](int ind) { return ind % 2; });
	//random
	int* arr5 = generate_array_int(size, [](int ind) { return rand() % 100; });

	sort(arr1, arr1 + (size - 1), &comp_int);
	sort(arr2, arr2 + (size - 1), &comp_int);
	sort(arr3, arr3 + (size - 1), &comp_int);
	sort(arr4, arr4 + (size - 1), &comp_int);
	sort(arr5, arr5 + (size - 1), &comp_int);

	EXPECT_EQ(check_sort_int(arr1, size), true);
	EXPECT_EQ(check_sort_int(arr2, size), true);
	EXPECT_EQ(check_sort_int(arr3, size), true);
	EXPECT_EQ(check_sort_int(arr4, size), true);
	EXPECT_EQ(check_sort_int(arr5, size), true);

	delete[] arr1;
	delete[] arr2;
	delete[] arr3;
	delete[] arr4;
	delete[] arr5;
}

TEST(SortStr, SortOneItem) {
	std::string arr[1] = { "a" };

	sort(arr, arr, &comp_str);

	EXPECT_EQ(arr[0], "a");
}

TEST(SortStr, SortTwoItems) {
	int size = 2;
	//increase
	std::string* arr1 = generate_array_str(size, [](int ind) { return std::string("" + (char)ind); });
	//decrease
	std::string* arr2 = generate_array_str(size, [](int ind) { return std::string("" + (char)(100 - ind)); });
	//equal
	std::string* arr3 = generate_array_str(size, [](int ind) { return std::string("a"); });

	sort(arr1, arr1 + (size - 1), &comp_str);
	sort(arr2, arr2 + (size - 1), &comp_str);
	sort(arr3, arr3 + (size - 1), &comp_str);

	EXPECT_EQ(check_sort_str(arr1, size), true);
	EXPECT_EQ(check_sort_str(arr2, size), true);
	EXPECT_EQ(check_sort_str(arr3, size), true);

	delete[] arr1;
	delete[] arr2;
	delete[] arr3;
}

TEST(SortStr, SortMoreItems) {
	int size = 100;

	//increase
	std::string* arr1 = generate_array_str(size, [](int ind) { return std::string("" + (char)ind); });
	//decrease
	std::string* arr2 = generate_array_str(size, [](int ind) { return std::string("" + (char)(100 - ind)); });
	//equal
	std::string* arr3 = generate_array_str(size, [](int ind) { return std::string("a"); });
	//alternate
	std::string* arr4 = generate_array_str(size, [](int ind) { return std::string("" + (char)(ind % 2)); });
	//random
	std::string* arr5 = generate_array_str(size, [](int ind) { return std::string("" + (char)(rand() % 100)); });

	sort(arr1, arr1 + (size - 1), &comp_str);
	sort(arr2, arr2 + (size - 1), &comp_str);
	sort(arr3, arr3 + (size - 1), &comp_str);
	sort(arr4, arr4 + (size - 1), &comp_str);
	sort(arr5, arr5 + (size - 1), &comp_str);

	EXPECT_EQ(check_sort_str(arr1, size), true);
	EXPECT_EQ(check_sort_str(arr2, size), true);
	EXPECT_EQ(check_sort_str(arr3, size), true);
	EXPECT_EQ(check_sort_str(arr4, size), true);
	EXPECT_EQ(check_sort_str(arr5, size), true);

	delete[] arr1;
	delete[] arr2;
	delete[] arr3;
	delete[] arr4;
	delete[] arr5;
}