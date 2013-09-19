#include "tree.h"

#include "make_unique.h"

namespace
{
    void print_lhs(std::ostream& os, expression const& e, precedence parent_precedence)
    {
        if (e.get_precedence() < parent_precedence)
            os << "(" << e << ")";
        else
            os << e;
    }

    void print_rhs(std::ostream& os, expression const& e, precedence parent_precedence)
    {
        if (e.get_precedence() <= parent_precedence)
            os << "(" << e << ")";
        else
            os << e;
    }

    bool is_zero(expression const& e)
    {
        if (const_ const* c = dynamic_cast<const_ const*>(&e))
            return c->get_value() == 0.;

        return false;
    }

    bool is_one(expression const& e)
    {
        if (const_ const* c = dynamic_cast<const_ const*>(&e))
            return c->get_value() == 1.;

        return false;
    }
}

expression::~expression()
{}

expression_sp expression::simplify() const
{
    return identity();
}

std::ostream& operator<<(std::ostream& os, expression const& e)
{
    e.print(os);
    return os;
}

negation::negation(expression_sp arg)
    : arg(std::move(arg))
{}

void negation::print(std::ostream& os) const
{
    os << "-";
    print_lhs(os, *arg, get_precedence());
}

precedence negation::get_precedence() const
{
    return precedence::unary;
}

expression_sp negation::identity() const
{
    return make_unique<negation>(arg->identity());
}

expression_sp negation::derivative() const
{
    return make_unique<negation>(arg->derivative());
}

expression_sp negation::simplify() const
{
    expression_sp s_arg = arg->simplify();
    if (is_zero(*s_arg))
        return make_unique<const_>(0.);

    return make_unique<negation>(std::move(s_arg));
}

addition::addition(expression_sp lhs, expression_sp rhs)
    : lhs(std::move(lhs))
    , rhs(std::move(rhs))
{}

void addition::print(std::ostream& os) const
{
    print_lhs(os, *lhs, get_precedence());
    os << " + ";
    print_rhs(os, *rhs, get_precedence());
}

precedence addition::get_precedence() const
{
    return precedence::addition;
}

expression_sp addition::identity() const
{
    return make_unique<addition>(lhs->identity(), rhs->identity());
}

expression_sp addition::derivative() const
{
    return make_unique<addition>(lhs->derivative(), rhs->derivative());
}

expression_sp addition::simplify() const
{
    expression_sp s_lhs = lhs->simplify();
    expression_sp s_rhs = rhs->simplify();

    if (is_zero(*s_lhs))
        return std::move(s_rhs);

    if (is_zero(*s_rhs))
        return std::move(s_lhs);

    return make_unique<addition>(std::move(s_lhs), std::move(s_rhs));
}

subtraction::subtraction(expression_sp lhs, expression_sp rhs)
    : lhs(std::move(lhs))
    , rhs(std::move(rhs))
{}

void subtraction::print(std::ostream& os) const
{
    print_lhs(os, *lhs, get_precedence());
    os << " - ";
    print_rhs(os, *rhs, get_precedence());
}

precedence subtraction::get_precedence() const
{
    return precedence::addition;
}

expression_sp subtraction::identity() const
{
    return make_unique<subtraction>(lhs->identity(), rhs->identity());
}

expression_sp subtraction::derivative() const
{
    return make_unique<subtraction>(lhs->derivative(), rhs->derivative());
}

expression_sp subtraction::simplify() const
{
    expression_sp s_lhs = lhs->simplify();
    expression_sp s_rhs = rhs->simplify();

    if (is_zero(*s_lhs))
        return make_unique<negation>(std::move(s_rhs))->simplify();

    if (is_zero(*s_rhs))
        return std::move(s_lhs);

    return make_unique<subtraction>(std::move(s_lhs), std::move(s_rhs));
}

multiplication::multiplication(expression_sp lhs, expression_sp rhs)
    : lhs(std::move(lhs))
    , rhs(std::move(rhs))
{}

void multiplication::print(std::ostream& os) const
{
    print_lhs(os, *lhs, get_precedence());
    os << " * ";
    print_rhs(os, *rhs, get_precedence());
}

precedence multiplication::get_precedence() const
{
    return precedence::multiplication;
}

expression_sp multiplication::identity() const
{
    return make_unique<multiplication>(lhs->identity(), rhs->identity());
}

expression_sp multiplication::derivative() const
{
    return make_unique<addition>(make_unique<multiplication>(lhs->derivative(), rhs->identity()),
                                 make_unique<multiplication>(lhs->identity(), rhs->derivative()));
}

expression_sp multiplication::simplify() const
{
    expression_sp s_lhs = lhs->simplify();
    expression_sp s_rhs = rhs->simplify();

    if (is_zero(*s_lhs))
        return make_unique<const_>(0.);
    else if (is_one(*s_lhs))
        return std::move(s_rhs);

    if (is_zero(*s_rhs))
        return make_unique<const_>(0.);
    else if (is_one(*s_rhs))
        return std::move(s_lhs);

    return make_unique<multiplication>(std::move(s_lhs), std::move(s_rhs));
}

