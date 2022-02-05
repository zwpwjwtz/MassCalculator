#include "atommass.h"
#include "../atom/atomic_weight.h"
#include "../atom/atomic_weight_average.h"
#include "../atom/atomic_weight_isotopic.h"


double AtomMass::electronMass()
{
    return  MC_ATOMIC_WEIGHT_ELECTRON;
}

double AtomMass::mass(int atomNumber, int massNumber)
{
    const double* massData;
    if (atomNumber > 0 && atomNumber <= MC_ATOMIC_WEIGHT_ELEMENT_MAX)
    {
        for (int i=0; i<MC_ATOMIC_WEIGHT_ELEMENT_ISOTOPE_MAX; i++)
        {
            massData = mc_atommass_isotopic[atomNumber][i];
            if (int(massData[0]) == massNumber)
                return massData[1];
            if (massData[0] == 0.0)
                return 0;
        }
        return 0;
    }
    else
        return 0;
}

double AtomMass::monoisotopicMass(int atomNumber)
{
    if (atomNumber > 0 && atomNumber <= MC_ATOMIC_WEIGHT_MAX)
        return mc_atommass_monoisotopic[atomNumber];
    else
        return 0;
}

double AtomMass::averageMass(int atomNumber)
{
    if (atomNumber > 0 && atomNumber <= MC_ATOMIC_WEIGHT_AVG_MAX)
        return mc_atommass_average[atomNumber];
    else
        return 0;
}
