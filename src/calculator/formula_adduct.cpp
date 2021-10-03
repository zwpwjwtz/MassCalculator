#include "formula_adduct.h"


const char* mc_formula_adduct_name[MC_FORMULA_ADDUCT_MAX] =
{
    "+H",
    "+Na",
    "+Cl",
    "+K",
    "+NH4",
    "+HCOOH",
    "+CH3COOH",
    "+CF3COOH",
    "-H",
    "+H-H2O",
    "+H-CO2"
};

const int
mc_formula_adduct_composition[MC_FORMULA_ADDUCT_MAX]
                             [MC_FORMULA_ADDUCT_ELEMENT_MAX][3] =
{
    {{1, 0, 1}, {0}, {0}, {0}, {0}},
    {{11, 0, 1}, {0}, {0}, {0}, {0}},
    {{17, 0, 1}, {0}, {0}, {0}, {0}},
    {{19, 0, 1}, {0}, {0}, {0}, {0}},
    {{1, 0, 4}, {7, 0, 1}, {0}, {0}, {0}},
    {{1, 0, 2}, {6, 0, 1}, {8, 0 ,2}, {0}, {0}},
    {{1, 0, 4}, {6, 0, 2}, {8, 0 ,2}, {0}, {0}},
    {{1, 0, 1}, {6, 0, 2}, {8, 0 ,2}, {9, 0 ,3}, {0}},
    {{1, 0, -1}, {0}, {0}, {0}, {0}},
    {{1, 0, -1}, {8, 0, -1}, {0}, {0}, {0}},
    {{1, 0, 1}, {6, 0, -1}, {8, 0, -2}, {0}, {0}}
};