division::division(expression_sp lhs, expression_sp rhs)
    : lhs(std::move(lhs))
    , rhs(std::move(rhs))
{}

void division::print(std::ostream& os) const
{
    print_lhs(os, *lhs, get_precedence());
    os << " / ";
    print_rhs(os, *rhs, get_precedence());
}

precedence division::get_precedence() const
{
    return precedence::multiplication;
}

expression_sp division::identity() const
{
    return make_unique<division>(lhs->identity(), rhs->identity());
}

expression_sp division::derivative() const
{
    return make_unique<division>(make_unique<subtraction>(make_unique<multiplication>(lhs->derivative(), rhs->identity()),
                                                          make_unique<multiplication>(rhs->derivative(), lhs->identity())),
                                 make_unique<multiplication>(rhs->identity(), rhs->identity()));
}

expression_sp division::simplify() const
{
    expression_sp s_lhs = lhs->simplify();
    expression_sp s_rhs = rhs->simplify();

    if (is_zero(*s_lhs))
        return make_unique<const_>(0.);

    if (is_one(*s_rhs))
        return std::move(s_lhs);

    return make_unique<division>(std::move(s_lhs), std::move(s_rhs));
}

variable::variable()
{}

void variable::print(std::ostream & os) const
{
    os << "x";
}

precedence variable::get_precedence() const
{
    return precedence::unary;
}

expression_sp variable::identity() const
{
    return make_unique<variable>();
}

expression_sp variable::derivative() const
{
    return make_unique<const_>(1.);
}

expression_sp variable::simplify() const
{
    return identity();
}

const_::const_(double value)
    : value(value)
{}

void const_::print(std::ostream& os) const
{
    os << value;
}

precedence const_::get_precedence() const
{
    return precedence::unary;
}

expression_sp const_::identity() const
{
    return make_unique<const_>(value);
}

expression_sp const_::derivative() const
{
    return make_unique<const_>(0.);
}

expression_sp const_::simplify() const
{
    return identity();
}

double const_::get_value() const
{
    return value;
}

sine::sine(expression_sp arg)
    : arg(std::move(arg))
{}

void sine::print(std::ostream & os) const
{
    os << "sin ";
    print_lhs(os, *arg, get_precedence());
}

precedence sine::get_precedence() const
{
    return precedence::unary;
}

expression_sp sine::identity() const
{
    return make_unique<sine>(arg->identity());
}

expression_sp sine::derivative() const
{
    return make_unique<multiplication>(make_unique<cosine>(arg->identity()),
                                       arg->derivative());
}

cosine::cosine(expression_sp arg)
    : arg(std::move(arg))
{}

void cosine::print(std::ostream & os) const
{
    os << "cos ";
    print_lhs(os, *arg, get_precedence());
}

precedence cosine::get_precedence() const
{
    return precedence::unary;
}

expression_sp cosine::identity() const
{
    return make_unique<cosine>(arg->identity());
}

expression_sp cosine::derivative() const
{
    return make_unique<multiplication>(make_unique<negation>(make_unique<sine>(arg->identity())),
                                       arg->derivative());
}

tangent::tangent(expression_sp arg)
    : arg(std::move(arg))
{}

void tangent::print(std::ostream & os) const
{
    os << "tan ";
    print_lhs(os, *arg, get_precedence());
}

precedence tangent::get_precedence() const
{
    return precedence::unary;
}

expression_sp tangent::identity() const
{
    return make_unique<tangent>(arg->identity());
}

expression_sp tangent::derivative() const
{
    return make_unique<multiplication>(make_unique<division>(make_unique<const_>(1.),
                                                             make_unique<multiplication>(arg->identity(), arg->identity())),
                                       arg->derivative());
}

exponentiation::exponentiation(expression_sp arg)
    : arg(std::move(arg))
{}

precedence exponentiation::get_precedence() const
{
    return precedence::unary;
}

void exponentiation::print(std::ostream & os) const
{
    os << "exp ";
    print_lhs(os, *arg, get_precedence());
}

expression_sp exponentiation::identity() const
{
    return make_unique<exponentiation>(arg->identity());
}

expression_sp exponentiation::derivative() const
{
    return make_unique<multiplication>(identity(),
                                       arg->derivative());
}

expression_sp exponentiation::simplify() const
{
    expression_sp s_arg = arg->simplify();
    if (is_zero(*s_arg))
        return make_unique<const_>(1.);

    return make_unique<exponentiation>(std::move(s_arg));
}

logarithm::logarithm(expression_sp arg)
    : arg(std::move(arg))
{}

void logarithm::print(std::ostream & os) const
{
    os << "log ";
    print_lhs(os, *arg, get_precedence());
}

precedence logarithm::get_precedence() const
{
    return precedence::unary;
}

expression_sp logarithm::identity() const
{
    return make_unique<logarithm>(arg->identity());
}

expression_sp logarithm::derivative() const
{
    return make_unique<multiplication>(make_unique<division>(make_unique<const_>(1.), arg->identity()),
                                       arg->derivative());
}
