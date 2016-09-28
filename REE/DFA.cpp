#include"DFA.h"
#include"RegularExpression.h"

Status*	CopyStatusOutEdges
(Status *status, vector<Status*> &svisited, 
vector<Edge*> &evisited, map<Status*, Status*> &reflection)
{
	Status*		Copy = new Status();
	svisited.push_back(status);
	if (!status->OutEdges.empty())
	{
		for (auto iter = status->OutEdges.begin();
			iter != status->OutEdges.end();
			iter++)
		{
			if (std::find(svisited.begin(), svisited.end(), (*iter)->End) == svisited.end())
			{
				Copy->Attribute = status->Attribute;
				//svisited.push_back((*iter)->End);
				Edge*	Temp =
					new Edge(Copy, CopyStatus((*iter)->End,svisited,evisited,reflection));
				Temp->MatchContent = (*iter)->MatchContent;
				Copy->OutEdges.push_back(Temp);
			}
			else
				delete Copy;
			if (std::find(evisited.begin(), evisited.end(), *iter) == evisited.end())
			{
				evisited.push_back(*iter);
			}
			else
			{
				Edge*	Temp =
					new Edge(Copy, reflection[(*iter)->End]);
				Copy->OutEdges.push_back(Temp);
			}
		}
	}
	reflection[status] = Copy;
	return Copy;
}

void	CopyStatusInEdges
(Status *status, vector<Status*> &svisited,
vector<Edge*> &evisited, map<Status*, Status*> &reflection)
{
	for (auto iter = svisited.begin();
		iter != svisited.end();
		iter++)
	{
		if (!(*iter)->InEdges.empty())
		{
			for (auto eiter = (*iter)->InEdges.begin();
				eiter != (*iter)->InEdges.end();
				eiter++)
			{
				Edge*	Temp =
					new Edge(reflection[(*eiter)->Start], reflection[(*eiter)->End]);
				Temp->MatchContent = (*eiter)->MatchContent;
				reflection[*iter]->InEdges.push_back(Temp);
			}
		}
	}
}

Status*	CopyStatus
(Status *status, vector<Status*> &svisited, 
vector<Edge*> &evisited, map<Status*, Status*> &reflection)
{
	Status	*Copy = 
		CopyStatusOutEdges(status, svisited, evisited, reflection);
	CopyStatusInEdges(status, svisited, evisited, reflection);
	return	Copy;
}

void	GetEndStatus(Status* Sstatus, Status* Estatus, vector<Status*> &svec)
{
	if (Sstatus->Attribute==STATUS_END)
	{
		Estatus=Sstatus;
	}
	else
	{
		svec.push_back(Sstatus);
		for (auto iter = Sstatus->OutEdges.begin();
			iter != Sstatus->OutEdges.end();
			iter++)
		{
			if (std::find(svec.begin(), svec.end(), (*iter)->End)==svec.end())
				GetEndStatus((*iter)->End, Estatus, svec);
		}
	}
}

std::pair<Status*, Status*>*
RegExpCharSet::BuildFA()
{
	Status* StartStatus = new Status(STATUS_START);
	Status*	EndStatus = new Status(STATUS_END);

	Edge*	SEEdge = new	Edge(TransCharList, StartStatus, EndStatus);
	StartStatus->OutEdges.push_back(SEEdge);
	EndStatus->InEdges.push_back(SEEdge);
	std::pair<Status*, Status*>	*SEPair =
		new std::pair<Status*, Status*>({ StartStatus, EndStatus });
	return	SEPair;
}

std::pair<Status*, Status*>*
RegExpLoop::BuildFA()
{
	vector<Status*>			svec;
	vector<Edge*>			evec;
	map<Status*, Status*>	Ref;
	Status*		sStatus = NULL;
	std::pair<Status*, Status*>	*ChildExp =
		Expression->BuildFA();
	//ChildExp->first->SetCommon();
	//ChildExp->second->SetCommon();
	if (Infinite&&MinTimes == 1)
	{
		Status*	ChildStart = CopyStatus(ChildExp->first, svec, evec, Ref);
		svec.clear();
		Status*	ChildEnd;
		GetEndStatus(ChildStart, ChildEnd, svec);
		ChildStart->SetCommon();
		ChildEnd->SetCommon();

		Edge*	eFirst = new Edge(ChildExp->second,ChildStart);
		ChildExp->second->OutEdges.push_back(eFirst);
		ChildStart->InEdges.push_back(eFirst);

		Edge*	eSecond = new Edge(ChildEnd, ChildExp->second);
		ChildExp->second->InEdges.push_back(eSecond);
		ChildEnd->OutEdges.push_back(eSecond);

		return ChildExp;

	}
	else if (Infinite&&MinTimes == 0)
	{
		sStatus = new Status(STATUS_STEND);

		Edge*	eFirst = new Edge(sStatus, ChildExp->first);
		ChildExp->first->InEdges.push_back(eFirst);
		sStatus->OutEdges.push_back(eFirst);

		Edge*	eSecond = new Edge(ChildExp->second, sStatus);
		ChildExp->second->OutEdges.push_back(eSecond);
		sStatus->OutEdges.push_back(eSecond);

		std::pair<Status*, Status*>	*SEPair =
			new std::pair<Status*, Status*>({ sStatus, sStatus });
		return SEPair;

	}
	else
		return		ChildExp;
}

