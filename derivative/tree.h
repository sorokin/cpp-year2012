#ifndef TREE_H
#define TREE_H

#include <memory>
#include <string>
#include <ostream>

struct expression;
typedef std::unique_ptr<expression> expression_sp;

enum class precedence
{
    addition,
    multiplication,
    unary,
};

struct expression
{
    virtual void print(std::ostream&) const = 0;
    virtual precedence get_precedence() const = 0;
    virtual expression_sp identity() const = 0;
    virtual expression_sp derivative() const = 0;
    virtual expression_sp simplify() const;
    virtual ~expression();
};

std::ostream& operator<<(std::ostream& os, expression const&);

struct negation : expression
{
    negation(expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;

private:
    expression_sp arg;
};

struct addition : expression
{
    addition(expression_sp, expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;

private:
    expression_sp lhs, rhs;
};

struct subtraction : expression
{
    subtraction(expression_sp, expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;

private:
    expression_sp lhs, rhs;
};

struct multiplication : expression
{
    multiplication(expression_sp, expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;

private:
    expression_sp lhs, rhs;
};

struct division : expression
{
    division(expression_sp, expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;

private:
    expression_sp lhs, rhs;
};

struct variable : expression
{
    variable();

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;
};

struct const_ : expression
{
    const_(double);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;

    double get_value() const;

private:
    double value;
};

struct sine : expression
{
    sine(expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;

private:
    expression_sp arg;
};

struct cosine : expression
{
    cosine(expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;

private:
    expression_sp arg;
};

struct tangent : expression
{
    tangent(expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;

private:
    expression_sp arg;
};

struct exponentiation : expression
{
    exponentiation(expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;
    expression_sp simplify() const;

private:
    expression_sp arg;
};

struct logarithm : expression
{
    logarithm(expression_sp);

    void print(std::ostream&) const;
    precedence get_precedence() const;
    expression_sp identity() const;
    expression_sp derivative() const;

private:
    expression_sp arg;
};

#endif // TREE_H
