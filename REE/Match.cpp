#include"Match.h"

int		RunDFA(int *charClass, int **DFATable, bool *Final, char *Input)
{
	int		LastFinalLength = -1;
	int		StatusNum = 0;

	char*	Init = Input;

	while (StatusNum != -1)
	{
		if (Final[StatusNum])
		{
			LastFinalLength = Input - Init;
		}
		int	cc = charClass[*Input];
		Input++;
		if (cc < 0)
			break;
		StatusNum = DFATable[StatusNum][cc];
	}
	return	LastFinalLength;
}

void
Match(int *charClass, int **DFATable, bool *Final, char *Input, char **Begin, int *length)
{
	*Begin = 0;
	while (*Input)
	{
		int	Len = RunDFA(charClass, DFATable, Final, Input);
		if (Len != -1)
		{
			*Begin = Input;
			*length = Len;
			return;
		}
		Input++;
	}
}