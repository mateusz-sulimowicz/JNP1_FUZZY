#ifndef FUZZY_H
#define FUZZY_H

#include <ostream>
#include<set>
#include <compare>
#include <cmath>

typedef double real_t;

using std::move;

class TriFuzzyNum {
public:

    constexpr TriFuzzyNum(double r1, double r2, double r3) {
        real_t r_min = std::min({r1, r2, r2});
        real_t r_max = std::max({r1, r2, r3});
        real_t r_mid = r1 + r2 + r3 - r_min - r_max;
        lower = r_min;
        modal = r_mid;
        upper = r_max;
    }

    constexpr TriFuzzyNum(const TriFuzzyNum &that) = default;

    constexpr TriFuzzyNum(TriFuzzyNum &&that) noexcept:
            lower(move(that.lower)),
            modal(move(that.modal)),
            upper(move(that.upper)) {}

    [[nodiscard]] constexpr real_t lower_value() const { return lower; }

    [[nodiscard]] constexpr real_t upper_value() const { return upper; }

    [[nodiscard]] constexpr real_t modal_value() const { return modal; }

    constexpr TriFuzzyNum &operator=(const TriFuzzyNum &that) = default;

    constexpr TriFuzzyNum &operator=(TriFuzzyNum &&that) noexcept {
        if (this != &that) {
            lower = that.lower;
            modal = move(that.modal);
            upper = that.upper;
        }
        return *this;
    }

    constexpr TriFuzzyNum &operator+=(const TriFuzzyNum &that) {
        return *this = *this + that;
    }

    constexpr TriFuzzyNum &operator-=(const TriFuzzyNum &that) {
        return *this = *this - that;
    }

    constexpr TriFuzzyNum &operator*=(const TriFuzzyNum &that) {
        return *this = *this * that;
    }

private:
    real_t lower;
    real_t modal;
    real_t upper;

    friend constexpr std::ostream &
    operator<<(std::ostream &os, const TriFuzzyNum &that) {
        os << "("
           << that.lower
           << ", "
           << that.modal
           << ", "
           << that.upper
           << ")";
        return os;
    }

    friend constexpr TriFuzzyNum
    operator+(const TriFuzzyNum &a, const TriFuzzyNum &b) {
        return {a.lower + b.lower,
                a.modal + b.modal,
                a.upper + b.upper};
    }

    friend constexpr TriFuzzyNum
    operator-(const TriFuzzyNum &a, const TriFuzzyNum &b) {
        return {a.lower - b.upper,
                a.modal - b.modal,
                a.upper - b.lower};
    }

    friend constexpr TriFuzzyNum
    operator*(const TriFuzzyNum &a, const TriFuzzyNum &b) {
        return {a.lower * b.lower,
                a.modal * b.modal,
                a.upper * b.upper};
    }

    friend constexpr bool
    operator==(const TriFuzzyNum &f1, const TriFuzzyNum &f2) {
        return f1.lower == f2.lower
               && f1.modal == f2.modal
               && f1.upper == f2.upper;
    }

    friend constexpr bool
    operator!=(const TriFuzzyNum &f1, const TriFuzzyNum &f2) {
        return f1.lower != f2.lower
               || f1.modal != f2.modal
               || f1.upper != f2.upper;
    }

    using fuzzy_rank = std::tuple<real_t, real_t, real_t>;

    [[nodiscard]] constexpr fuzzy_rank rank() const {
        real_t l = this->lower_value();
        real_t m = this->modal_value();
        real_t u = this->upper_value();

        real_t z = (u - l) + sqrt(1 + (u - m) * (u - m)) +
                   sqrt(1 + (m - l) * (m - l));
        real_t y = (u - l) / z;
        real_t x = ((u - l) * m + sqrt(1 + (u - m) * (u - m)) * l +
                    sqrt(1 + (m - l) * (m - l)) * u) / z;

        return {x, y, z};
    }

    friend constexpr std::strong_ordering
    operator<=>(const TriFuzzyNum &a, const TriFuzzyNum &b) {
        fuzzy_rank rank_a = a.rank();
        fuzzy_rank rank_b = b.rank();

        if (rank_a < rank_b) {
            return std::strong_ordering::less;
        } else if (rank_a > rank_b) {
            return std::strong_ordering::greater;
        } else {
            return std::strong_ordering::equal;
        }
    }

    friend class TriFuzzyNumSet;

};

consteval TriFuzzyNum crisp_number(real_t v) {
    return {v, v, v};
}

constinit static TriFuzzyNum crisp_zero = crisp_number(0);

class TriFuzzyNumSet {
public:

    TriFuzzyNumSet() = default;

    TriFuzzyNumSet(std::initializer_list<TriFuzzyNum> nums) :
            sum_lower(0),
            sum_modal(0),
            sum_upper(0) {
        for (auto &num: nums) {
            insert(num);
        }
    }

    TriFuzzyNumSet(const TriFuzzyNumSet &that) = default;

    TriFuzzyNumSet(TriFuzzyNumSet &&that) noexcept:
            num_set(move(that.num_set)),
            sum_lower(move(that.sum_lower)),
            sum_modal(move(that.sum_modal)),
            sum_upper(move(that.sum_upper)) {}

    TriFuzzyNumSet &operator=(const TriFuzzyNumSet &that) = default;

    TriFuzzyNumSet &operator=(TriFuzzyNumSet &&that) noexcept {
        if (this != &that) {
            this->sum_lower = that.sum_lower;
            this->sum_modal = that.sum_modal;
            this->sum_upper = that.sum_upper;
            this->num_set = move(that.num_set);
        }
        return *this;
    }

    void insert(const TriFuzzyNum &num) {
        sum_lower += num.lower;
        sum_modal += num.modal;
        sum_upper += num.upper;
        num_set.insert(num);
    }

    void insert(TriFuzzyNum &&num) {
        sum_lower += num.lower;
        sum_modal += num.modal;
        sum_upper += num.upper;
        num_set.insert(move(num));
    }

    void remove(const TriFuzzyNum &num) {
        auto itr = num_set.find(num);
        if (itr != num_set.end()) {
            sum_lower -= itr->lower;
            sum_modal -= itr->modal;
            sum_upper -= itr->upper;
            num_set.erase(itr);
        }
    }

    TriFuzzyNum arithmetic_mean() {
        if (num_set.empty()) {
            throw std::length_error(
                    "TriFuzzyNumSet::arithmetic_mean - the set is empty.");
        } else {
            auto nums_amount = (double) num_set.size();
            return {sum_lower / nums_amount,
                    sum_modal / nums_amount,
                    sum_upper / nums_amount};
        }
    }

private:
    std::multiset<TriFuzzyNum> num_set;
    real_t sum_lower{};
    real_t sum_modal{};
    real_t sum_upper{};
};


#endif //FUZZY_H
