#ifndef FORMULA_H
#define FORMULA_H

#include <string>
#include "chemicalcomposition.h"


class Formula : public ChemicalComposition
{
public:
    Formula();
    Formula(const ChemicalComposition& composition);

    bool parse(const std::string& formula);

    double toAverageMass() const;
    double toMass() const;
    static double toMass(const std::string& formula, bool* error = nullptr);

    std::string toString() const;
    std::string toString(const std::vector<std::string>& elementOrder) const;

protected:
     bool parseGroup(const std::string& formula, long &beginning,
                     ChemicalComposition& result);
};

#endif // FORMULA_H
