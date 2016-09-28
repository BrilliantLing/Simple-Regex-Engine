#include"RegularExpression.h"

using	std::cout;
using	std::cerr;
using	std::cin;
using	std::endl;
using	std::ends;

/*int	main()
{
	char Buffer[1000];
	cout << "输入一个正则表达式:" << endl;
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
			throw SyntaxError("出现多余字符", Stream);
		}
		else
		{
			Exp->Visit();
			delete Exp;
		}
	}
	catch (SyntaxError e)
	{
		cerr << "发生错误" << endl;
		e.Show();
	}
	system("PAUSE");
	return 0;
}*/