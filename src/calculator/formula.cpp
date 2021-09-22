#include <sstream>
#include "formula.h"
#include "atomname.h"
#include "atommass.h"
#include "chemicalcomposition.h"


bool Formula::parse(const std::string& formula)
{
    char c;
    char* p;
    int atomNumber;
    double atomCount = 0, previousAtomCount;
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
                previousAtomCount = countElement(atomNumber);
                setElement(atomNumber, previousAtomCount + atomCount);
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
    return !errorFlag;
}

double Formula::toAverageMass() const
{
    // Sum up the averaged atomic weight
    double mass = 0;
    for (auto i=elements.cbegin(); i!=elements.cend(); i++)
        mass += i->second * AtomMass::averageMass(i->first.first);
    return mass;
}

double Formula::toMass() const
{
    // Sum up the atomic weight
    double mass = 0;
    for (auto i=elements.cbegin(); i!=elements.cend(); i++)
        mass += i->second * AtomMass::monoisotopicMass(i->first.first);
    return mass;
}

double Formula::toMass(const std::string& formula, bool* error)
{
    Formula f;
    if (error == nullptr)
        f.parse(formula);
    else
        *error = !f.parse(formula);
    return f.toMass();
}

std::string Formula::toString() const
{
    std::ostringstream result;
    for (auto i=elements.cbegin(); i!=elements.cend(); i++)
    {
        if (i->second > 1)
            result << AtomName::abbreviation(i->first.first) << i->second;
        else if (i->second == 1.0)
            result << AtomName::abbreviation(i->first.first);
    }
    return result.str();
}

std::string
Formula::toString(const std::vector<std::string>& elementOrder) const
{
    std::ostringstream result;

    // Output elements with specified (priority) order
    std::map<std::pair<int, int>, double>::iterator i;
    std::map<std::pair<int, int>, double> tempElementList(elements);
    for (auto j=elementOrder.cbegin(); j!= elementOrder.cend(); j++)
    {
        for (i=tempElementList.begin(); i!=tempElementList.end(); i++)
        {
            if (*j == AtomName::abbreviation(i->first.first))
                break;
        }
        if (i != tempElementList.end())
        {
            if (i->second > 1)
                result << AtomName::abbreviation(i->first.first) << i->second;
            else if (i->second == 1.0)
                result << AtomName::abbreviation(i->first.first);
            tempElementList.erase(i);
        }
    }

    // Output the rest elements
    for (i=tempElementList.begin(); i!=tempElementList.cend(); i++)
    {
        if (i->second > 1)
            result << AtomName::abbreviation(i->first.first) << i->second;
        else if (i->second == 1.0)
            result << AtomName::abbreviation(i->first.first);
    }
    return result.str();
}
