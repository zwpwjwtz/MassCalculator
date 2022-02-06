#include "chemicalcomposition.h"


ChemicalComposition::ChemicalComposition()
{}

void ChemicalComposition::clear()
{
    elements.clear();
    defaultIsotope.clear();
}

bool ChemicalComposition::isEmpty() const
{
    return elements.size() == 0;
}

std::vector<int> ChemicalComposition::allElements() const
{
    std::vector<int> elementNumbers;
    elementNumbers.reserve(defaultIsotope.size());
    for (auto i=defaultIsotope.cbegin(); i!=defaultIsotope.cend(); i++)
        elementNumbers.push_back(i->first);
    return elementNumbers;
}

void ChemicalComposition::clearElement(int atomNumber)
{
    clearIsotope(atomNumber, 0);
}

double ChemicalComposition::countElement(int atomNumber) const
{
    return countIsotope(atomNumber, 0);
}

void ChemicalComposition::setElement(int atomNumber, double count)
{
    setIsotope(atomNumber, 0, count);
}

std::vector<std::pair<int, int>> ChemicalComposition::allIsotopes() const
{
    std::vector<std::pair<int, int>> isotopes;
    isotopes.reserve(elements.size());
    for (auto i=elements.cbegin(); i!=elements.cend(); i++)
        isotopes.push_back(i->first);
    return isotopes;
}

void ChemicalComposition::clearIsotope(int atomNumber, int nominalMass)
{
    auto j = defaultIsotope.find(atomNumber);
    if (j == defaultIsotope.end())
        return;

    int isotopeCount = 0;
    for (auto i = elements.begin(); i!=elements.end();)
    {
        if (i->first.first == atomNumber)
        {
            isotopeCount++;

            if (nominalMass > 0)
            {
                if (i->first.second == nominalMass)
                    i = elements.erase(i);
                else
                    i++;
            }
            else
                i = elements.erase(i);
        }
        else
            i++;
    }
    if (nominalMass == 0 || isotopeCount <= 1)
        defaultIsotope.erase(j);
}

double ChemicalComposition::countIsotope(int atomNumber, int nominalMass) const
{
    double count = 0;
    if (nominalMass > 0)
    {
        auto i = elements.find({atomNumber, nominalMass});
        if (i != elements.cend())
            count = i->second;
    }
    else
    {
        for (auto i = elements.begin(); i!=elements.end(); i++)
        {
            if (i->first.first == atomNumber)
                count += i->second;
        }
    }
    return count;
}

void ChemicalComposition::setIsotope(int atomNumber,
                                     int nominalMass,
                                     double count)
{
    std::map<std::pair<int, int>, double>::iterator i;
    std::map<int, bool>::iterator j = defaultIsotope.find(atomNumber);
    if (j == defaultIsotope.cend())
    {
        defaultIsotope.insert({atomNumber, nominalMass == 0});
        elements.insert({{atomNumber, nominalMass}, count});
        return;
    }

    if (nominalMass == 0)
    {
        for (i = elements.begin(); i!=elements.end(); i++)
        {
            if (i->first.first == atomNumber)
            {
                if (j->second)
                {
                    // No isotopic pattern specified yet; modify it
                    i->second = count;
                }
                else
                {
                    // Isotopic pattern already specified;
                    // clear it and create a new entry
                    while (i != elements.end())
                    {
                        if (i->first.first == atomNumber)
                            i = elements.erase(i);
                        else
                            i++;
                    }
                    j->second = true;
                    elements.insert({{atomNumber, 0}, count});
                }
                break;
            }
        }
    }
    else
    {
        if (j->second)
        {
            // No isotopic pattern specified yet;
            // clear the default entry and create a new one
            j->second = false;
            elements.erase(elements.find({atomNumber, 0}));
            elements.insert({{atomNumber, nominalMass}, count});
        }
        else
        {
            // Isotopic pattern already specified; modify it
            i = elements.find({atomNumber, nominalMass});
            if (i == elements.end())
                elements.insert({{atomNumber, nominalMass}, count});
            else
                i->second = count;
        }
    }
}

