#ifndef VALENCE_H
#define VALENCE_H

#include "chemicalcomposition.h"


class Valence
{
public:
    static double doubleBondIndex(const ChemicalComposition& composition);
};

#endif // VALENCE_H
