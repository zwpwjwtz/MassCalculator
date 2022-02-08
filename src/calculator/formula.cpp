#include <sstream>
#include <set>
#include "formula.h"
#include "atomname.h"
#include "atommass.h"
#include "atomabundance.h"
#include "chemicalcomposition.h"


Formula::Formula()
{}

Formula::Formula(const ChemicalComposition& composition)
    : ChemicalComposition (composition)
{}

double Formula::countElement(int atomNumber) const
{
    auto i = defaultIsotope.find(atomNumber);
    if (i == defaultIsotope.end())
        return 0;
    if (i->second)
    {
        // No isotopoic pattern set yet; use the default implementation
        return ChemicalComposition::countElement(atomNumber);
    }
    else
    {
        // Isotopoic pattern already set; count for the default isotope
        return ChemicalComposition::countIsotope(atomNumber,
                             AtomAbundance::mostAbundantMassNumber(atomNumber));
    }
}

void Formula::setElement(int atomNumber, double count)
{
    auto i = defaultIsotope.find(atomNumber);
    if (i == defaultIsotope.end() || i->second)
    {
        // No isotopoic pattern set yet; use the default implementation
        ChemicalComposition::setElement(atomNumber, count);
    }
    else
    {
        // Isotopoic pattern already set; set the default isotope
        ChemicalComposition::setIsotope(atomNumber,
                              AtomAbundance::mostAbundantMassNumber(atomNumber),
                              count);
    }
}

void Formula::setIsotope(int atomNumber, int nominalMass, double count)
{
    auto i = defaultIsotope.find(atomNumber);
    if (i != defaultIsotope.end() && i->second)
    {
        // Isotopoic pattern already set
        // Convert the count of element to the count of default isotope
        double atomCount = ChemicalComposition::countElement(atomNumber);
        int massNumber = AtomAbundance::mostAbundantMassNumber(atomNumber);
        if (massNumber != nominalMass)
            ChemicalComposition::setIsotope(atomNumber, massNumber, atomCount);
        else
            count += atomCount;
    }
    ChemicalComposition::setIsotope(atomNumber, nominalMass, count);
}

bool Formula::parse(const std::string &formula)
{
    // Parse the formula recursively
    long beginning = 0;
    return parseGroup(formula, beginning, *this);
}

