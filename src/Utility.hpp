#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <string>
#include <sstream>

namespace utility
{
	template< class T >
	std::string to_string( const T& value )
	{
		std::stringstream ss;
		ss << value;

		return ss.str();
	}
}
#endif