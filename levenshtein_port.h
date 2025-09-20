#ifndef LEVENSHTEIN_PORT_H
#define LEVENSHTEIN_PORT_H

#include <string>

namespace levenshtein {

class DiscountFunction {
public:
    virtual ~DiscountFunction() = default;
    virtual double compute(int discounts) const = 0;
};

class CodaDetector {
public:
    virtual ~CodaDetector() = default;
    virtual int detect(const std::string& s, const std::string& vowels) const = 0;
};

class LevenshteinAlgorithm {
public:
    virtual ~LevenshteinAlgorithm() = default;
    virtual double compute(
        const std::string& src,
        const std::string& tar,
        const std::string& discount_from,
        const std::string& discount_func,
        const std::string& mode,
        const std::string& vowels
    ) const = 0;
};

} // namespace levenshtein

#endif // LEVENSHTEIN_PORT_H