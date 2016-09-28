#ifndef _CHAR_CLASS_

#define _CHAR_CLASS_

#include<vector>

using	std::vector;

class	CharClass
{
public:
	int	Set[128];

public:
	void	GetCharClass(std::vector<char> &);
};

#endif // !_CHAR_CLASS_