std::pair<Status*,Status*>*
RegExpSection::BuildFA()
{
	Status*									sStart = NULL;
	Status*									sEnd = NULL;
	std::pair<Status*, Status*>*			ChildExp = NULL;
	vector<std::pair<Status*, Status*>*>	pStack;
	for (auto iter = Next.begin();
		iter != Next.end();
		iter++)
	{
		ChildExp = (*iter)->BuildFA();
		pStack.push_back(ChildExp);
	}
	if (Next.size == 1)
		return	Next.back()->BuildFA();
	if (Type == Sequence)
	{
		for (auto iter = pStack.begin();
			iter != pStack.end() - 2;
			iter++)
		{
			Edge*	Temp = new Edge();
			std::pair<Status*, Status*>*	pCurr= (*iter);
			std::pair<Status*, Status*>*	pNext = (*iter + 1);
			pCurr->first->SetCommon();
			pCurr->second->SetCommon();
			pNext->first->SetCommon();
			pNext->second->SetCommon();
			Temp->Start = pCurr->second;
			Temp->End = pNext->first;
			pCurr->second->OutEdges.push_back(Temp);
			pNext->first->InEdges.push_back(Temp);
		}
		pStack.front()->first->Attribute = STATUS_START;
		pStack.back()->second->Attribute = STATUS_END;
		std::pair<Status*, Status*>		*pTemp = 
			new std::pair<Status*, Status*>({ (pStack.front()->first), (pStack.back()->second) });
		return	pTemp;
	}
	else
	{
		sStart = new Status(STATUS_START);
		sEnd = new Status(STATUS_END);
		for (auto iter = pStack.begin();
			iter != pStack.end();
			iter++)
		{
			Edge*	eFirst = new Edge();
			eFirst->Start = sStart;
			eFirst->End = (*iter)->first;
			sStart->OutEdges.push_back(eFirst);
			(*iter)->first->InEdges.push_back(eFirst);

			Edge*	eSecond = new Edge();
			eSecond->Start = (*iter)->second;
			eSecond->End = sEnd;
			sEnd->InEdges.push_back(eSecond);
			(*iter)->second->OutEdges.push_back(eSecond);

			(*iter)->first->SetCommon();
			(*iter)->second->SetCommon();
		}
		std::pair<Status*, Status*>*		pTemp =
			new std::pair<Status*, Status*>({ sStart, sEnd });
		return	pTemp;
	}
}

void
FindValidStatus(Status* sStart,vector<Status*> &ValidVec,
vector<Status*> &InvalidVec, vector<Status*> &VisitVec)
{
	VisitVec.push_back(sStart);
	for (auto iter = sStart->InEdges.begin();
		iter != sStart->InEdges.end();
		iter++)
	{
		if (sStart->Attribute == STATUS_START ||
			sStart->Attribute == STATUS_STEND)
		{
			ValidVec.push_back(sStart);
			continue;
		}
		if (!(*iter)->MatchContent.empty())
		{
			ValidVec.push_back(sStart);
			break;
		}
		else
			InvalidVec.push_back(sStart);
	}
	if (!sStart->OutEdges.empty())
	{
		for (auto iter = sStart->OutEdges.begin();
			iter != sStart->OutEdges.end();
			iter++)
		{
			if (std::find(VisitVec.begin(), VisitVec.end(), (*iter)->End) == VisitVec.end())
				FindValidStatus((*iter)->End, ValidVec, InvalidVec, VisitVec);
		}
	}
}

void
FindeEdge(Status *sStart, vector<Status*> &VisitVec, vector<Edge*> &Vec)
{
	VisitVec.push_back(sStart);
	if (sStart->OutEdges.empty())
	{
		for (auto eiter = sStart->OutEdges.begin();
			eiter != sStart->OutEdges.end();
			eiter++)
		{
			if (!(*eiter)->MatchContent.empty() &&
				std::find(Vec.begin(), Vec.end(), (*eiter)) == Vec.end())
			{
				Vec.push_back(*eiter);
			}
			if (std::find(VisitVec.begin(), VisitVec.end(), (*eiter)->End) == VisitVec.end())
				FindeEdge((*eiter)->End, VisitVec, Vec);
		}
	}
}

