#include <iostream>
#include <string>

#include "parser.h"

void test(std::string const& text)
{
    expression_sp a = parse(text);

    std::cout << *a << std::endl;

    expression_sp deriv = a->derivative();
    std::cout << "  : " << *deriv << std::endl;
    std::cout << "  : " << *deriv->simplify() << std::endl;
}

int main()
{
    test("x");
    test("5.");
    test("x + 5");
    test("x + x * 2");
    test("(x)");
    test("(x + 2) * x");
    test("(x + 2) / (x - 1)");
    test("-(x*x)");
    test("sin(x*x)");
    test("cos(x*x)");
    test("tan(x*x)");
    test("exp(x*x)");
    test("log(x*x)");
    test("exp x");
    test("1-------5");
    test("1++++x");

    for (;;)
    {
        try
        {
            std::string s;
            std::getline(std::cin, s);
            if (s.empty())
                break;
            test(s);
        }
        catch (std::exception const& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}

