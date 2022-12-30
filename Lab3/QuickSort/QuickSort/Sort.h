#pragma once

template<typename T, typename Compare>
void insertion_sort(T* first, T* last, Compare comp) {
	for (T* i = first + 1; i <= last; i++) {

		T* j = i - 1;
		T val = *i;

		while (j >= 0 && comp(val, *j)) {
			*(j + 1) = std::move(*j);
			j--;
		}

		*(j + 1) = val;
	}
}

template<typename T, typename Compare>
void quick_sort(T* first, T* last, Compare comp) {

	while (last - first > 0) {

		T* mid = (last - first) / 2 + first;
		T* pivot = comp(*last, *first) ? comp(*first, *mid) ? first : mid : comp(*last, *mid) ? last : mid;
	
		std::swap(*first, *pivot);
		pivot = first;

		T* fp = first + 1;
		T* bp = last;

		while (1) {
			while (comp(*fp, *pivot)) fp++;
			while (comp(*pivot, *bp)) bp--;

			if (fp >= bp)
				break;

			std::swap(*(fp++), *(bp--));
		}

		std::swap(*bp, *pivot);

		if (bp - first > last - bp) {
			quick_sort(bp + 1, last, comp);
			last = bp - 1;
		}
		else {
			quick_sort(first, bp - 1, comp);
			first = bp + 1;
		}
	}
}

constexpr int kInsertSortTreshold = 14;

template<typename T, typename Compare>
void sort(T* first, T* last, Compare comp) {

	while (last - first > kInsertSortTreshold) {

		T* mid = (last - first) / 2 + first;
		T* pivot = comp(*last, *first) ? comp(*first, *mid) ? first : mid : comp(*last, *mid) ? last : mid;

		std::swap(*first, *pivot);
		pivot = first;

		T* fp = first + 1;
		T* bp = last;

		while (1) {
			while (comp(*fp, *pivot)) fp++;
			while (comp(*pivot, *bp)) bp--;

			if (fp >= bp)
				break;

			std::swap(*(fp++), *(bp--));
		}

		std::swap(*bp, *pivot);

		if (bp - first > last - bp) {
			sort(bp + 1, last, comp);
			last = bp - 1;
		}
		else {
			sort(first, bp - 1, comp);
			first = bp + 1;
		}
	}

	insertion_sort(first, last, comp);

}