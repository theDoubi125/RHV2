#pragma once

#include "util/handle.h"


#ifdef USE_IMGUI
#include <tuple>
#endif

#include "join.h"

namespace action
{
	enum class Direction;
}

template<typename First, typename... Rest>
struct TableElement;

template<typename First, typename... Rest>
struct Table : public Table<Rest...>
{
	using ElementType = TableElement<First, Rest...>;

	template<typename Allocator>
	Table(Allocator& allocator, int capacity) : Table<Rest...>(allocator, capacity)
	{
		first = allocator.allocate<First>(capacity);
	}

	template<typename Allocator>
	Table(Allocator& allocator, const First* firstValues, const Rest*... restValues, int size, int capacity) : Table<Rest...>(allocator, restValues..., size, capacity)
	{
		first = allocator.allocate<First>(capacity);
		for (int i = 0; i < size; i++)
			first[i] = firstValues[i];
	}

	void copy(Table<First, Rest...>& target)
	{
		for (int i = 0; i < count; i++)
		{
			target.first[i] = first[i];
		}
		copy<Rest...>(target);
	}

	void add(const First& firstValue, const Rest&... restValues)
	{
		for (int i = 0; i < count; i++)
		{
			if (firstValue < first[i])
			{
				insert(firstValue, restValues..., i);
				return;
			}
		}
		insert(firstValue, restValues..., count);
	}

	void add(const ElementType& element)
	{
		int index = array::getSortInsertIndex(first, count, element.first);
		insert(element, index);
	}

	void remove(const First& value)
	{
		for (int i = 0; i < count; i++)
		{
			if (first[i] == value)
			{
				removeAt(i);
				return;
			}
		}
		return;
	}

	void removeAt(int index)
	{
		for (int i = index; i < count; i++)
		{

		}
	}

	void insert(const First& firstValue, const Rest&... restValues, int index)
	{
		First bufferValue = first[index];
		First toAdd = firstValue;
		for (int i = index; i < count; i++)
		{
			bufferValue = first[i];
			first[i] = toAdd;
			toAdd = bufferValue;
		}
		first[count] = toAdd;
		Table<Rest...>::insert(restValues..., index);
	}

	void insert(const ElementType& value, int index)
	{
		array::insertAt(first, count, index, value.first);
		Table<Rest...>::insert(value, index);
	}

	template<int sortColumn>
	void quickSort()
	{
		auto& allocator = memory::allocators::stack;
		int* sortIndex = allocator.allocate<int>(count);
		for (int i = 0; i < count; i++) 
			sortIndex[i] = i;
		array::quickSort<First>(get<sortColumn>(), sortIndex, count);
		internal_quickSort<sortColumn>(sortIndex);
	}

	template<int sortColumn>
	void internal_quickSort(int* order)
	{
		array::reorder(order, first, count);
		Table<Rest...>::internal_quickSort<sortColumn - 1>(order);
	}

	template<>
	void internal_quickSort<0>(int* order)
	{
		Table<Rest...>::internal_quickSort<-1>(order);
	}

	template<int index>
	auto get() -> decltype(GetImpl<index, First, Rest...>::value(this));

	template<int index>
	struct Get
	{
		static int value()
		{
			return GetImpl<First, Rest...>::value(this);
		}
	};

#ifdef USE_IMGUI
	void showEditor(char* label) { showTableEditor<First, Rest...>(label, getColumnNames(), this); }
	void showEditor(char* label, TableElement<First, Rest...>* elementToAdd)
	{
		showTableEditor<First, Rest...>(label, getColumnNames(), this, elementToAdd);
	}

	virtual char** getColumnNames() = 0;
#endif

	First* first;
};

template<typename First>
struct Table<First>
{
	template<typename Allocator>
	Table(Allocator& allocator, int in_capacity) : capacity(in_capacity), count(0)
	{
		first = allocator.allocate<First>(in_capacity);
	}

