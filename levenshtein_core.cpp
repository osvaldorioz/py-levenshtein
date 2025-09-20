#include "levenshtein_core.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <vector>

namespace levenshtein {

double LogDiscountFunction::compute(int discounts) const {
    if (discounts <= 0) return 1.0;
    double d = static_cast<double>(discounts);
    return 1.0 / (std::log(1.0 + d / 5.0) + 1.0);
}

double ExpDiscountFunction::compute(int discounts) const {
    if (discounts <= 0) return 1.0;
    double d = static_cast<double>(discounts);
    return 1.0 / std::pow(d + 1.0, 0.2);
}

int DefaultCodaDetector::detect(const std::string& s, const std::string& vowels) const {
    std::string low_s;
    for (char c : s) {
        low_s += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    size_t len = low_s.size();
    size_t first_vowel = len;
    for (size_t i = 0; i < len; ++i) {
        if (vowels.find(low_s[i]) != std::string::npos) {
            first_vowel = i;
            break;
        }
    }
    if (first_vowel == len) {
        return static_cast<int>(len);
    }
    size_t coda = first_vowel + 1;
    for (size_t i = first_vowel; i < len; ++i) {
        if (vowels.find(low_s[i]) == std::string::npos) {
            coda = i;
            break;
        }
    }
    return static_cast<int>(coda);
}

DiscountedLevenshtein::DiscountedLevenshtein(const DiscountFunction& discount_func, const CodaDetector& coda_detector)
    : discount_func_(discount_func), coda_detector_(coda_detector) {}

double DiscountedLevenshtein::compute(
    const std::string& src,
    const std::string& tar,
    const std::string& discount_from,
    const std::string& discount_func,
    const std::string& mode,
    const std::string& vowels
) const {
    int m = static_cast<int>(src.size());
    int n = static_cast<int>(tar.size());
    int df_src, df_tar;

    if (discount_from == "coda") {
        df_src = coda_detector_.detect(src, vowels);
        df_tar = coda_detector_.detect(tar, vowels);
    } else {
        df_src = df_tar = std::stoi(discount_from);
    }

    if (src == tar) return 0.0;

    if (m == 0) {
        double sum = 0.0;
        for (int j = 1; j <= n; ++j) {
            sum += discount_func_.compute(std::max(0, j - df_tar));
        }
        return sum;
    }
    if (n == 0) {
        double sum = 0.0;
        for (int i = 1; i <= m; ++i) {
            sum += discount_func_.compute(std::max(0, i - df_src));
        }
        return sum;
    }

    std::vector<std::vector<double>> d(m + 1, std::vector<double>(n + 1, 0.0));

    for (int i = 1; i <= m; ++i) {
        d[i][0] = d[i - 1][0] + discount_func_.compute(std::max(0, i - df_src));
    }
    for (int j = 1; j <= n; ++j) {
        d[0][j] = d[0][j - 1] + discount_func_.compute(std::max(0, j - df_tar));
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            double del_c = discount_func_.compute(std::max(0, i - df_src));
            double ins_c = discount_func_.compute(std::max(0, j - df_tar));
            double sub_c = (src[i - 1] == tar[j - 1]) ? 0.0 : discount_func_.compute(std::max(0, std::max(i - df_src, j - df_tar)));

            double del = d[i - 1][j] + del_c;
            double ins = d[i][j - 1] + ins_c;
            double sub = d[i - 1][j - 1] + sub_c;
            double min_val = std::min({del, ins, sub});

            if (mode == "osa" && i > 1 && j > 1 && src[i - 2] == tar[j - 1] && src[i - 1] == tar[j - 2]) {
                double trans_c = discount_func_.compute(std::max(0, std::max((i - 1) - df_src, (j - 1) - df_tar)));
                double trans = d[i - 2][j - 2] + trans_c;
                min_val = std::min(min_val, trans);
            }

            d[i][j] = min_val;
        }
    }
    return d[m][n];
}

} // namespace levenshtein