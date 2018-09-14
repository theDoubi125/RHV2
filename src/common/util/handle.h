#pragma once

struct handle
{
	unsigned int id;

	inline bool operator>(const handle& other) const
	{
		return id > other.id;
	}

	inline bool operator<(const handle& other) const
	{
		return id < other.id;
	}

	inline bool operator==(const handle& other) const
	{
		return id == other.id;
	}

	inline bool operator!=(const handle& other) const
	{
		return id != other.id;
	}
};