bool ChemicalComposition::operator==(const ChemicalComposition& arg) const
{
    return this->elements == arg.elements;
}

bool ChemicalComposition::operator!=(const ChemicalComposition& arg) const
{
    return this->elements != arg.elements;
}

ChemicalComposition
ChemicalComposition::operator+(const ChemicalComposition& arg) const
{
    ChemicalComposition result(*this);

    // Merge "defaultIsotope"
    std::map<int, bool>::const_iterator m;
    std::map<int, bool>::iterator n;
    for (m = arg.defaultIsotope.cbegin(); m!= arg.defaultIsotope.cend(); m++)
    {
        for (n = result.defaultIsotope.begin();
             n!= result.defaultIsotope.end(); n++)
        {
            if (m->first == n->first)
            {
                if (m->second ^ n->second)
                    n->second = false;
            }
        }
        if (n == result.defaultIsotope.end())
            result.defaultIsotope.insert(*m);
    }

    // Merge "elements"
    std::map<std::pair<int, int>, double>::const_iterator i;
    std::map<std::pair<int, int>, double>::iterator j;
    for (i=arg.elements.cbegin(); i!=arg.elements.cend(); i++)
    {
        for (j=result.elements.begin(); j!=result.elements.end(); j++)
        {
            if (i->first.first == j->first.first &&
                i->first.second == j->first.second)
            {
                j->second += i->second;
                if (j->second == 0.0)
                    result.clearIsotope(j->first.first, j->first.second);
                break;
            }
        }
        if (j == result.elements.end())
            result.elements.insert(*i);
    }

    return result;
}

ChemicalComposition
ChemicalComposition::operator-(const ChemicalComposition& arg) const
{
    ChemicalComposition result(*this);

    // Merge "defaultIsotope"
    std::map<int, bool>::const_iterator m;
    std::map<int, bool>::iterator n;
    for (m = arg.defaultIsotope.cbegin(); m!= arg.defaultIsotope.cend(); m++)
    {
        for (n = result.defaultIsotope.begin();
             n!= result.defaultIsotope.end(); n++)
        {
            if (m->first == n->first)
            {
                if (m->second ^ n->second)
                    n->second = false;
            }
        }
        if (n == result.defaultIsotope.end())
            result.defaultIsotope.insert(*m);
    }

    // Merge "elements"
    std::map<std::pair<int, int>, double>::const_iterator i;
    std::map<std::pair<int, int>, double>::iterator j;
    for (i=arg.elements.cbegin(); i!=arg.elements.cend(); i++)
    {
        for (j=result.elements.begin(); j!=result.elements.end(); j++)
        {
            if (i->first.first == j->first.first &&
                i->first.second == j->first.second)
            {
                j->second -= i->second;
                if (j->second == 0.0)
                    result.clearIsotope(j->first.first, j->first.second);
                break;
            }
        }
        if (j == result.elements.end())
            result.elements.insert(*i);
    }

    return result;
}

ChemicalComposition ChemicalComposition::operator*(double count) const
{
    ChemicalComposition result(*this);
    std::map<std::pair<int, int>, double>::iterator i;
    for (i=result.elements.begin(); i!=result.elements.end(); i++)
        i->second *= count;
    return result;
}

ChemicalComposition ChemicalComposition::operator/(double count) const
{
    if (count == 0.0)
        return ChemicalComposition();

    ChemicalComposition result(*this);
    std::map<std::pair<int, int>, double>::iterator i;
    for (i=result.elements.begin(); i!=result.elements.end(); i++)
        i->second /= count;
    return result;
}

ChemicalComposition operator*(double count, const ChemicalComposition& arg)
{
    return arg * count;
}
