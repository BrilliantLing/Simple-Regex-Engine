#include"RegularExpression.h"

using	std::cout;
using	std::cerr;
using	std::cin;
using	std::endl;
using	std::ends;

/*int	main()
{
	char Buffer[1000];
	cout << "����һ��������ʽ:" << endl;
	gets(Buffer);
	try
	{
		char	*Stream = Buffer;
		RegExpPtr	Exp =
			GetExpr(Stream);
		while (*Stream == ' ')
			Stream++;
		if (*Stream)
		{
			delete	Exp;
			throw SyntaxError("���ֶ����ַ�", Stream);
		}
		else
		{
			Exp->Visit();
			delete Exp;
		}
	}
	catch (SyntaxError e)
	{
		cerr << "��������" << endl;
		e.Show();
	}
	system("PAUSE");
	return 0;
}*/