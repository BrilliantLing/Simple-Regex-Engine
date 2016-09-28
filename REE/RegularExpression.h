#ifndef _REGULAR_EXPRESSION_

#define _REGULAR_EXPRESSION_

#include<list>
#include<vector>
#include<iostream>
#include<utility>
#include"DFA.h"
#include<algorithm>

using	std::list;
using	std::vector;
using	std::cerr;
using	std::cout;
using	std::endl;

struct Exception
{
	virtual void	Show() = 0;
};

struct SyntaxError:public Exception
{
	char	*Info;
	char	*Position;
	SyntaxError(char *pos, char *err) 
		:Info(err), Position(pos){}
	void	Show()
	{
		cout << "语法错误：" << endl
			<< "位置：" << Position << endl
			<< "信息：" << Info << endl;
	}
};

struct LexicalError:public Exception
{
	char	*Info;
	char	*Position;
	LexicalError(char *pos, char *err)
		:Info(err), Position(pos){}
	void	Show()
	{
		cout << "词法错误：" << endl
			<< "位置：" << Position << endl
			<< "信息：" << Info << endl;
	}
};

class	RegExp
{
public:
	virtual	void	Visit() = 0;
	virtual	void	MergeCharSet(vector<char> &vec) = 0;
	virtual	std::pair<Status*, Status*>*	
		BuildFA() = 0;
};

typedef	RegExp*	RegExpPtr;

class	RegExpCharSet:public RegExp
{
public:
	char	*CharList;
	char	*TransCharList;
	bool	Reverse;

public:
	RegExpCharSet(char *str) :CharList(str), Reverse(false){};
	RegExpCharSet(char *str, bool rev) :CharList(str), Reverse(rev){}
	RegExpCharSet() :CharList(NULL), Reverse(false){}

	void	Visit()
	{
		cout << "这是一个字符集" << endl;
	}

	void	AnalyseString();

	void	MergeCharSet(vector<char> &vec)
	{
		AnalyseString();
		char	*Read = TransCharList;
		while (*Read=='\0')
		{
			vec.push_back(*Read);
			Read++;
		}
	}
	
	std::pair<Status*, Status*>*	
		BuildFA();

};

class	RegExpLoop :public RegExp
{
public:
	int			MinTimes;
	int			MaxTimes;
	bool		Infinite;
	RegExpPtr	Expression;
public:
	RegExpLoop()
		:MinTimes(0), MaxTimes(0),
		Infinite(false), Expression(NULL){}
	RegExpLoop(int min, int max, RegExpPtr exp)
		:MinTimes(min), MaxTimes(max),
		Infinite(false), Expression(exp){}
	RegExpLoop(int min, RegExpPtr exp)
		:MinTimes(min), MaxTimes(-1),
		Infinite(true), Expression(exp){}

	void	Visit()
	{
		cout << "这是一个重复" << endl;
		if (Expression != NULL)
			Expression->Visit();
	}
	void	MergeCharSet(vector<char> &vec)
	{
		Expression->MergeCharSet(vec);
	}

	std::pair<Status*, Status*>*	
		BuildFA();

};

enum Section
{
	Sequence,
	Branch
};

class	RegExpSection:public RegExp
{
public:
	typedef	list<RegExpPtr>	RegExpList;

public:
	RegExpList		Next;
	Section			Type;
	//RegExpPtr		Content;

public:
	RegExpSection()
		:Next(RegExpList()), Type(Sequence){}
	RegExpSection(Section s)
		:Next(RegExpList()), Type(s){}

	void	Add(RegExpPtr ptr)
	{
		Next.push_back(ptr);
	}

	void	Visit()
	{
		if (Type == Sequence)
			cout << "串联" << endl;
		else
			cout << "并联" << endl;
		//cout << endl;
		if (!Next.empty())
		{
			for (auto iter = Next.begin();
				iter != Next.end();
				iter++)
				(*iter)->Visit();
		}
		cout << endl;
	}

	void	MergeCharSet(vector<char> &vec)
	{
		if (!Next.empty())
		{
			for (auto iter = Next.begin();
				iter != Next.end();
				iter++)
				(*iter)->MergeCharSet(vec);
		}
	}

	std::pair<Status*, Status*>*	
		BuildFA();

};

bool Is(char*& Stream, const char* Text);

RegExpPtr		GetCharSet(char *&stream);

RegExpPtr		GetPrim(char *&stream);

RegExpPtr		GetLoop(char *&stream);

RegExpPtr		GetExpr(char *&stream);

char*	AnalyseTransMean(char* );


#endif // !_REGULAR_EXPRESSION_
