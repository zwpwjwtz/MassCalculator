#include <sstream>
#include "formula.h"
#include "atomname.h"
#include "atommass.h"
#include "chemicalcomposition.h"


Formula::Formula()
{}

Formula::Formula(const ChemicalComposition& composition)
    : ChemicalComposition (composition)
{}

bool Formula::parse(const std::string &formula)
{
    // Parse the formula recursively
    long beginning = 0;
    return parseGroup(formula, beginning, *this);
}

bool Formula::parseGroup(const std::string& formula, long& beginning,
                         ChemicalComposition& result)
{
    char c;
    char* p;
    int atomNumber;
    double atomCount = 0;
    std::string numberBuffer, reversedNumberBuffer;
    std::string nameBuffer, reversedNameBuffer;

    // Parse the formula reversely
    result.clear();
    long pos2;
    bool errorFlag = false;
    ChemicalComposition tempComposition;
    for (long pos = formula.length() - 1; pos >= beginning; pos--)
    {
        c = formula.at(pos);

        if (c == '(')
        {
            // End parsing a group of atoms
            beginning = pos;
            break;
        }
        if ((c >= '0' && c <= '9') || c == '+' || c == '-')
        {
            // Reading a number
            if (nameBuffer.length() > 0)
            {
                // Unparsed (invalid) atom name; ignore it and continue
                errorFlag = true;
                nameBuffer.clear();
            }
            numberBuffer.push_back(c);
        }
        else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ')')
        {
            if (numberBuffer.length() > 0)
            {
                // Parse the subscript
                reversedNumberBuffer.assign(numberBuffer.crbegin(),
                                            numberBuffer.crend());
                atomCount = strtol(reversedNumberBuffer.c_str(), &p, 10);
                if (*p != '\0')
                {
                    // Invalid subscript
                    errorFlag = true;
                }
                numberBuffer.clear();
            }
            else
            {
                if (atomCount == 0.0)
                {
                    // Atom without subscript; seen as single atom
                    atomCount = 1;
                }
            }

            if (c == ')')
            {
                // Begin parsing a group of atoms
                pos2 = beginning;
                errorFlag |= !parseGroup(formula.substr(0, pos), pos2,
                                         tempComposition);
                result = result + tempComposition * atomCount;
                pos = pos2;
            }
            else
            {
                // Parse the atom name
                nameBuffer.push_back(c);
                reversedNameBuffer.assign(nameBuffer.crbegin(),
                                          nameBuffer.crend());
                atomNumber =
                        AtomName::numberFromAbbreviation(reversedNameBuffer);
                if (atomNumber > 0)
                {
                    result.setElement(atomNumber,
                                      result.countElement(atomNumber) +
                                      atomCount);
                    nameBuffer.clear();
                    atomCount = 0;
                }
            }
        }
        else if (c >= '\0' && c <= ' ')
        {
            // Blank or control character; ignore it and continue
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
        if (i->second == 1.0)
            result << AtomName::abbreviation(i->first.first);
        else if (i->second != 0.0)
            result << AtomName::abbreviation(i->first.first) << i->second;
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
            if (i->second == 1.0)
                result << AtomName::abbreviation(i->first.first);
            else if (i->second != 0.0)
                result << AtomName::abbreviation(i->first.first) << i->second;
            tempElementList.erase(i);
        }
    }

    // Output the rest elements
    for (i=tempElementList.begin(); i!=tempElementList.cend(); i++)
    {
        if (i->second == 1.0)
            result << AtomName::abbreviation(i->first.first);
        else if (i->second != 0.0)
            result << AtomName::abbreviation(i->first.first) << i->second;
    }
    return result.str();
}
