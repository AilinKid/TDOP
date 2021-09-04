#include "stdio.h"
char * s = "1+2*3*4+5";
int expression(int rbp = 0);

class TokenBase
{
public:
	virtual int led(int left) = 0;

	virtual int lbp()
	{
		if (_lbp == -999)
		{
			printf("error get _lbp");
		}
		return _lbp;
	}

	virtual int nud()
	{
		if (_value == -999)
		{
			printf("error get _nud");
		}
		return _value;
	}

	virtual int value()
	{
		if (_value == -999)
		{
			printf("error get _value");
		}
		return _value;
	}

public:
	int _lbp{ -999 };
	int _value{ -999 };
};

TokenBase * g_token = nullptr;

class literal_token : public TokenBase
{
public:
	literal_token(int x)
	{
		_value = x;
	}

	int led(int left)
	{
		printf("literal_token has no led");
		return 0;
	}
};

class operator_add_token : public TokenBase
{
public:
	operator_add_token()
	{
		_lbp = 10;
	}

	 int led(int left)
	{
		 int right = expression(10);    //相当于是构建了一个 prior 10 的平台,上面只允许 > 10 的操作, 当然可以包括其他表达式
		 return left + right;
	}
};

class operator_mul_token : public TokenBase
{
public:
	operator_mul_token()
	{
		_lbp = 20;
	}

	int led(int left)
	{
		int right = expression(20);
		return left * right;
	}
};


TokenBase * tokenize();

int expression(int rbp)
{
	TokenBase *t = g_token;                 // number
	if (!g_token)
	{
		printf("error: no number???");
		return -1;
	}

	g_token = tokenize();                   // operator
	if (!g_token)
	{
		printf("program is finish \n");
		return t->nud();
	}


	int left = t->nud();
	while (g_token && rbp < g_token->lbp())
	{
		t = g_token;
		g_token = tokenize();
		if (!g_token)
			break;
		left = t->led(left);

	}

	return left;
}

bool isNum(char c)
{
	return  (c >= '0' && c <= '9');
}


// 自己构建 token 解析流, 也可以使用正则解析符号和数字, 这里是手动解析的.
TokenBase * tokenize()
{
	TokenBase *token;
	static char *p = s;

	if (isNum(*p))
	{
		token = new literal_token(*p - '0');
	}
	else if (*p == '+')
	{
		token = new operator_add_token();
	}
	else if (*p == '*')
	{
		token = new operator_mul_token();
	}
	else
	{
		return nullptr;
	}
	p++;
	return token;
}

int main(int argc, char* argv[])
{
    //char * s = "1+2*3*4+5";
	g_token = tokenize();
	printf("the result is %d\n", expression( ));
	return 0;
}

// 分析:
// 该方法类似算法模拟题,波兰式表达式求解,用的是同样的模拟思路.
// 1 +                                   这里看到 + 符号, 其拥有比常数更好的优先级
// 1 + expr(                             由于不知道后面的可能会遇到更高等级的表达式,所以加法要等等
// 1 + expr(2 * expr(                    同样, 当看到乘法时, 后面可能还有更高级的符号需要先算,所以还要等等
// 1 + expr(2 * expr(3 * ?               这里第二次算 expr 的时候, 由于同样是乘法的优先级别, 所以 expr(3 * ?) 没有进入下一次递归,而是返回 3
// 1 + expr(2 * 3) * ?                   注意我 expr 右括号我始终是没有画出来的, 但是这里 2 * 3 是确定的, * 4 不具有更高的优先级, 所以这里就是 2 * 3 先算
// 1 + expr(6 * expr(                    这里算完之后, 继续向后探索, 由于后面不知道又没有更高优先级的表达要先算, 所以还是先用 expr 包一下
// 1 + expr(6 * expr(4 + ?               但是后面更的 + 号, 不具有更高的优先级, 进不了 while, 所以这个表达式直接返回 4
// 1 + expr(6 * 4) + ?                   算出来 24 之后, 还是得继续往后探索
// 1 + expr(24 + ?)                      这里 24 之后的符号是 + 号, 不具有更高优先级, 所以这个 expr 的括号终于可以解开了
// 1 + 24 + ?                            这里直接是 1 + 24 也可以看成是从 main 发出调用的原始 expr 的运算
// 25 + expr(                            最后 25 + 还是等像第二不那样用 expr 包一下继续向后探索, 因为你不知道后面又没有等高级的表达式要先算.
// 25 + expr( 5 over                     因为是最后一个元素, 所以表达式进不了 while, 直接返回字面值 5
// 25 + 5 = 30                           最后表达式完成计算,返回给 main
//
// 该方法效率很好, 虽然也有递归, 但是递归层次有限制, 永远都是局部小范围内的递归, 一旦表达式可以运算时候,就会立即解开一层递归.
// 从时间复杂度上来看, 可以认为是 o(n) 的复杂度, 并没有传统 parser 那种全范围的全层次的递归调用.


// 区别传统的递归下降的 parser 解析方法, TDOP 使用了是基于运算符优先级的 token 解析方式.
// 该 demo 实现有 python 版本,但是我觉得 C++ 方便理解和调试一点. 详情请 Refer:
// <<Top-Down operator precedence parsing>> https://eli.thegreenplace.net/2010/01/02/top-down-operator-precedence-parsing
// 原论文地址: https://tdop.github.io/