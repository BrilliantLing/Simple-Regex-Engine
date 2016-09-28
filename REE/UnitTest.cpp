#include<iostream>

char*	Trans(char* str)
{
	char*	Read = str;
	char*	New = new char[strlen(str)];
	char*	Write = New;
	while (*Read!='\0')
	{
		if (*Read == '\\')
			Read++;
		*Write = *Read;
		Read++;
		Write++;
	}
	*Write = 0;
	return New;
}

int		main()
{
	char* New = new char[100];
	gets(New);
	std::cout << Trans(New) << std::endl;
}