#include "atomabundance.h"
#include "atom/atomic_abundance_isotopic.h"


int AtomAbundance::mostAbundantMassNumber(int atomNumber)
{
    if (atomNumber <= 0 && atomNumber > MC_ATOMIC_ABUNDANCE_ELEMENT_MAX)
        return 0;
    for (int i=0; i<MC_ATOMIC_ABUNDANCE_ELEMENT_ISOTOPE_MAX; i++)
    {
        if (mc_atom_abundance_rank[atomNumber][i] == 1)
            return int(mc_atom_abundance_isotopic[atomNumber][i][0]);
    }
    // Unreachable code
    return 0;
}

float AtomAbundance::relativeAbundance(int atomNumber, int massNumber)
{
    if (atomNumber <= 0 && atomNumber > MC_ATOMIC_ABUNDANCE_ELEMENT_MAX)
        return 0;

    const float* isotopeData;
    for (int i=0; i<MC_ATOMIC_ABUNDANCE_ELEMENT_ISOTOPE_MAX; i++)
    {
        isotopeData = mc_atom_abundance_isotopic[atomNumber][i];
        if (int(isotopeData[0]) == massNumber)
            return isotopeData[1] / 100.0f;
        if (isotopeData[0] == 0.0f)
            return 0;
    }
    return 0;
}

float AtomAbundance::relativeNormalizedAbundance(int atomNumber, int massNumber)
{
    if (atomNumber <= 0 && atomNumber > MC_ATOMIC_ABUNDANCE_ELEMENT_MAX)
        return 0;

    float sumAbundance;
    const float* isotopeData;
    for (int i=0; i<MC_ATOMIC_ABUNDANCE_ELEMENT_ISOTOPE_MAX; i++)
    {
        isotopeData = mc_atom_abundance_isotopic[atomNumber][i];
        if (int(isotopeData[0]) == massNumber)
        {
            int j = 0;
            sumAbundance = 0;
            while (j < MC_ATOMIC_ABUNDANCE_ELEMENT_ISOTOPE_MAX)
            {
                sumAbundance += mc_atom_abundance_isotopic[atomNumber][j][1];
                if (mc_atom_abundance_isotopic[atomNumber][++j][0] == 0.0f)
                    break;
            }
            return isotopeData[1] / sumAbundance;
        }
        if (isotopeData[0] == 0.0f)
            return 0;
    }
    return 0;
}