void
FindClosure(Status* sStat, vector<Status*> &sVec)
{
	for (auto iter = sStat->OutEdges.begin();
		iter != sStat->OutEdges.end();
		iter++)
	{
		if ((*iter)->MatchContent.empty()&&
			std::find(sVec.begin(), sVec.end(), (*iter)->End) == sVec.end())	
		{
			sVec.push_back((*iter)->End);
			FindClosure((*iter)->End, sVec);
		}
		
	}
}

void
AddEdges(Status* sStat)
{
	vector<Status*>		vClosure;
	FindClosure(sStat, vClosure);
	for (auto siter = vClosure.begin();
		siter != vClosure.end();
		siter++)
	{
		if ((*siter)->Attribute == STATUS_END ||
			(*siter)->Attribute == STATUS_STEND)
		{
			sStat->Attribute = STATUS_END;
		}
		for (auto eiter = (*siter)->OutEdges.begin();
			eiter != (*siter)->OutEdges.end();
			eiter++)
		{
			if (!(*eiter)->MatchContent.empty())
			{
				Edge*	Temp = new Edge(sStat, (*eiter)->End);
				Temp->MatchContent = (*eiter)->MatchContent;
				sStat->OutEdges.push_back(Temp);
				(*eiter)->End->InEdges.push_back(Temp);
			}
		}
	}
}

void
eNFA2NFA(Status* sStart)
{
	vector<Status*>	ValidVec, InvalidVec, VisitedVec;
	vector<Edge*>	eEdgeVec;
	FindValidStatus(sStart, ValidVec, InvalidVec, VisitedVec);
	VisitedVec.clear();
	FindeEdge(sStart, VisitedVec, eEdgeVec);
	for (auto siter = ValidVec.begin();
		siter != ValidVec.end();
		siter++)
	{
		AddEdges(*siter);
	}

	//delete e edges and invalid status
	for (auto eiter = eEdgeVec.begin();
		eiter != eEdgeVec.end();
		eiter++)
	{
		(*eiter)->Start->OutEdges.remove(*eiter);
		(*eiter)->End->InEdges.remove(*eiter);
		delete (*eiter);
	}
	for (auto siter = InvalidVec.begin();
		siter != InvalidVec.end();
		siter++)
	{
		for (auto eiter = (*siter)->OutEdges.begin();
			eiter != (*siter)->OutEdges.end();
			eiter++)
		{
			if (!(*eiter)->MatchContent.empty())
			{
				(*eiter)->End->InEdges.remove(*eiter);
			}
		}
		delete (*siter);
	}
}

void
DeleteReplicateEdges(Status* s)
{
	//vector<std::pair<char, Edge*>>	InContent, OutContent;
	set<vector<char>>	InEdges, OutEdges;
	for (auto eiter = s->InEdges.begin();
		eiter != s->InEdges.end();
		eiter++)
	{
		std::sort((*eiter)->MatchContent.begin(),(*eiter)->MatchContent.end());
		if (InEdges.find((*eiter)->MatchContent) != InEdges.end())
		{
			InEdges.insert((*eiter)->MatchContent);
		}
		else
		{
			delete (*eiter);
		}
	}
	for (auto eiter = s->OutEdges.begin();
		eiter != s->OutEdges.end();
		eiter++)
	{
		std::sort((*eiter)->MatchContent.begin(), (*eiter)->MatchContent.end());
		if (OutEdges.find((*eiter)->MatchContent) != OutEdges.end())
		{
			OutEdges.insert((*eiter)->MatchContent);
		}
		else
		{
			delete (*eiter);
		}
	}
}

