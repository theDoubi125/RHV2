#include "movement.h"
#include "util/array/sorted.h"


void updateMovement(MovementTable& table, float deltaTime)
{
	for (int i = 0; i < table.count; i++)
	{
		table.positions[i] += table.velocities[i] * deltaTime;
	}
}

void applyForces(MovementTable& movementTable, ForcesTable& forcesTable, float deltaTime)
{
	JoinIterator<memory::FreeListAllocator, handle> joinIterator(memory::allocators::freeList, movementTable.characters, movementTable.count, forcesTable.characters, movementTable.count);
	for (; !joinIterator.isFinished(); joinIterator++)
	{
		int movementCursor = joinIterator.getIndexes().x;
		int forcesCursor = joinIterator.getIndexes().y;
		movementTable.velocities[movementCursor] += forcesTable.forces[forcesCursor] * deltaTime;
	}
}

void removeMovements(MovementTable& movement, handle* toRemove, int count)
{
	movement.removeAll(toRemove, count);
	for (int i = 0; i < count; i++)
		movement.removed[i + movement.removedCount];
	movement.removedCount += count;
}

void ForcesTable::assignBindings(MovementTable& movementTable)
{
	entityBinding = &movementTable.addBinding();
}

void ForcesTable::updateBindings()
{
	entityBinding->update<ForcesTable, 0>(*this);
}