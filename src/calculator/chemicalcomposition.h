#ifndef CHEMICALCOMPOSITION_H
#define CHEMICALCOMPOSITION_H

#include <vector>
#include <map>


class ChemicalComposition
{
public:
    ChemicalComposition();

    void clear();
    bool isEmpty() const;

    std::vector<int> allElements() const;
    void clearElement(int atomNumber);
    double countElement(int atomNumber) const;
    void setElement(int atomNumber, double count);

    std::vector<std::pair<int, int>> allIsotopes() const;
    void clearIsotope(int atomNumber, int nominalMass);
    double countIsotope(int atomNumber, int nominalMass) const;
    void setIsotope(int atomNumber, int nominalMass, double count);

    bool operator==(const ChemicalComposition& arg) const;
    bool operator!=(const ChemicalComposition& arg) const;
    ChemicalComposition operator+(const ChemicalComposition& arg) const;
    ChemicalComposition operator-(const ChemicalComposition& arg) const;

protected:
    std::map<int, bool> defaultIsotope;
    std::map<std::pair<int, int>, double> elements;
};

#endif // CHEMICALCOMPOSITION_H
