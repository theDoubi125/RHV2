#pragma once

#define MAX_BINDINGS_COUNT 100
#define MAX_BINDING_SIZE 100

#define TABLE_CONSTRUCTOR(ClassName, ParentClass) template<typename Allocator> ClassName(Allocator& allocator, int capacity) : ParentClass(allocator, capacity) {}

#ifdef USE_IMGUI
#define TABLE_DEBUG_COLUMNS(...) char* columns[100] = { __VA_ARGS__ }; virtual char** getColumnNames() { return columns; }
#else
#define TABLE_DEBUG_COLUMNS(...)
#endif

// when updated, removes from the bound table every element corresponding to the content of the removed array
template<int BUFFER_SIZE>
struct TableBinding
{
	TableBinding()
	{

	}

	void remove(handle* toRemove, int count) 
	{
		for (int i = 0; i < count; i++)
		{
			removed[removedCount + i] = toRemove[i];
		}
		removedCount += count;
	}

	template<typename TableType, int boundColumn>
	void update(TableType& table)
	{
		auto& allocator = memory::allocators::stack;
		allocator.pushStack();
		array::quickSort(removed, removedCount);

		handle* sortedColumnElements = allocator.allocate<handle>(table.count);
		int* sortedRowNumber = allocator.allocate<int>(table.count);
		int* rowsToRemove = allocator.allocate<int>(table.count);
		int rowsToRemoveCursor = 0;
		for (int i = 0; i < table.count; i++)
		{
			sortedColumnElements[i] = table.get<boundColumn>()[i];
			sortedRowNumber[i] = i;
		}
		array::quickSort(sortedColumnElements, sortedRowNumber, table.count);

		// sortedRow & removed are now sorted => join with a single iteration
		// result = row index of an element containing a removed entity in the column TRACKED_COLUMN
		int sortedRowCursor = 0;
		int removedCursor = 0;
		for (; sortedRowCursor < table.count; sortedRowCursor++)
		{
			for (; removedCursor < removedCount && removed[removedCursor] < sortedColumnElements[sortedRowCursor]; removedCursor++);
			if (removedCursor < removedCount && removed[removedCursor] == sortedColumnElements[sortedRowCursor])
			{
				rowsToRemove[rowsToRemoveCursor] = sortedRowNumber[sortedRowCursor];
				rowsToRemoveCursor++;
			}
		}
		allocator.popStack();
		removedCount = 0;
		array::quickSort(rowsToRemove, rowsToRemoveCursor);
		table.removeAt(rowsToRemove, rowsToRemoveCursor);
		table.count -= rowsToRemoveCursor;
	}

	handle removed[BUFFER_SIZE];
	int removedCount = 0;
};

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

	handle add(const TableElement<Types...> element)
	{
		handle result = nextHandle;
		Table<handle, Types...>::add(nextHandle, element);
		nextHandle.id++;
		return nextHandle;
	}

	void removeAt(int* indexes, int toRemoveCount)
	{
		auto& allocator = memory::allocators::stack;
		allocator.pushStack();
		handle* toRemove = allocator.allocate<handle>(toRemoveCount);
		for (int i = 0; i < toRemoveCount; i++)
		{
			toRemove[i] = first[indexes[i]];
		}
		for (int i = 0; i < bindingsCount; i++)
		{
			bindings[i].remove(toRemove, toRemoveCount);
		}
		Table<handle, Types...>::removeAt(indexes, toRemoveCount);
		allocator.popStack();
	}

	TableBinding<MAX_BINDING_SIZE>& addBinding()
	{
		bindings[bindingsCount] = TableBinding<MAX_BINDING_SIZE>();
		return bindings[bindingsCount++];
	}

	// toRemove must be sorted
	void removeAll(handle* toRemove, int toRemoveCount)
	{
		auto& allocator = memory::allocators::stack;
		allocator.pushStack();
		int toRemoveCursor = 0;
		int* toRemoveIndexes = allocator.allocate<int>(count);
		int indexesCount = 0;
		for (int i = 0; i < count; i++)
		{
			for (; toRemoveCursor < toRemoveCount && toRemove[toRemoveCursor] < first[i]; toRemoveCursor++);

			if (toRemove[toRemoveCursor] == first[i])
			{
				toRemoveIndexes[indexesCount] = i;
				indexesCount++;
			}
		}
		removeAt(toRemoveIndexes, indexesCount);
		count -= indexesCount;
		allocator.popStack();
	}

	void showEditor(char* label, typename Table<Types...>::ElementType* elementToAdd)
	{
		char buffer[100];
		sprintf_s(buffer, "%s editor", label);
		ImGui::PushID(buffer);
		ImGui::Columns(getSize(this) + 1);
		showElementEditor<Types...>(label, elementToAdd);
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

	TableBinding<MAX_BINDING_SIZE> bindings[MAX_BINDINGS_COUNT];
	int bindingsCount = 0;

	handle nextHandle = { 0 };
};

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

template<typename... Types>
struct DeletedIterator
{
	DeletedIterator(HandleTable<Types...>& in_table, handle* in_removed, int in_removedCount) : table(in_table), removed(in_removed), removedCount(in_removedCount)
	{
		if (!isCurrentValid())
			operator++(0);
	}

	inline void moveCursor()
	{
		if (cursor >= table.count)
			return;
		handle currentValue = table.first[cursor];
		while (cursor < table.count && currentValue == table.first[cursor])
		{
			cursor++;
		}
	}

	inline bool isCurrentValid()
	{
		if (removedCursor >= removedCount || cursor >= table.count)
			return true;
		while (removedCursor < removedCount && removed[removedCursor] < table.first[cursor]) { removedCursor++; }
		return !(removedCursor < removedCount && cursor < table.count && removed[removedCursor] == table.first[cursor]);
	}

	DeletedIterator operator++(int i)
	{
		if (cursor < table.count)
		{
			cursor++;
			for (; removedCursor < removedCount && removed[removedCursor] < table.first[cursor]; removedCursor++);
			while (cursor < table.count && table.first[cursor] == removed[removedCursor])
			{
				moveCursor();
				for (; removedCursor < removedCount && removed[removedCursor] < table.first[cursor]; removedCursor++);
			}
		}
		return *this;
	}

	bool isFinished() const
	{
		return cursor >= table.count;
	}

	HandleTable<Types...>& table;
	handle* removed;
	int removedCount;
	int cursor = 0;
	int removedCursor = 0;
};

template<typename ATable, int AJoinColumn, typename BTable, int BJoinColumn>
JoinIterator<memory::FreeListAllocator, TABLE_TYPE(ATable, AJoinColumn)> join(ATable A, BTable B)
{
	using JoinType = JoinIterator<memory::FreeListAllocator, TABLE_TYPE(ATable, AJoinColumn)>;
	return JoinType(memory::allocators::freeList, A.get<AJoinColumn>(), A.count, B.get<BJoinColumn>(), B.count);
}