bool Formula::parseGroup(const std::string& formula, long& beginning,
                         Formula& result)
{
    char c;
    char* p;
    int atomNumber = 0;
    int massNumber = 0;
    double atomCount = 0;
    bool atomCountSpecified = false;
    std::string numberBuffer, reversedNumberBuffer;
    std::string nameBuffer, reversedNameBuffer;

    // Parse the formula reversely
    result.clear();
    long pos2;
    bool isotope = false;
    bool errorFlag = false;
    Formula tempComposition;
    for (long pos = formula.length() - 1; pos >= beginning; pos--)
    {
        c = formula.at(pos);

        if (c == '(')
        {
            // End parsing a group of atoms
            beginning = pos;
            break;
        }
        if (c == '[' && isotope)
        {
            if (atomCount > 0 || atomNumber > 0)
            {
                // End parsing an isotope (atom with specified mass numbers)
                if(numberBuffer.length() > 0)
                {
                    // Parse the mass number (the superscript)
                    reversedNumberBuffer.assign(numberBuffer.crbegin(),
                                                numberBuffer.crend());
                    massNumber = strtol(reversedNumberBuffer.c_str(), &p, 10);
                    if (*p != '\0')
                    {
                        // Invalid superscript
                        errorFlag = true;
                    }
                    result.setIsotope(atomNumber, massNumber,
                                      result.countIsotope(atomNumber,
                                                          massNumber) +
                                      atomCount);
                    numberBuffer.clear();
                }
                else
                {
                    // The isotope flag was set but no mass number was specified
                    // Ignore the isotope flag
                    result.setElement(atomNumber,
                                      result.countElement(atomNumber) +
                                      atomCount);
                }
            }
            atomCount = 0;
            atomCountSpecified = false;
            isotope = false;
        }
        else if ((c >= '0' && c <= '9') || c == '+' || c == '-')
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
        else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                 c == ')' || c == ']')
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
                atomCountSpecified = true;
                numberBuffer.clear();
            }
            else
            {
                if (atomCount == 0.0 && !atomCountSpecified)
                {
                    // Atom without subscript; seen as single atom
                    atomCount = 1;
                }
            }

            if (c == ')')
            {
                // Begin parsing a group of atoms
                isotope = false;
                pos2 = beginning;
                errorFlag |= !parseGroup(formula.substr(0, pos), pos2,
                                         tempComposition);
                result = result + tempComposition * atomCount;
                pos = pos2;
            }
            else if (c == ']')
            {
                // Begin parsing an isotope
                isotope = true;
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
                    // The atom name is valid
                    if (!isotope)
                    {
                        // No mass number specified; append it directly
                        result.setElement(atomNumber,
                                          result.countElement(atomNumber) +
                                          atomCount);
                        atomCountSpecified = false;
                        atomCount = 0;
                    }
                    nameBuffer.clear();
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
    int atomNumber, atomCount;
    double atomMass;
    double sumMass = 0, sumMass2;
    double abundance, sumAbundance;
    std::set<int> processedAtomNumber;
    std::set<int>::const_iterator k;
    for (auto i=elements.cbegin(); i!=elements.cend(); i++)
    {
        atomNumber = i->first.first;
        if (processedAtomNumber.find(atomNumber) != processedAtomNumber.cend())
        {
            // The element has already been processed; skip it
            continue;
        }

        if (defaultIsotope.find(atomNumber)->second)
            sumMass += i->second * AtomMass::averageMass(atomNumber);
        else
        {
            // Calculate the averaged mass weighted by isotopic abundance
            sumMass2 = 0;
            atomCount = 0;
            sumAbundance = 0;
            for (auto j=elements.cbegin(); j!=elements.cend(); j++)
            {
                if (j->first.first != atomNumber)
                    continue;
                atomCount += j->second;
                if (atomCount <= 0)
                    continue;
                atomMass = AtomMass::mass(atomNumber, j->first.second);
                if (atomMass <= 0)
                {
                    // Abundance information not available for this isotope
                    return -1;
                }

                abundance = AtomAbundance::relativeAbundance(atomNumber,
                                                             j->first.second);
                sumMass2 += abundance * atomMass;
                sumAbundance += abundance;
            }
            if (sumAbundance > 0)
                sumMass += double(atomCount) * sumMass2 / sumAbundance;
        }
        processedAtomNumber.insert(atomNumber);
    }
    return sumMass;
}

double Formula::toMass() const
{
    // Sum up the atomic weight
    double mass = 0;
    for (auto i=elements.cbegin(); i!=elements.cend(); i++)
    {
        if (defaultIsotope.find(i->first.first)->second)
            mass += i->second * AtomMass::monoisotopicMass(i->first.first);
        else
            mass += i->second * AtomMass::mass(i->first.first, i->first.second);
    }
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
        if (i->second != 0.0)
        {
            if (defaultIsotope.find(i->first.first)->second)
                result << AtomName::abbreviation(i->first.first);
            else
                result << "["
                       << std::to_string(i->first.second)
                       << AtomName::abbreviation(i->first.first)
                       << "]";
            if (i->second != 1.0)
                result << i->second;
        }
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
            if (i->second != 0.0)
            {
                if (defaultIsotope.find(i->first.first)->second)
                    result << AtomName::abbreviation(i->first.first);
                else
                    result << "["
                           << std::to_string(i->first.second)
                           << AtomName::abbreviation(i->first.first)
                           << "]";
                if (i->second != 1.0)
                    result << i->second;
            }
            tempElementList.erase(i);
        }
    }

    // Output the rest elements
    for (i=tempElementList.begin(); i!=tempElementList.cend(); i++)
    {
        if (i->second != 0.0)
        {
            if (defaultIsotope.find(i->first.first)->second)
                result << AtomName::abbreviation(i->first.first);
            else
                result << "["
                       << std::to_string(i->first.second)
                       << AtomName::abbreviation(i->first.first)
                       << "]";
            if (i->second != 1.0)
                result << i->second;
        }
    }
    return result.str();
}
