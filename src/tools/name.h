#pragma once

#include <SDL.h>
#include <functional>
#include <map>

#define DEBUG_NAMES

class Name
{
public:
	Name();
	Name(const std::string& string);

	bool operator==(const Name& other) const;

	const std::string& toString() const;

	bool operator<(const Name& other) const;

private:
	std::size_t m_hash;
	static std::map<std::size_t, std::string> m_associationTable;
#ifdef DEBUG_NAMES
	std::string m_debugString;
#endif
};