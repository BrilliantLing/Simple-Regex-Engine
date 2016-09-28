#ifndef _MATCH_

#define	_MATCH_

#include"RegularExpression.h"
#include"CharClass.h"

int		RunDFA(int *CharClass, int **DFA, bool *Final, char *Input);

void	Match
(int *CharClass, int **DFA, bool *Final, char *Input, char **Begin, int *length);

#endif // !_MATCH_
