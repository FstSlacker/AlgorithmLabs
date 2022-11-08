#pragma once
#include <cstdlib>
#include <stdexcept>
#include <iostream>


template<typename T>
class Array final
{
public:
	Array();
	Array(size_t capacity);
	~Array();
	Array<T>& operator=(const Array<T>& other);
	Array<T>& operator=(Array<T>&& other);
	Array(const Array<T>& other);
	Array(Array<T>&& other);

	const T& operator[](int index) const;
	T& operator[](int index);
	int insert(int index, const T& value);
	int insert(const T& value);
	void remove(int index);
	int size() const;
	int capacity(); // for tests

	class ConstIterator
	{
	public:
		ConstIterator(Array<T>* arr_ptr, bool reverse);

		const T& get() const;
		void next();
		bool hasNext() const;

	protected:
		Array<T>* m_arr_ptr;
		int m_position;
		int m_inc;

	};

	class Iterator : public ConstIterator {
	public:
		Iterator(Array<T>* arr_ptr, bool reverse);
		void set(const T& value);
	};

	Iterator iterator();
	Iterator reverseIterator();
	ConstIterator iterator() const;
	ConstIterator reverseIterator() const;

private:
	static constexpr size_t kDefaultCapacity = 8;
	size_t m_capacity;
	size_t m_size;
	T* items;

	T* alloc_array(size_t arr_size);
	void free_array();
	void resize(size_t new_capacity);
	void swap_with(Array<T>& other);

};

template<typename T>
void Array<T>::free_array() {
	for (int i = 0; i < m_size; i++)
		items[i].~T();
	free(items);
}

template<typename T>
void Array<T>::resize(size_t new_capacity)
{
	T* new_items = alloc_array(new_capacity);

	if (new_items == nullptr)
		throw std::bad_alloc();

	for (int i = 0; i < m_size; i++) {
		new(new_items + i) T(std::move(items[i]));
		items[i].~T();
	}

	free(items);

	m_capacity = new_capacity;
	items = new_items;

}

template<typename T>
Array<T>::Array() : Array(kDefaultCapacity) {}

template<typename T>
Array<T>::Array(size_t capacity)
{
	if (capacity < 1)
		throw std::out_of_range("Capacity must be greater or equal 1!");

	m_size = 0;
	m_capacity = capacity;
	items = alloc_array(capacity);
}

template<typename T>
Array<T>::Array(const Array<T>& other) {

	m_capacity = other.m_capacity;
	m_size = other.m_size;
	items = alloc_array(m_capacity);

	for (int i = 0; i < m_size; i++) {
		new(items + i) T(other.items[i]);
	}
}

template<typename T>
Array<T>::Array(Array<T>&& other) {
	m_capacity = other.m_capacity;
	m_size = other.m_size;
	items = other.items;

	other.m_size = 0;
	other.m_capacity = 0;
	other.items = nullptr;
}

template<typename T>
Array<T>::~Array() {
	free_array();
}

template<typename T>
Array<T>& Array<T>::operator=(const Array<T>& other) {

	Array<T> tmp(other);

	swap_with(tmp);

	return *this;
}

template<typename T>
Array<T>& Array<T>::operator=(Array<T>&& other) {

	free_array();

	m_capacity = other.m_capacity;
	m_size = other.m_size;
	items = other.items;

	other.m_size = 0;
	other.m_capacity = 0;
	other.items = nullptr;

	return *this;
}

template<typename T>
void Array<T>::swap_with(Array<T>& other) {
	std::swap(items, other.items);
	std::swap(m_capacity, other.m_capacity);
	std::swap(m_size, other.m_size);
}

template<typename T>
int Array<T>::insert(const T& value)
{
	return insert(m_size, value);
}

template<typename T>
void Array<T>::remove(int index)
{
	if (index >= m_size || index < 0)
		throw std::out_of_range("Index must be positive and less then array size!");

	for (int i = index; i < m_size - 1; i++) {
		items[i] = items[i + 1];
	}

	items[m_size - 1].~T();
	m_size--;
}

template<typename T>
int Array<T>::insert(int index, const T& value)
{
	if (index > m_size || index < 0)
		throw std::out_of_range("Index must be positive and less or equal then array size!");

	if (m_capacity == m_size) {
		resize(m_capacity * 2);
	}

	for (int i = m_size; i > index; i--) {
		new(&items[i]) T(std::move(items[i - 1]));
	}

	new(items + index) T(value);

	m_size++;

	return index;
}

template<typename T>
const T& Array<T>::operator[](int index) const
{
	return items[index];
}

template<typename T>
T& Array<T>::operator[](int index)
{
	return items[index];
}

template<typename T>
int Array<T>::size() const
{
	return m_size;
}

template<typename T>
int Array<T>::capacity() {
	return m_capacity;
}

template<typename T>
T* Array<T>::alloc_array(size_t arr_size) {
	T* arr = (T*)malloc(sizeof(T) * arr_size);

	return arr;
}

template<typename T>
Array<T>::ConstIterator::ConstIterator(Array<T>* arr_ptr, bool reverse) {
	m_arr_ptr = arr_ptr;
	m_inc = reverse ? -1 : 1;
	if (arr_ptr->size() > 0)
		m_position = reverse ? arr_ptr->size() - 1 : 0;
	else
		m_position = -1;
}

template<typename T>
Array<T>::Iterator::Iterator(Array<T>* arr_ptr, bool reverse) : ConstIterator(arr_ptr, reverse) { }

template<typename T>
const T& Array<T>::ConstIterator::get() const {
	return (*m_arr_ptr)[m_position];
}

template<typename T>
void Array<T>::Iterator::set(const T& value) {
	(*(ConstIterator::m_arr_ptr))[ConstIterator::m_position] = value;
}

template<typename T>
void Array<T>::ConstIterator::next() {
	m_position += m_inc;
}

template<typename T>
bool Array<T>::ConstIterator::hasNext() const {
	return (m_position >= 0) && (m_position < m_arr_ptr->size());
}

template<typename T>
typename Array<T>::Iterator Array<T>::iterator() {
	return Iterator(this, false);
}

template<typename T>
typename Array<T>::Iterator Array<T>::reverseIterator() {
	return Iterator(this, true);
}

template<typename T>
typename Array<T>::ConstIterator Array<T>::iterator() const {
	return ConstIterator(this, false);
}

template<typename T>
typename Array<T>::ConstIterator Array<T>::reverseIterator() const {
	return ConstIterator(this, true);
}

