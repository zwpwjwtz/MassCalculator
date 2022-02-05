#include <cstring>
#include "atomname.h"
#include "../atom/atomic_abbreviation.h"


std::string AtomName::abbreviation(int atomNumber)
{
    if (atomNumber > 0 && atomNumber <= MC_ATOMIC_ABBR_MAX)
        return mc_atomname_abbr[atomNumber];
    else
        return std::string();
}

int AtomName::numberFromAbbreviation(const std::string& abbr)
{
    for (int i=1; i<=MC_ATOMIC_ABBR_MAX; i++)
    {
        if (strcmp(mc_atomname_abbr[i], abbr.c_str()) == 0)
            return i;
    }
    return 0;
}
