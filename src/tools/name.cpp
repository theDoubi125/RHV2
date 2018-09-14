#include "name.h"

std::map<std::size_t, std::string> Name::m_associationTable;

Name::Name()
{

}

Name::Name(const std::string& string)
{
	m_hash = std::hash<std::string>{}(string);
	if (m_associationTable.count(m_hash) == 0)
		m_associationTable[m_hash] = string;
#ifdef DEBUG_NAMES
	m_debugString = string;
#endif
}

bool Name::operator==(const Name& other) const
{
	return other.m_hash == m_hash;
}

const std::string& Name::toString() const
{
	return m_associationTable.at(m_hash);
}

bool Name::operator<(const Name& other) const
{
	return m_hash < other.m_hash;
}