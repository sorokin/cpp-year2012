#include "parser.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include "make_unique.h"

namespace
{
    bool is_digit_or_dot(char c)
    {
        return isdigit(c) || c == '.';
    }

    struct parser_context
    {
        parser_context(char const* start, char const* end)
            : pos(start)
            , end(end)
        {}

        expression_sp parse_addition()
        {
            expression_sp lhs = parse_multiplication();

            for (;;)
            {
                if (eof())
                    return std::move(lhs);

                char c = peek();
                if (c == '+')
                {
                    advance();
                    lhs = make_unique<addition>(std::move(lhs), parse_multiplication());
                }
                else if (c == '-')
                {
                    advance();
                    lhs = make_unique<subtraction>(std::move(lhs), parse_multiplication());
                }
                else
                    return std::move(lhs);
            }
        }

        expression_sp parse_multiplication()
        {
            expression_sp lhs = parse_unary();

            for (;;)
            {
                if (eof())
                    return std::move(lhs);

                char c = peek();
                if (c == '*')
                {
                    advance();
                    lhs = make_unique<multiplication>(std::move(lhs), parse_unary());
                }
                else if (c == '/')
                {
                    advance();
                    lhs = make_unique<division>(std::move(lhs), parse_unary());
                }
                else
                    return std::move(lhs);
            }
        }

        expression_sp parse_unary()
        {
            error_if_eof();
            char c = peek();
            if (c == '(')
                return parse_parenthesis();
            else if (c == '+')
            {
                advance();
                return parse_unary();
            }
            else if (c == '-')
            {
                advance();
                return make_unique<negation>(parse_unary());
            }
            else if (isalpha(c))
            {
                std::string ident = parse_identifier();
                if (ident == "x")
                    return make_unique<variable>();
                else if (ident == "sin")
                    return make_unique<sine>(parse_unary());
                else if (ident == "cos")
                    return make_unique<cosine>(parse_unary());
                else if (ident == "tan")
                    return make_unique<tangent>(parse_unary());
                else if (ident == "exp")
                    return make_unique<exponentiation>(parse_unary());
                else if (ident == "log")
                    return make_unique<logarithm>(parse_unary());
                else
                    throw std::runtime_error("syntax error");
            }
            else if (is_digit_or_dot(c))
                return make_unique<const_>(boost::lexical_cast<double>(parse_literal()));
            else
                throw std::runtime_error("syntax error");
        }

        expression_sp parse_parenthesis()
        {
            assert(peek() == '(');
            advance();

            expression_sp inner = parse_addition();

            if (eof() || peek() != ')')
                throw std::runtime_error("expected ')'");
            advance();

            return std::move(inner);
        }

        std::string parse_identifier()
        {
            assert(isalpha(peek()));

            skip_ws();

            std::string res;
            while (pos != end && isalpha(*pos))
            {
                res += *pos;
                advance();
            }

            return res;
        }

        std::string parse_literal()
        {
            assert(is_digit_or_dot(peek()));

            skip_ws();

            std::string res;
            while (pos != end && is_digit_or_dot(*pos))
            {
                res += *pos;
                advance();
            }

            return res;
        }

        void error_if_eof()
        {
            if (eof())
                throw std::runtime_error("unexcepted eof");
        }

        bool eof()
        {
            skip_ws();
            return pos == end;
        }

        char peek()
        {
            assert(!eof());
            skip_ws();
            return *pos;
        }

        void advance()
        {
            assert(!eof());
            skip_ws();
            ++pos;
        }

        void skip_ws()
        {
            while (pos != end && *pos == ' ')
                ++pos;
        }

    private:
        char const* pos;
        char const* end;
    };
}

expression_sp parse(std::string const& s)
{
    parser_context ctx(s.data(), s.data() + s.size());

    expression_sp r = ctx.parse_addition();
    if (!ctx.eof())
        throw std::runtime_error("expected eof");

    return std::move(r);
}