	template<typename Allocator>
	Table(Allocator& allocator, const First* firstValues, int size, int in_capacity)
	{
		first = allocator.allocate<First>(in_capacity);
		for (int i = 0; i < size; i++)
			first[i] = firstValues[i];
		capacity = in_capacity;
		count = size;
	}
	
	void copy(Table<First>& target)
	{
		for (int i = 0; i < count; i++)
		{
			target.first[i] = first[i];
		}
	}

	void insert(const First& value, int index)
	{
		array::insertAt(first, count, index, value);
		count++;
	}

	void insert(const TableElement<First>& element, int index)
	{
		array::insertAt(first, count, index, element.first);
		count++;
	}

	template<int sortColumn>
	void quickSort()
	{
		auto& allocator = memory::allocators::stack;
		int* sortIndex = allocator.allocate<int>(count);
		array::quickSort<First>(get<sortColumn>(), sortIndex, count);
	}

	template<int sortColumn>
	void internal_quickSort(int* order)
	{
		if (sortColumn != 0)
		{
			array::reorder(order, first, count);
		}
	}

	First* first;
	int capacity;
	int count;

#ifdef USE_IMGUI
	void showEditor(char* label) { showTableEditor<First>(label, this); }
	void showEditor(char* label, TableElement<First> elementToAdd) { showTableEditor<First>(label, this, elementToAdd); }
#endif
};

template<typename First, typename... Rest>
struct TableElement : public TableElement<Rest...>
{
	TableElement() {}
	TableElement(First firstValue, Rest... restValues) : TableElement<Rest...>(restValues...), first(firstValue) {}
	
	template<int index>
	auto get() -> decltype(GetImpl<index, First, Rest...>::value(this))
	{
		return GetImpl<index, First, Rest..>::value(this);
	}

#ifdef USE_IMGUI
	void showEditor(char* label) { showElementEditor<First, Rest...>(label, this); }
#endif

	First first;
};

template<typename First>
struct TableElement<First>
{
	TableElement() {}
	TableElement(const First& value) : first(value) {}

	First first;
};

template<typename First, typename... Rest>
template<int index>
auto Table<First, Rest...>::get() -> decltype(GetImpl<index, First, Rest...>::value(this))
{
	return GetImpl<index, First, Rest...>().value(this);
}

template<int index, typename First, typename... Rest>
struct GetImpl
{
	static auto value(const Table<First, Rest...>* t) -> decltype(GetImpl<index - 1, Rest...>::value(t))
	{
		return GetImpl<index - 1, Rest...>::value(t);
	}

	static auto value(const TableElement<First, Rest...>* t) -> decltype(GetImpl<index - 1, Rest...>::value(t))
	{
		return GetImpl<index - 1, Rest...>::value(t);
	}
};

template<typename First, typename... Rest>
struct GetImpl<0, First, Rest...>
{
	static First* value(const Table<First, Rest...>* t)
	{
		return t->first;
	}

	static First* value(const TableElement<First, Rest...>* t)
	{
		return t->first;
	}
};

template<typename First, typename... Rest>
struct GetSizeImpl
{
	static int value()
	{
		return GetSizeImpl<Rest...>::value() + 1;
	}

};

template<typename First>
struct GetSizeImpl<First>
{
	static int value()
	{
		return 1;
	}
};

template<typename First, typename... Rest>
constexpr int getSize(const Table<First, Rest...>* t)
{
	return GetSizeImpl<First, Rest...>::value();
}

template<int index, typename First, typename... Rest>
auto get(const Table<First, Rest...>& t) -> decltype(GetImpl<index, First, Rest...>::value(&t))
{
	return GetImpl<index, First, Rest...>().value(&t);
}

template<typename... Types>
struct HandleTable : public Table<handle, Types...>
{
	template<typename Allocator>
	HandleTable(Allocator& allocator, int capacity) : Table<handle, Types...>(allocator, capacity)
	{
	}

