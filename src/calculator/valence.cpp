#include "valence.h"
#include "atomvalence.h"


double Valence::doubleBondIndex(const ChemicalComposition& composition)
{
    int atomCount, atomValence;
    int negativeValenceCount = 0;
    double valence = 0;
    auto allElements = composition.allElements();
    for (auto i=allElements.cbegin(); i!=allElements.cend(); i++)
    {
        atomCount = composition.countElement(*i);
        atomValence = AtomValence::defaultValence(*i);
        valence += atomValence * atomCount;
        if (atomValence < -1)
            negativeValenceCount += atomCount;
    }
    // Correction for atoms that have negative valence (valence < -1)
    // e.g. for C2H6O, DBI = (-4) * 2 + 1 * 6 + (-2) + CORRECTION,
    // where CORRECTION = ((2 + 1) - 1) * 2
    if (negativeValenceCount > 1)
        valence += (negativeValenceCount - 1) * 2;
    return -(valence / 2);
}
