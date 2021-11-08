#ifndef FUZZY_H
#define FUZZY_H

#include<set>

using real_t = double;
using std::multiset;

class TriFuzzyNum {
public:
    TriFuzzyNum(real_t r1, real_t r2, real_t r3);

    TriFuzzyNum(const TriFuzzyNum &that);

    TriFuzzyNum(TriFuzzyNum &&that) noexcept;

    ~TriFuzzyNum();

    real_t lower_value() const;

    real_t upper_value() const;

    real_t modal_value() const;

    TriFuzzyNum &operator=(const TriFuzzyNum &that);

    TriFuzzyNum &operator=(TriFuzzyNum &&that);

    TriFuzzyNum &operator+=(const TriFuzzyNum &that);

    TriFuzzyNum &operator-=(const TriFuzzyNum &that);

    TriFuzzyNum &operator*=(const TriFuzzyNum &that);

private:
    real_t lower;
    real_t modal;
    real_t upper;

    friend std::ostream &operator<<(std::ostream &os, const TriFuzzyNum &that);

    friend const TriFuzzyNum operator+(const TriFuzzyNum &that);

    friend const TriFuzzyNum operator+(const TriFuzzyNum &that);

    friend const TriFuzzyNum operator*(const TriFuzzyNum &that);

    friend bool operator==(const TriFuzzyNum &f1, const TriFuzzyNum &f2);

    friend bool operator!=(const TriFuzzyNum &f1, const TriFuzzyNum &f2);

    // TODO: porównania liczb rozmytych
    friend auto operator<=>(const TriFuzzyNum &f1, const TriFuzzyNum &f2);
};

class TriFuzzyNumSet {
public:
    TriFuzzyNumSet(std::initializer_list<TriFuzzyNum> nums) {};

    TriFuzzyNumSet(const TriFuzzyNumSet& that);

    TriFuzzyNumSet(TriFuzzyNumSet&& that);

    ~TriFuzzyNumSet();

    TriFuzzyNumSet &operator=(const TriFuzzyNumSet& that);

    TriFuzzyNumSet &operator=(TriFuzzyNumSet&& that);

    void insert(const TriFuzzyNum &num);

    void insert(TriFuzzyNum &&num);

    void remove(const TriFuzzyNum &num);

    TriFuzzyNum arithmetic_mean();

private:
    multiset<TriFuzzyNum> num_set;
};

#endif //FUZZY_H