	template<typename Allocator>
	HandleTable(Allocator& allocator, const Types*... values, int size, int capacity) : Table<handle, Types...>(allocator, values..., size, capacity)
	{

	}

	handle add(const Types&... values)
	{
		handle result = nextHandle;
		Table<handle, Types...>::add(nextHandle, values...);
		nextHandle.id++;
		return nextHandle;
	}

	handle nextHandle = { 0 };
};

#ifdef USE_IMGUI
#include "imgui/imgui.h"

template<typename T>
bool showElementEditor(char* label, T* element) { return true; }

template<>
bool showElementEditor<int>(char* label, int* element);

template<>
bool showElementEditor<float>(char* label, float* element);

template<>
bool showElementEditor<ivec2>(char* label, ivec2* element);

template<>
bool showElementEditor<vec2>(char* label, vec2* element);

template<>
bool showElementEditor<action::Direction>(char* label, action::Direction* dir);

template<>
bool showElementEditor<handle>(char* label, handle* element);

template<typename First, typename... Rest>
bool showTableElementEditors(char* label, Table<First, Rest...>* t, int i, int columnIndex = 0)
{
	char buffer[100];
	sprintf_s(buffer, "%s%d", label, columnIndex);
	showElementEditor<First>(buffer, &t->first[i]);
	ImGui::NextColumn();
	showTableElementEditors<Rest...>(label, (Table<Rest...>*)t, i, columnIndex+1);
	return true;
}

template<typename First>
bool showTableElementEditors(char* label, Table<First>* t, int i, int columnIndex = 0)
{
	char buffer[100];
	sprintf_s(buffer, "%s%d", label, columnIndex);
	showElementEditor<First>(buffer, &t->first[i]);
	ImGui::NextColumn();
	return true;
}

template<typename First, typename... Rest>
bool showTableEditor(char* label, char** columnNames, Table<First, Rest...>* t)
{
	ImGui::PushID(label);
	char buffer[100];
	sprintf_s(buffer, "%stoAdd", label);
	int columnCount = getSize(t);
	ImGui::Columns(columnCount);
	for (int i = 0; i < columnCount; i++)
	{
		ImGui::Text(columnNames[i]);
		ImGui::NextColumn();
	}
	for (int i = 0; i < t->count; i++)
	{
		sprintf_s(buffer, "%s%d", label, i);
		showTableElementEditors<First, Rest...>(buffer, t, i);
	}
	ImGui::Columns();
	ImGui::PopID();
	return true;
}

template<typename First, typename... Rest>
bool showTableEditor(char* label, char** columnNames, Table<First, Rest...>* t, TableElement<First, Rest...>* elementToAdd)
{
	ImGui::PushID(label);
	char buffer[100];
	sprintf_s(buffer, "%stoAdd", label);
	showElementEditor<First, Rest...>(buffer, elementToAdd);
	if (ImGui::Button("Add"))
	{
		t->add(*elementToAdd);
	}
	showTableEditor(label, columnNames, t);
	ImGui::PopID();
	return true;
}

template<typename First, typename... Rest>
bool showElementEditor(char* label, TableElement<First, Rest...>* element)
{
	char buffer[100];
	sprintf_s(buffer, "%s%d", label, sizeof...(Rest));
	showElementEditor<First>(buffer, &element->first);
	showElementEditor<Rest...>(buffer, (TableElement<Rest...>*)element);
	return true;
}

template<typename First>
bool showElementEditor(char* label, TableElement<First>* element)
{
	char buffer[100];
	sprintf_s(buffer, "%s%d", label, 0);
	showElementEditor<First>(buffer, &element->first);
	return true;
}

