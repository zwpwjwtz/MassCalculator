#include "atomvalence.h"
#include "atom/atomic_valence.h"


int AtomValence::defaultValence(int atomNumber)
{
    if (atomNumber > 0 && atomNumber <= MC_ATOMIC_VALENCE_ELEMENT_MAX)
        return mc_atom_valence[atomNumber][0];
    else
        return 0;
}
