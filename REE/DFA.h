#ifndef _DFA_

#define	_DFA_

#include<list>
#include<map>
#include<queue>
#include<set>
#include"CharClass.h"

#define		STATUS_START	0
#define		STATUS_END		1
#define		STATUS_COMMON	2
#define		STATUS_STEND	3

using	std::list;
using	std::map;
using	std::set;
using	std::queue;
using	std::multimap;

class	Edge;

class	Status
{
public:
	typedef	list<Edge*>	EdgeList;
public:
	EdgeList		InEdges;
	EdgeList		OutEdges;
	int				Attribute;
	//bool			Visited;

	Status()
		:Attribute(STATUS_COMMON){}
	Status(int sp)
		:Attribute(sp){}

	Status(const Status &state) = delete;



	void	SetCommon()
	{
		Attribute = STATUS_COMMON;
	}

};

class	Edge
{
public:
	vector<char>	MatchContent;
	Status			*Start;
	Status			*End;

public:
	Edge()
		:Start(NULL), End(NULL){}
	Edge(Status* st, Status* en)
		:Start(st), End(en){}
	Edge(char *str, Status *st, Status *en)
		:Start(st), End(en)
	{
		char	*Read = str;
		while (*Read)
		{
			MatchContent.push_back(*Read);
		}
	}
	Edge(const Edge& edge) = delete;

};

class	DFA
{
public:
	Status*				Start;
	vector<Status*>		StatusMem;
	vector<Edge*>		EdgeMem;
	int**				StatusGraph;
	bool*				StatusTable;
	int					Count;
	CharClass			cSet;

public:
	DFA() :Start(NULL), Count(0),
		StatusGraph(NULL){}
	DFA(Status* sta)
		:Start(sta), Count(0),
		StatusGraph(NULL)
	{
		Count = GetCount();
	};

	int		GetCount();

	void	InitMem();

	void	BuildStatusGraph(vector<char>&);

	void	InitStatusGraph(vector<char>&);

	void	InitStatusTable();

};


//由状态图的开始节点复制出一个状态图
Status*	CopyStatus(Status *, vector<Status*> &, vector<Edge*> &, map<Status*, Status*>&);

//找e闭包
void	FindClosure(Status*, vector<Status*>&);

//eNFA转化为NFA
void	eNFA2NFA(Status*);

//NFA转化为DFA
void	NFA2DFA(Status*);

#endif // !_DFA_
