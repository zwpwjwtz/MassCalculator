#include <cmath>
#include <limits>
#include <list>
#include "isotopicpattern.h"
#include "isotopicpattern_p.h"
#include "atommass.h"
#include "atomabundance.h"

#define MC_ISOTOPIC_ISOTOPE_MAX   40


IsotopicPattern::IsotopicPattern()
{
    d_ptr = new IsotopicPatternPrivate;

    d_ptr->interval = defaultBinningWidth;
    d_ptr->maxIntervalCount = defaultMaxBinCount;
}

IsotopicPattern::~IsotopicPattern()
{
    delete d_ptr;
}

ChemicalComposition IsotopicPattern::composition() const
{
    return d_ptr->composition;
}

bool IsotopicPattern::setComposition(const ChemicalComposition& composition)
{
    double count;
    double epsilon = std::numeric_limits<double>::epsilon();
    auto isotopeList = composition.allIsotopes();
    for (auto i=isotopeList.cbegin(); i!=isotopeList.cend(); i++)
    {
        // Check if the given composition is valid
        count = composition.countIsotope(i->first, i->second);
        if (count < 0 || fabs(count - int(count)) > epsilon)
            return false;
    }
    d_ptr->composition = composition;
    return true;
}

double IsotopicPattern::binningWidth() const
{
    return d_ptr->interval;
}

void IsotopicPattern::setBinningWidth(double width)
{
    d_ptr->interval = width > 0 ? width : 0;
}

int IsotopicPattern::maxBinCount() const
{
    return d_ptr->maxIntervalCount;
}

void IsotopicPattern::setMaxBinCount(int count)
{
    d_ptr->maxIntervalCount = count > 0 ? count : 0;
}

std::vector<std::pair<double, double>> IsotopicPattern::distribution() const
{
    std::vector<std::pair<double, double>> result;

    // Retrieve the number and the abundance information for all isotopes
    int atomCount;
    std::vector<int> allElements = d_ptr->composition.allElements();
    std::vector<int> isotopeCount;
    std::vector<std::pair<double, double>> tempDistribution;
    std::list<std::vector<std::pair<double, double>>> isotopeList;
    for (auto i=allElements.begin(); i!=allElements.end();)
    {
        atomCount = int(d_ptr->composition.countElement(*i));
        tempDistribution = distribution(*i);
        if (atomCount > 0 && tempDistribution.size() > 0)
        {
            isotopeCount.push_back(atomCount);
            isotopeList.push_back(tempDistribution);
            i++;
        }
        else
            i = allElements.erase(i);
    }

    // Generate a list of possible mass for the given isotope list
    // Each element is dealed in an iteration
    // Each isotope is dealed in a recursive layer
    int maxMassCount = maxBinCount();
    double atomMass;
    std::list<double> massList, tempMassList;
    std::list<double>::iterator l, m;
    for (auto j=isotopeList.cbegin(); j!=isotopeList.cend(); j++)
    {
        atomCount = isotopeCount[std::distance(isotopeList.cbegin(), j)];
        tempMassList = IsotopicPatternPrivate::enumerateIsotopeMass(j->cbegin(),
                                                                    j->cend(),
                                                                    atomCount);
        if (tempMassList.size() == 0)
            continue;
        if (massList.size() == 0)
            massList.assign(tempMassList.cbegin(), tempMassList.cend());
        else
        {
            for (l = massList.begin(); l!=massList.end();)
            {
                atomMass = *l;
                l = massList.erase(l);
                for (m = tempMassList.begin(); m!=tempMassList.end(); m++)
                {
                    massList.insert(l, atomMass + *m);
                    if (massList.size() > maxMassCount)
                        break;
                }
            }
            IsotopicPatternPrivate::binMassList(massList, d_ptr->interval);
        }

        // Truncate the mass list if it is oversized
        if (massList.size() > maxMassCount)
        {
            l = massList.begin();
            std::advance(l, maxBinCount());
            massList.erase(l, massList.end());
        }
    }

    for (l=massList.begin(); l!=massList.end(); l++)
        result.push_back({*l, 0});
    return result;
}

std::vector<std::pair<double, double>>
IsotopicPattern::distribution(int atomNumber) const
{
    int j, maxCount;
    float abundance;
    double atomMass;
    std::vector<std::pair<double, double>> distribution;
    int* massNumbers = new int[MC_ISOTOPIC_ISOTOPE_MAX];
    auto allIsotopes = d_ptr->composition.allIsotopes();
    for (auto i=allIsotopes.cbegin(); i!=allIsotopes.cend(); i++)
    {
        if (i->first != atomNumber)
            continue;

        // Check if a specific isotope is set for each element
        if (i->second > 0)
        {
            atomMass = AtomMass::mass(i->first, i->second);
            abundance = AtomAbundance::relativeAbundance(i->first, i->second);
            if (atomMass > 0)
                distribution.push_back({atomMass, abundance});
        }
        else
        {
            // No isotope specified for this element
            // Retrieve all known isotopes with abundance information
            maxCount = AtomMass::getAllMassNumbers(i->first,
                                                   MC_ISOTOPIC_ISOTOPE_MAX,
                                                   massNumbers);
            for (j=0; j<maxCount; j++)
            {
                abundance = AtomAbundance::relativeAbundance(i->first,
                                                             massNumbers[j]);
                if (abundance > 0)
                    distribution.push_back({AtomMass::mass(i->first,
                                                          massNumbers[j]),
                                            abundance});
            }
        }
    }
    delete[] massNumbers;

    return distribution;
}

std::list<double> IsotopicPatternPrivate::enumerateIsotopeMass(
            std::vector<std::pair<double, double>>::const_iterator isotope,
            std::vector<std::pair<double, double>>::const_iterator isotopeEnd,
            int atomCount)
{
    double atomMass = isotope->first;
    std::list<double> massList;
    massList.push_back(atomCount * atomMass);
    if (isotope + 1 != isotopeEnd)
    {
        // Deal with contributions from other isotopes
        std::list<double> tempMassList;
        std::list<double>::const_iterator i;
        for (int c=atomCount - 1; c>=0 ; c--)
        {
            // The total atom count should be the same for each combination
            tempMassList = enumerateIsotopeMass(isotope + 1, isotopeEnd,
                                                atomCount - c);
            for (i=tempMassList.cbegin(); i!=tempMassList.cend(); i++)
            {
                // Sum up the atomic weight for each combination
                massList.push_back(*i + c * atomMass);
            }
        }
    }
    return massList;
}

void IsotopicPatternPrivate::binMassList(std::list<double>& mass,
                                         double interval)
{
    // Group (merge) mass with specified bin width
    std::list<double>::iterator i, j;
    for (i=mass.begin(); i!=mass.end(); i++)
    {
        j = i;
        for (j++; j!=mass.end();)
        {
            if (fabs(*i - *j) < interval)
                j = mass.erase(j);
            else
                j++;
        }
    }
}