template<typename Allocator, typename T>
struct JoinIterator
{
	JoinIterator(Allocator& in_allocator, const JoinIterator& model) : allocator(in_allocator), sizeA(model.sizeA), sizeB(model.sizeB), cursorA(model.cursoA), cursorB(model.cursorB), constBSequenceSize(model.constBSequenceSize)
	{
		A = allocator.allocate<T>(sizeA);
		AIndexes = allocator.allocate<int>(sizeA);
		B = allocator.allocate<T>(sizeB);
		BIndexes = allocator.allocate<int>(sizeB);
		for (int i = 0; i < sizeA; i++)
		{
			A[i] = model.A[i];
			AIndexes[i] = model.AIndexes[i];
		}

		for (int j = 0; j < sizeB; j++)
		{
			B[i] = model.B[i];
			BIndexes[i] = model.BIndexes[i];
		}
		array::quickSort(A, AIndexes, sizeA);
		array::quickSort(B, BIndexes, sizeB);

		while (!isFinished() && A[cursorA] != B[cursorB])
		{
			if (A[cursorA] < B[cursorB])
				cursorA++;
			else
				cursorB++;
		}
	}

	JoinIterator(Allocator& in_allocator, T* in_A, int in_sizeA, T* in_B, int in_sizeB) : allocator(in_allocator), sizeA(in_sizeA), sizeB(in_sizeB)
	{
		A = allocator.allocate<T>(sizeA);
		AIndexes = allocator.allocate<int>(sizeA);
		B = allocator.allocate<T>(sizeB);
		BIndexes = allocator.allocate<int>(sizeB);
		for (int i = 0; i < sizeA; i++)
		{
			A[i] = in_A[i];
			AIndexes[i] = i;
		}
		for (int i = 0; i < sizeB; i++)
		{
			B[i] = in_B[i];
			BIndexes[i] = i;
		}

		array::quickSort(A, AIndexes, sizeA);
		array::quickSort(B, BIndexes, sizeB);

		while (!isFinished() && A[cursorA] != B[cursorB])
		{
			if (A[cursorA] < B[cursorB])
				cursorA++;
			else
				cursorB++;
		}
	}

	ivec2 getIndexes() const
	{
		return ivec2(AIndexes[cursorA], BIndexes[cursorB]);
	}

	~JoinIterator()
	{
		allocator.free(A);
		allocator.free(B);
		allocator.free(AIndexes);
		allocator.free(BIndexes);
	}

	JoinIterator& operator++(int i)
	{
		if (isFinished())
			return *this;
		if (cursorB + 1 < sizeB && B[cursorB + 1] == B[cursorB])
		{
			cursorB++;
			constBSequenceSize++;
			return *this;
		}
		if (cursorA + 1 < sizeA && A[cursorA + 1] == A[cursorA])
		{
			cursorA++;
			cursorB -= constBSequenceSize;
			constBSequenceSize = 0;
			return *this;
		}
		if (A[cursorA] == B[cursorB])
		{
			cursorA++;
			cursorB++;
		}
		while (!isFinished() && A[cursorA] != B[cursorB])
		{
			if (A[cursorA] < B[cursorB])
				cursorA++;
			else
				cursorB++;
		}

		return *this;
	}

	bool isFinished()
	{
		return cursorA >= sizeA || cursorB >= sizeB;
	}

	T* A;
	T* B;
	int cursorA = 0, cursorB = 0;
	int sizeA, sizeB;
	int constBSequenceSize = 0;
	
	int* AIndexes;
	int* BIndexes;

	Allocator& allocator;
};


#define TABLE_TYPE(Table, Column) decltype(Table::Get<Column>::value())

template<typename ATable, int AJoinColumn, typename BTable, int BJoinColumn>
JoinIterator<memory::FreeListAllocator, TABLE_TYPE(ATable, AJoinColumn)> join(ATable A, BTable B)
{
	using JoinType = JoinIterator<memory::FreeListAllocator, TABLE_TYPE(ATable, AJoinColumn)>;
	return JoinType(memory::allocators::freeList, A.get<AJoinColumn>(), A.count, B.get<BJoinColumn>(), B.count);
}

#endif