void
NFA2DFA(Status* sStart)
{
	set<Status*>				Visited;
	queue<Status*>				Curr;

	vector<char>				EdgeContent;
	set<Status*>				TempDest;

	multimap<char, Status*>		csMap;

	Curr.push(sStart);
	Visited.insert(sStart);

	while (Curr.empty())
	{
		//扫描字符集合
		for (auto eiter = Curr.front()->OutEdges.begin();
			eiter != Curr.front()->OutEdges.end();
			eiter++)
		{
			if (!(*eiter)->MatchContent.empty())
			{
				for (auto citer = (*eiter)->MatchContent.begin();
					citer != (*eiter)->MatchContent.end();
					citer++)
				{
					EdgeContent.push_back(*citer);
				}
			}
		}

		//找到每个字符对应的边
		for (auto citer = EdgeContent.begin();
			citer != EdgeContent.end();
			citer++)
		{
			for (auto eiter = Curr.front()->OutEdges.begin();
				eiter != Curr.front()->OutEdges.end();
				eiter++)
			{
				if (std::find((*eiter)->MatchContent.begin(), (*eiter)->MatchContent.end(), *citer) !=
					(*eiter)->MatchContent.end())
				{
					TempDest.insert((*eiter)->End);
					csMap.insert({ (*citer), (*eiter)->End });
				}
			}
		}

		//如果一个字符对应了两个及以上状态，则创建新状态，删除对应状态
		for (auto citer = EdgeContent.begin();
			citer != EdgeContent.end();
			citer++)
		{
			if ((csMap.count(*citer) > 1))
			{
				Status	*Temp = new Status();
				for (auto miter = csMap.lower_bound(*citer);
					miter != csMap.upper_bound(*citer);
					miter++)
				{
					for (auto eiter = miter->second->InEdges.begin();
						eiter != miter->second->InEdges.end();
						eiter++)
					{
						(*eiter)->End = Temp;
						Temp->InEdges.push_back(*eiter);
					}
					for (auto eiter = miter->second->OutEdges.begin();
						eiter != miter->second->OutEdges.end();
						eiter++)
					{
						(*eiter)->Start = Temp;
						Temp->OutEdges.push_back(*eiter);
					}
					TempDest.erase(miter->second);
					delete miter->second;
				}
				DeleteReplicateEdges(Temp);
				csMap.erase(csMap.lower_bound(*citer), csMap.upper_bound(*citer));
				csMap.insert({ *citer, Temp });
				TempDest.insert(Temp);
			}
		}
		///

		for (auto siter = TempDest.begin();
			siter != TempDest.end();
			siter++)
		{
			if (Visited.find(*siter) == Visited.end())
			{
				Visited.insert(*siter);
				Curr.push(*siter);
			}
		}

		Curr.pop();
		TempDest.clear();
		csMap.clear();
	}
}

void		
CountStatus(Status* sStart,set<Status*> &Visted)
{
	if (Visted.find(sStart) == Visted.end())
	{
		Visted.insert(sStart);
	}
	else
	{
		return;
	}
	if (sStart->Attribute != STATUS_END)
	{
		for (auto eiter = sStart->OutEdges.begin();
			eiter != sStart->OutEdges.end();
			eiter++)
		{
			CountStatus((*eiter)->End, Visted);
		}
	}
}

int
DFA::GetCount()
{
	set<Status*>		Visited;
	CountStatus(Start, Visited);
	Count = Visited.size();
	return	Visited.size();
}

void
DFA::InitStatusGraph(vector<char>& CharSet)
{
	int		CharNum = CharSet.size();
	StatusGraph = new int*[Count];
	for (int i = 0; i < Count; i++)
	{
		StatusGraph[i] = new int[CharNum];
	}
	for (int i = 0; i < Count; i++)
	{
		for (int j = 0; j < CharNum; j++)
			StatusGraph[i][j] = 0;
	}
	cSet.GetCharClass(CharSet);
}

void
VisitStatusGraph(Status *Curr, vector<Status*> &sVisited, vector<Edge*> &eVisited)
{
	if (std::find(sVisited.begin(), sVisited.end(), Curr) == sVisited.end())
	{
		sVisited.push_back(Curr);
	}
	else
	{
		return;
	}
	if (Curr->Attribute != STATUS_END)
	{
		for (auto eiter = Curr->OutEdges.begin();
			eiter != Curr->OutEdges.end();
			eiter++)
		{
			if (std::find(eVisited.begin(), eVisited.end(), (*eiter)) == eVisited.end())
			{
				eVisited.push_back(*eiter);
			}
			VisitStatusGraph((*eiter)->End, sVisited, eVisited);
		}
	}
}

void
DFA::InitMem()
{
	VisitStatusGraph(Start, StatusMem, EdgeMem);
}

void
DFA::BuildStatusGraph(vector<char>& CharSet)
{
	//cSet.GetCharClass(CharSet);
	for (int i = 0; i < StatusMem.size(); i++)
	{
		for (auto eiter = StatusMem[i]->OutEdges.begin();
			eiter != StatusMem[i]->OutEdges.end();
			eiter++)
		{
			for (auto citer = (*eiter)->MatchContent.begin();
				citer != (*eiter)->MatchContent.end();
				citer++)
			{
				StatusGraph[i][cSet.Set[(*citer)]] =
					std::find(StatusMem.begin(), StatusMem.end(), (*eiter)->End) - StatusMem.begin();
			}
		}
	}
}

void
DFA::InitStatusTable()
{
	StatusTable = new bool[StatusMem.size()];
	for (int i = 0; i < StatusMem.size(); i++)
	{
		if (StatusMem[i]->Attribute == STATUS_END)
		{
			StatusTable[i] = true;
		}
	}
}