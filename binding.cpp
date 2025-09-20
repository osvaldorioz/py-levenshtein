#include <pybind11/pybind11.h>
#include "levenshtein_core.h"

namespace py = pybind11;
using namespace levenshtein;

double compute_distance(
    const std::string& src,
    const std::string& tar,
    const std::string& discount_from = "1",
    const std::string& discount_func = "log",
    const std::string& mode = "lev",
    const std::string& vowels = "aeiou"
) {
    std::unique_ptr<DiscountFunction> df;
    if (discount_func == "exp") {
        df = std::make_unique<ExpDiscountFunction>();
    } else {
        df = std::make_unique<LogDiscountFunction>();
    }

    DefaultCodaDetector coda_detector;
    DiscountedLevenshtein algo(*df, coda_detector);

    return algo.compute(src, tar, discount_from, discount_func, mode, vowels);
}

PYBIND11_MODULE(levdes, m) {
    m.doc() = "Módulo para Levenshtein descontado";
    m.def("distance", &compute_distance, "Calcula la distancia Levenshtein descontada",
          py::arg("src"), py::arg("tar"), py::arg("discount_from") = "1",
          py::arg("discount_func") = "log", py::arg("mode") = "lev", py::arg("vowels") = "aeiou");
}