#include"CharClass.h"
#include<algorithm>

void
CharClass::GetCharClass(vector<char> &vec)
{
	int		Index = 0;
	int		CharIndex = 0;
	char	*ChClass = new char[128];
	std::sort(vec.begin(), vec.end());
	vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
	for (auto iter = vec.begin();
		iter != vec.end();
		iter++)
	{
		ChClass[Index] = *iter;
		Index++;
	}
	ChClass[Index] = '\0';
	char	*Read = ChClass;
	while (*Read != '\0')
	{
		Set[*Read] = CharIndex;
		CharIndex++;
	}
}