#include "formulamass.h"
#include "atomname.h"
#include "atommass.h"
#include "chemicalcomposition.h"


double FormulaMass::toMass(const std::string& formula, bool* error)
{
    char c;
    char* p;
    int atomNumber;
    double atomCount = 0, previousAtomCount;
    ChemicalComposition composition;
    std::string numberBuffer, reversedNumberBuffer;
    std::string nameBuffer, reversedNameBuffer;

    // Parse the formula reversely
    bool errorFlag = false;
    for (long pos = formula.length() - 1; pos >= 0; pos--)
    {
        c = formula.at(pos);
        if (c >= '0' && c <= '9')
        {
            // Reading a number
            if (nameBuffer.length() > 0)
            {
                // Unparse (invalid) atom name; ignore it and continue
                errorFlag = true;
                nameBuffer.clear();
            }
            numberBuffer.push_back(c);
        }
        else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        {
            if (numberBuffer.length() > 0)
            {
                // Parse the subscript
                reversedNumberBuffer.assign(numberBuffer.crbegin(),
                                            numberBuffer.crend());
                atomCount = strtol(reversedNumberBuffer.c_str(), &p, 10);
                if (*p != '\0' || atomCount < 0)
                {
                    // Invalid subscript
                    errorFlag = true;
                }
                numberBuffer.clear();
            }
            else
            {
                if (atomCount == 0)
                {
                    // Atom without subscript; seen as single atom
                    atomCount = 1;
                }
            }

            // Parse the atom name
            nameBuffer.push_back(c);
            reversedNameBuffer.assign(nameBuffer.crbegin(),
                                      nameBuffer.crend());
            atomNumber = AtomName::numberFromAbbreviation(reversedNameBuffer);
            if (atomNumber > 0)
            {
                previousAtomCount = composition.countElement(atomNumber);
                composition.setElement(atomNumber,
                                       previousAtomCount + atomCount);
                nameBuffer.clear();
                atomCount = 0;
            }
        }
        else
        {
            // Invalid character; ignore it and continue
            errorFlag = true;
        }
    }
    if (nameBuffer.length() > 0 || numberBuffer.length() > 0)
    {
        // Unparsed characters; ignore them
        errorFlag = true;
    }

    if (error != nullptr)
        *error = errorFlag;

    // Sum up the atomic weight
    double mass = 0;
    auto elementList = composition.allElements();
    for (auto i=elementList.cbegin(); i!=elementList.cend(); i++)
        mass += composition.countElement(*i) * AtomMass::monoisotopicMass(*i);
    return mass;
}
