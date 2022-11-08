#include "pch.h"
#include "../../DynamicArray/DynamicArray/myarray.cpp"

TEST(ArrayTest, ArrayConstructorsTests) {
    Array<int> arr1;
    Array<int> arr2(10);

    EXPECT_EQ(arr1.capacity(), 8);
    EXPECT_EQ(arr1.size(), 0);

    EXPECT_EQ(arr2.capacity(), 10);
    EXPECT_EQ(arr2.size(), 0);
}

TEST(ArrayTest, ArrayInsertTest) {
    Array<int> arr;
    Array<std::string> arr2;

    arr.insert(11);
    arr.insert(22);
    arr.insert(33);

    arr2.insert("11");
    arr2.insert("22");
    arr2.insert("33");

    EXPECT_EQ(arr[0], 11);
    EXPECT_EQ(arr[1], 22);
    EXPECT_EQ(arr[2], 33);
    EXPECT_EQ(arr.size(), 3);

    EXPECT_EQ(arr2[0], "11");
    EXPECT_EQ(arr2[1], "22");
    EXPECT_EQ(arr2[2], "33");
    EXPECT_EQ(arr2.size(), 3);
}

TEST(ArrayTest, ArrayInsertResizeTest) {
    Array<int> arr(2);
    Array<std::string> arr2(2);

    arr.insert(11);
    arr.insert(22);
    arr.insert(33);

    arr2.insert("11");
    arr2.insert("22");
    arr2.insert("33");

    EXPECT_EQ(arr[0], 11);
    EXPECT_EQ(arr[1], 22);
    EXPECT_EQ(arr[2], 33);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr.capacity(), 4);

    EXPECT_EQ(arr2[0], "11");
    EXPECT_EQ(arr2[1], "22");
    EXPECT_EQ(arr2[2], "33");
    EXPECT_EQ(arr2.size(), 3);
    EXPECT_EQ(arr2.capacity(), 4);
}

TEST(ArrayTest, ArrayInsertToStartTest) {
    Array<int> arr(2);
    Array<std::string> arr2(2);

    arr.insert(22);
    arr.insert(33);
    arr.insert(0, 11);

    arr2.insert("22");
    arr2.insert("33");
    arr2.insert(0, "11");

    EXPECT_EQ(arr[0], 11);
    EXPECT_EQ(arr[1], 22);
    EXPECT_EQ(arr[2], 33);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr.capacity(), 4);

    EXPECT_EQ(arr2[0], "11");
    EXPECT_EQ(arr2[1], "22");
    EXPECT_EQ(arr2[2], "33");
    EXPECT_EQ(arr2.size(), 3);
    EXPECT_EQ(arr2.capacity(), 4);
}

TEST(ArrayTest, ArrayInsertToMiddleTest) {
    Array<int> arr(2);
    Array<std::string> arr2(2);

    arr.insert(11);
    arr.insert(33);
    arr.insert(1, 22);

    arr2.insert("11");
    arr2.insert("33");
    arr2.insert(1, "22");

    EXPECT_EQ(arr[0], 11);
    EXPECT_EQ(arr[1], 22);
    EXPECT_EQ(arr[2], 33);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr.capacity(), 4);

    EXPECT_EQ(arr2[0], "11");
    EXPECT_EQ(arr2[1], "22");
    EXPECT_EQ(arr2[2], "33");
    EXPECT_EQ(arr2.size(), 3);
    EXPECT_EQ(arr2.capacity(), 4);
}

TEST(ArrayTest, ArrayRemoveTest) {
    Array<int> arr(2);
    Array<std::string> arr2(2);

    arr.insert(11);
    arr.insert(22);
    arr.insert(33);

    arr.remove(1);

    arr2.insert("11");
    arr2.insert("22");
    arr2.insert("33");

    arr2.remove(1);

    EXPECT_EQ(arr[0], 11);
    EXPECT_EQ(arr[1], 33);
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr.capacity(), 4);

    EXPECT_EQ(arr2[0], "11");
    EXPECT_EQ(arr2[1], "33");
    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2.capacity(), 4);
}

TEST(ArrayTest, ArrayIteratorForTest) {
    Array<int> arr(2);

    for (int i = 0; i < 10; i++) {
        arr.insert(i);
    }

    int ind = 0;
    for (auto it = arr.iterator(); it.hasNext(); it.next()) {
        EXPECT_EQ(it.get(), ind);
        ind++;
    }
}

TEST(ArrayTest, ArrayIteratorSetTest) {
    Array<int> arr(2);

    for (int i = 1; i < 10; i++) {
        arr.insert(i);
    }

    int ind = 1;
    for (auto it = arr.iterator(); it.hasNext(); it.next()) {
        it.set(-ind);
        ind++;
    }

    for (int i = 1; i < 10; i++) {
        EXPECT_EQ(arr[i - 1], -i);
    }
}

TEST(ArrayTest, ArrayReverseIteratorForTest) {
    Array<int> arr(2);

    for (int i = 0; i < 10; i++) {
        arr.insert(i);
    }

    int ind = 9;
    for (auto it = arr.reverseIterator(); it.hasNext(); it.next()) {
        EXPECT_EQ(it.get(), ind);
        ind--;
    }
}

TEST(ArrayTest, ArrayReverseIteratorSetTest) {
    Array<int> arr(2);

    for (int i = 1; i < 10; i++) {
        arr.insert(i);
    }

    int ind = 1;
    for (auto it = arr.reverseIterator(); it.hasNext(); it.next()) {
        it.set(-ind);
        ind++;
    }

    ind = 1;
    for (int i = arr.size() - 1; i >= 0; i--) {
        EXPECT_EQ(arr[i], -ind);
        ind++;
    }
}

TEST(ArrayTest, ArrayCopyTest) {
    Array<int> arr(2);

    for (int i = 0; i < 10; i++) {
        arr.insert(i);
    }

    Array<int> arr1(arr);
    Array<int> arr2 = arr1;

    EXPECT_EQ(arr.size(), arr1.size());
    EXPECT_EQ(arr1.size(), arr2.size());

    EXPECT_EQ(arr.capacity(), arr1.capacity());
    EXPECT_EQ(arr1.capacity(), arr2.capacity());

    for (int i = 0; i < arr.size(); i++) {
        EXPECT_EQ(arr[i], arr1[i]);
        EXPECT_EQ(arr1[i], arr2[i]);
    }
}

TEST(ArrayTest, TwoDimentionalArrayTest) {
    Array<Array<int>> arr;

    for (int i = 0; i <= 10; i++) {
        arr.insert(Array<int>());
        for (int j = 0; j <= 12; j++) {
            arr[i].insert(i * 12 + j);
        }
    }

    for (int i = 0; i <= 10; i++) {
        for (int j = 0; j <= 12; j++) {
            EXPECT_EQ(arr[i][j], i * 12 + j);
        }
    }
}

