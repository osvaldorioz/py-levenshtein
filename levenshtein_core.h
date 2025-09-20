#ifndef LEVENSHTEIN_CORE_H
#define LEVENSHTEIN_CORE_H

#include "levenshtein_port.h"
#include <string>

namespace levenshtein {

class LogDiscountFunction : public DiscountFunction {
public:
    double compute(int discounts) const override;
};

class ExpDiscountFunction : public DiscountFunction {
public:
    double compute(int discounts) const override;
};

class DefaultCodaDetector : public CodaDetector {
public:
    int detect(const std::string& s, const std::string& vowels) const override;
};

class DiscountedLevenshtein : public LevenshteinAlgorithm {
private:
    const DiscountFunction& discount_func_;
    const CodaDetector& coda_detector_;

public:
    DiscountedLevenshtein(const DiscountFunction& discount_func, const CodaDetector& coda_detector);
    double compute(
        const std::string& src,
        const std::string& tar,
        const std::string& discount_from,
        const std::string& discount_func,
        const std::string& mode,
        const std::string& vowels
    ) const override;
};

} // namespace levenshtein

#endif // LEVENSHTEIN_CORE_H