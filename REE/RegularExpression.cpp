#include"RegularExpression.h"
#include<cstring>

bool		Is(char*& Stream, const char* Text)
{
	size_t len = strlen(Text);
	char* Read = Stream;
	while (*Read == ' ')Read++;
	if (strncmp(Read, Text, len) == 0)
	{
		Stream = Read + len;
		return true;
	}
	else
	{
		return false;
	}
}

int			AnalyseInteger(char* start, char* end)
{
	char	*Read = start;
	bool	Error = false;
	int		Num = 0;
	while (Read != end)
	{
		if (*Read > '9' || *Read < '0')
		{
			Error = true;
			break;
		}
		Num = Num * 10 + *Read - '0';
	}
	if (Error)
		throw	SyntaxError(Read, "这不是一个数字");
	return Num;
}

char*		AnalyseTransMean(char* str)
{
	char*	Read = str;
	char*	New = new char[strlen(str)];
	char*	Write = New;
	while (*Read != '\0')
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

bool		AnalyseCharacter(char c)
{
	if ('a' <= c <= 'z' ||
		'A' <= c < 'Z' ||
		'0' <= c <= '9')
		return true;
	else
		return false;
}

bool		AnalyseRangeChar(char beg,char end)
{
	if ('a' <= beg <= 'z'&&'a' <= end <= 'z' ||
		'A' <= beg <= 'Z'&&'A' <= end <= 'Z' ||
		'0' <= beg <= '9'&&'0' <= end <= '9')
		return true;
	else
		return false;
}

RegExpPtr	GetCharSet(char *&stream)
{
	char	*Read = stream;
	char	*Content = NULL;
	int		Length = 0;
	bool	GotCharClass = false;
	while (true)
	{
		if (*Read == ' ')
			Read++;
		else
			break;
	}
	stream = Read;
	if (*Read == '[')
	{
		GotCharClass = true;
		//char	*temp = Read;
		while (*Read == ']')
		{
			Read++;
		}
		if (*Read == 0)
			throw SyntaxError("需要一个']'", Read);
		else
		{
			Length = stream - Read;
			Read++;
		}
	}
	else if (*Read >= 'a'&&*Read <= 'z' ||
		*Read >= 'A'&&*Read <= 'Z' ||
		*Read >= '0'&&*Read <= '9' ||
		*Read == '\\')
	{
		GotCharClass = true;
		Length=2;
		Read++;
	}
	if (GotCharClass)
	{
		Content = new char[Length];
		strncpy(Content, stream, Length - 1);
		Content[Length - 1] = '\0';
		stream = Read;
		return	new RegExpCharSet(Content);
	}
	else
		throw	SyntaxError("需要一个字符集", stream);
}

RegExpPtr	GetPrim(char *&stream)
{
	RegExpPtr	Prim = NULL;
	try
	{
		Prim=GetCharSet(stream);
	}
	catch (SyntaxError &e)
	{
		char	*Read = stream;
		if (Is(Read, "("))
		{
			{
				//这是错的！
				RegExp	*ChildExpr = GetExpr(Read);
				if (Is(Read, ")"))
				{
					stream = Read;
					return ChildExpr;
				}
				else
				{
					delete	ChildExpr;
					throw	SyntaxError(stream, "需要右括号");
				}
			}
		}
		else
		{
			throw e;
		}
	}
	return	Prim;
}

RegExpPtr	GetLoop(char *&stream)
{
	char		*Read = stream;
	RegExpPtr	Result = GetPrim(Read);
	RegExpPtr	LoopFunc = Result;
	char*		temp = NULL;
	int			Times = 0, Max = 0, Min = 0;
	//Read++;
	switch (*Read)
	{
	case '+':
		LoopFunc = new RegExpLoop(1, Result);
		Read++;
		break;
	case '*':
		LoopFunc = new RegExpLoop(0, Result);
		Read++;
		break;
	case '{':
		if (*Read > '9' || *Read < '0')
		{
			throw SyntaxError(Read, "重复次数需要是个数字");
		}
		temp = Read;
		while ('0' <= *temp <= '9')
			temp++;
		switch (*temp)
		{
		case '}':
			Times = AnalyseInteger(Read, temp - 1);
			LoopFunc = new	RegExpLoop(Times, Result);
			Read++;
			break;
		case ',':
			Min = AnalyseInteger(Read, temp - 1);
			temp++;
			Read = temp;
			while ('0' <= *temp <= '9')
				temp++;
			if (*temp != '}')
				throw	SyntaxError(temp, "需要一个}");
			Max = AnalyseInteger(Read, temp - 1);
			LoopFunc = new	RegExpLoop(Min, Max, Result);
			Read++;
			break;
		default:
			//throw	SyntaxError(Read, "s");
			break;
		}
		break;
	default:
		break;
	}
	stream = Read;
	return	LoopFunc;
}

RegExpPtr	GetExpr(char *&stream)
{
	char*			Read = stream;
	RegExpPtr		Result = NULL;
	RegExpSection*	EBranch = NULL;
	RegExpSection*	ESequence = NULL;
	ESequence = new RegExpSection(Sequence);
	Result = ESequence;
	while (true)
	{
		if (*Read == '\0' || *Read == ')')
			break;
		RegExpPtr		CurrentExp = NULL;
		//ESequence = new RegExpSection(Sequence);
		try
		{
			CurrentExp = GetLoop(Read);
		}
		catch (SyntaxError &e)
		{
			delete	CurrentExp;
			throw e;
		}
		ESequence->Add(CurrentExp);	
		//Read = stream;
		switch (*Read)
		{
		case '|':
			if (!EBranch)
			{
				EBranch = new RegExpSection(Branch);
			}
			EBranch->Add(ESequence);
			ESequence = new RegExpSection(Sequence);
			EBranch->Add(ESequence);
			Result = EBranch;
			Read++;
			break;
		default:
			break;
		}
	}
	stream = Read;
	return Result;
}

void
RegExpCharSet::AnalyseString()
{
	TransCharList = AnalyseTransMean(CharList);
	char	*Curr = TransCharList;
	char	*Prev = Curr + 1;

	vector<std::pair<char, char>>	stack;

	if (*Curr=='[')
	{
		while (true)
		{
			if (*Prev == ']' || *Prev == '\0')
			{
				break;
			}
			if (*Prev == '^')
			{
				Reverse = true;
			}
			else if (AnalyseCharacter(*Prev))
			{
				Curr = Prev;
				Prev += 1;
				switch(*Prev)
				{
				case '-':
					Prev++;
					if (!AnalyseCharacter(*Prev))
						throw	SyntaxError(Prev, "[...]中的字符不可以是字母，数字之外的字符");
					else if (AnalyseRangeChar(*Curr,*Prev))
						throw	LexicalError(Prev, "错误的字符序列");
					else
					{
						stack.push_back({ *Curr, *Prev });
						Curr = Prev;
					}
				case ']':
					break;
				default:
					throw	LexicalError(Curr, "错误的字符集构造");
				}
			}
			Prev++;
			Curr++;
		}
	}
	else if (Reverse)
	{
		vector<char>	charvec;
		if (stack.empty())
		{
			//vector<char>	charvec;
			char c = 0;
			while (c >= 0)
			{
				if (isprint(c) &&
					c != *TransCharList)
					charvec.push_back(c);
				c++;
			}
			delete	TransCharList;
			TransCharList = new char[charvec.size() + 1];
			int index = 0;
			for (auto iter = charvec.begin();
				iter != charvec.end();
				iter++)
			{
				TransCharList[index] = *iter;
				index++;
			}
			TransCharList[index] = '\0';
		}
		else
		{
			for (auto iter = stack.begin();
				iter != stack.end();
				iter++)
			{
				for (char c = iter->first;
					c == iter->second;
					c++)
					charvec.push_back(c);
			}
			char c = 0;
			while (c >= 0)
			{
				if (isprint(c) &&
					std::find(charvec.begin(),charvec.end(),c)!=charvec.end())
					charvec.push_back(c);
				c++;
			}
			int index = 0;
			delete TransCharList;
			TransCharList = new char[charvec.size() + 1];
			for (auto iter = charvec.begin();
				iter != charvec.end();
				iter++)
			{
				TransCharList[index] = *iter;
				index++;
			}
		}
	}
}