#include <iostream>
#include "fuzzy.h"
#include <cmath>
#include <tuple>
#include <compare>

namespace {

    using std::move;
    using std::tuple;
    using rank = tuple<real_t, real_t, real_t>;
    const int X = 0;
    const int Y = 1;
    const int Z = 2;

    static tuple<real_t, real_t, real_t> calc_rank(const TriFuzzyNum &a) {
        real_t l = a.lower_value();
        real_t m = a.modal_value();
        real_t u = a.upper_value();

        real_t z = (u - l) + sqrt(1 + (u - m) * (u - m)) + sqrt(1 + (m - l) * (m - l));
        real_t y = (u - l) / z;
        real_t x = ((u - l) * m + sqrt(1 + (u - m) * (u - m)) * l + sqrt(1 + (m - l) * (m - l)) * u) / z;

        return std::tie(x, y, z);
    }

}


TriFuzzyNum::TriFuzzyNum(const TriFuzzyNum &that) : lower(that.lower),
                                                    modal(that.modal),
                                                    upper(that.upper) {}

TriFuzzyNum::TriFuzzyNum(TriFuzzyNum &&that) noexcept: lower(move(that.lower)),
                                                       modal(move(that.modal)),
                                                       upper(move(
                                                               that.upper)) {}

real_t TriFuzzyNum::lower_value() const {
    return this->lower;
}

real_t TriFuzzyNum::modal_value() const {
    return this->modal;
}

real_t TriFuzzyNum::upper_value() const {
    return this->upper;
}

TriFuzzyNum &TriFuzzyNum::operator=(const TriFuzzyNum &that) {
    if (this != &that) {
        lower = that.lower;
        modal = that.modal;
        upper = that.upper;
    }
    return *this;
}

TriFuzzyNum &TriFuzzyNum::operator=(TriFuzzyNum &&that) {
    if (this != &that) {
        lower = move(that.lower);
        modal = move(that.modal);
        upper = move(that.upper);
    }
    return *this;
}

TriFuzzyNum &TriFuzzyNum::operator+=(const TriFuzzyNum &that) {
    lower += that.lower;
    modal += that.modal;
    upper += that.upper;
    return *this;
}

TriFuzzyNum &TriFuzzyNum::operator-=(const TriFuzzyNum &that) {
    lower -= that.lower;
    modal -= that.modal;
    upper -= that.upper;
    return *this;
}

TriFuzzyNum &TriFuzzyNum::operator*=(const TriFuzzyNum &that) {
    lower *= that.lower;
    modal *= that.modal;
    upper *= that.upper;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const TriFuzzyNum &that) {
    os << "("
       << that.lower
       << ", "
       << that.modal
       << ", "
       << that.upper
       << ")";
    return os;
}

const TriFuzzyNum operator+(const TriFuzzyNum &a, const TriFuzzyNum &b) {
    return TriFuzzyNum(a) += b;
}

const TriFuzzyNum operator-(const TriFuzzyNum &a, const TriFuzzyNum &b) {
    return TriFuzzyNum(a) -= b;
}

const TriFuzzyNum operator*(const TriFuzzyNum &a, const TriFuzzyNum &b) {
    return TriFuzzyNum(a) *= b;
}

bool operator==(const TriFuzzyNum &f1, const TriFuzzyNum &f2) {
    return f1.lower == f2.lower
           && f1.modal == f2.modal
           && f1.upper == f2.upper;
}

bool operator!=(const TriFuzzyNum &f1, const TriFuzzyNum &f2) {
    return f1.lower != f2.lower
           || f1.modal != f2.modal
           || f1.upper != f2.upper;
}

auto operator<=>(const TriFuzzyNum &f1, const TriFuzzyNum &f2) {
    rank f1_rank = calc_rank(f1);
    rank f2_rank = calc_rank(f2);

    if (f1_rank < f2_rank) {
        return std::weak_ordering::less;
    } else if ( f1_rank > f2_rank) {
        return std::weak_ordering::greater;
    } else {
        return std::weak_ordering::equivalent;
    }

    // TODO: ładniej
}

