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

	void add(const First& firstValue, const TableElement<Rest...> restValues)
	{
		int index = array::getSortInsertIndex(first, count, firstValue);
		insert(firstValue, restValues, index);
	}

	void add(const ElementType& element)
	{
		int index = array::getSortInsertIndex(first, count, element.first);
		insert(element, index);
	}

	// toRemove must be sorted, it contains indexes of the cells that must be removed
	void removeAt(int* toRemove, int toRemoveCount)
	{
		int toMoveCursor = 0;
		int targetCursor = 0;
		int toRemoveCursor = 0;
		for (int toMoveCursor = 0; toMoveCursor < count; toMoveCursor++)
		{
			for (; toRemoveCursor < toRemoveCount && toRemove[toRemoveCursor] < toMoveCursor; toRemoveCursor++);
			if (toRemoveCursor >= toRemoveCount || toRemove[toRemoveCursor] != toMoveCursor)
			{
				first[targetCursor] = first[toMoveCursor];
				targetCursor++;
			}
		}
		Table<Rest...>::removeAt(toRemove, toRemoveCount);
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

	void insert(const First& firstValue, const TableElement<Rest...> restValues, int index)
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
		Table<Rest...>::insert(restValues, index);
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
	void showEditor(char* label)
	{
		ImGui::Columns(getSize(this));
		showColumnsEditor();
		for(int i=0; i<count; i++)
			showRowEditor(label, i);
	}

	void showEditor(char* label, TableElement<First, Rest...>* elementToAdd)
	{
		char buffer[100];
		sprintf_s(buffer, "%s editor", label);
		ImGui::PushID(buffer);
		ImGui::Columns(getSize(this) + 1);
		showElementEditor<First, Rest...>(label, elementToAdd);
		int columnCount = ImGui::GetColumnsCount();
		ImGui::Columns(1);
		if (ImGui::Button("Add"))
		{
			add(*elementToAdd);
		}
		ImGui::Columns(columnCount);
		showColumnsEditor();
		ImGui::NextColumn();
		for (int i = 0; i < count; i++)
		{
			showRowEditor(label, i);
			sprintf_s(buffer, "remove %s %d", label, i);
			if (ImGui::Button(buffer))
			{
				removeAt(&i, 1);
				count--;
			}
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void showColumnsEditor()
	{
		int columnCount = getSize(this);
		for (int i = 0; i < columnCount; i++)
		{
			ImGui::Text(getColumnNames()[i]);
			ImGui::NextColumn();
		}
	}

	void showRowEditor(char* label, int row)
	{
		char buffer[100];
		sprintf_s(buffer, "%s%d", label, row);
		showTableElementEditors<First, Rest...>(buffer, this, row);
	}

	virtual char** getColumnNames() = 0;
#endif

	First* first;
};

template<typename First>
struct Table<First>
{
	using ElementType = TableElement<First>;
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

	void removeAt(int* toRemove, int toRemoveCount)
	{
		int toMoveCursor = 0;
		int targetCursor = 0;
		int toRemoveCursor = 0;
		for (int toMoveCursor = 0; toMoveCursor < count; toMoveCursor++)
		{
			for (; toRemoveCursor < toRemoveCount && toRemove[toRemoveCursor] < toMoveCursor; toRemoveCursor++);
			if (toRemove[toRemoveCursor] != toMoveCursor)
			{
				first[targetCursor] = first[toMoveCursor];
				toMoveCursor++;
			}
		}
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
	showTableElementEditors<Rest...>(label, (Table<Rest...>*)t, i, columnIndex + 1);
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
	ImGui::NextColumn();
	showElementEditor<Rest...>(buffer, (TableElement<Rest...>*)element);
	return true;
}

template<typename First>
bool showElementEditor(char* label, TableElement<First>* element)
{
	char buffer[100];
	sprintf_s(buffer, "%s%d", label, 0);
	showElementEditor<First>(buffer, &element->first);
	ImGui::NextColumn();
	return true;
}

#define TABLE_TYPE(Table, Column) decltype(Table::Get<Column>::value())


#endif

#include "handle_table.h"
