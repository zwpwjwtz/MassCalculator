#ifndef FORMULA_H
#define FORMULA_H

#include <string>
#include "chemicalcomposition.h"


class Formula : public ChemicalComposition
{
public:
    bool parse(const std::string& formula);

    double toMass() const;
    static double toMass(const std::string& formula, bool* error = nullptr);

    std::string toString() const;
};

#endif // FORMULA_H
