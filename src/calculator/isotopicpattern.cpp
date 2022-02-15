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
    d_ptr->interval = width > 0 ? width / 2.0 : 0;
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
    IsotopicPatternPrivate::IsotopeAbundanceList tempDistribution;
    IsotopicPatternPrivate::ElementList isotopeList;
    for (auto i=allElements.begin(); i!=allElements.end();)
    {
        atomCount = int(d_ptr->composition.countElement(*i));
        tempDistribution = distribution(*i);
        if (atomCount > 0 && tempDistribution.size() > 0)
        {
            isotopeList.push_back({atomCount, tempDistribution});
            i++;
        }
        else
            i = allElements.erase(i);
    }

    // Generate a list of possible mass for the given isotope list
    // Each element is dealed in an iteration
    // Each isotope is dealed in a recursive layer
    double atomMass;
    int maxMassCount = maxBinCount();
    std::list<double> massList, tempMassList;
    std::list<double>::iterator l, m;
    for (auto j=isotopeList.cbegin(); j!=isotopeList.cend(); j++)
    {
        tempMassList = IsotopicPatternPrivate::enumerateMass(j->second.cbegin(),
                                                             j->second.cend(),
                                                             j->first);
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
            massList.sort();
            l = massList.begin();
            std::advance(l, maxMassCount);
            massList.erase(l, massList.end());
        }
    }

    // Calculate the convoluted probability at each (binned) mass
    massList.sort();
    result.reserve(massList.size());
    for (l=massList.begin(); l!=massList.end(); l++)
        result.push_back(
            {*l,
             IsotopicPatternPrivate::jointProbability(*l, d_ptr->interval,
                                                      0, massList,
                                                      isotopeList.cbegin(),
                                                      isotopeList.cend())});
    return result;
}

std::vector<std::pair<double, double>>
IsotopicPattern::distribution(int atomNumber) const
{
    int j, maxCount;
    float abundance;
    double atomMass;
    int massNumbers[MC_ISOTOPIC_ISOTOPE_MAX];
    std::vector<std::pair<double, double>> distribution;
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
                abundance =
                    AtomAbundance::relativeAbundance(i->first, massNumbers[j]);
                if (abundance > 0)
                    distribution.push_back({AtomMass::mass(i->first,
                                                          massNumbers[j]),
                                            abundance});
            }
        }
    }

    // Normalize the distribution
    double sum = 0;
    for (j=0; j<distribution.size(); j++)
        sum += distribution[j].second;
    if (sum > 0)
    {
        for (j=0; j<distribution.size(); j++)
            distribution[j].second /= sum;
    }

    return distribution;
}

unsigned long long IsotopicPatternPrivate::combinatorial(unsigned int N,
                                                         unsigned int r)
{
    unsigned d, i;
    unsigned long long numerator = 1;
    unsigned long long denominator = 1;
    static auto limit = std::numeric_limits<unsigned long long>::max();

    if (r < N / 2 - 1)
        r = N - r;
    for (i=r+1; i<=N; i++)
    {
        if (numerator > limit / i)
        {
            // Will overflow; abort the calculation
            numerator = 0;
            break;
        }
        numerator *= i;
    }
    if (numerator == 0)
        return 0;

    d = N - r;
    for (i=2; i<=d; i++)
        denominator *= i;

    return numerator / denominator;
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
            {
                *i = (*i + *j) / 2;
                j = mass.erase(j);
            }
            else
                j++;
        }
    }
}

std::list<double>
IsotopicPatternPrivate::enumerateMass(IsotopeAbundanceIterator isotopeBegin,
                                      IsotopeAbundanceIterator isotopeEnd,
                                      int atomCount)
{
    double atomMass = isotopeBegin->first;
    std::list<double> massList;
    massList.push_back(atomCount * atomMass);
    if (isotopeBegin + 1 != isotopeEnd)
    {
        // Deal with contributions from other isotopes
        std::list<double> tempMassList;
        std::list<double>::const_iterator i;
        for (int c=atomCount - 1; c>=0 ; c--)
        {
            // The total atom count should be the same for each combination
            tempMassList = enumerateMass(isotopeBegin + 1, isotopeEnd,
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

double
IsotopicPatternPrivate::jointProbability(
                                    double mass,
                                    double tolerance,
                                    double massShift,
                                    const std::list<double>& massList,
                                    const ElementListIterator& elementListBegin,
                                    const ElementListIterator& elementListEnd)
{
    int atomCount = elementListBegin->first;
    const IsotopeAbundanceList& isotopeList = elementListBegin->second;

    ElementListIterator i = elementListBegin;
    if (++i == elementListEnd)
    {
        // This is the last element to deal (last recursive layer)
        return marginalProbability(mass, tolerance, atomCount,
                                   isotopeList.cbegin(), isotopeList.cend());
    }

    // See if combinations with other isotopes are possible
    // Before convoluting, we need to shift the mass list to origin by
    // substracting the minimum isotopic mass in the isotope list
    double minIsotopicMass = massList.front();
    for (auto j=isotopeList.cbegin(); j!=isotopeList.cend(); j++)
    {
        if (j->first < minIsotopicMass)
            minIsotopicMass = j->first;
    }
    massShift += minIsotopicMass * atomCount;

    double residualMass;
    double probability = 0, marginal1, marginal2;
    for (auto m=massList.begin(); m!=massList.end(); m++)
    {
        residualMass = *m - massShift;
        if (mass <= residualMass)
            continue;

        marginal1 = marginalProbability(mass - residualMass, tolerance,
                                        atomCount,
                                        isotopeList.cbegin(),
                                        isotopeList.cend());
        if (marginal1 <= 0)
            continue;

        i = elementListBegin;
        for (i++; i!=elementListEnd; i++)
        {
            marginal2 = jointProbability(residualMass, tolerance,
                                         massShift, massList,
                                         i, elementListEnd);
            if (marginal2 > 0)
                probability += marginal1 * marginal2;
        }
    }
    return probability;
}

double IsotopicPatternPrivate::marginalProbability(
                                    double residualMass,
                                    double tolerance,
                                    int atomCount,
                                    IsotopeAbundanceIterator isotopeBegin,
                                    const IsotopeAbundanceIterator& isotopeEnd)
{
    double probability = 0, marginal;
    const double& atomMass = isotopeBegin->first;
    const double& abundance = isotopeBegin->second;
    if (fabs(residualMass - atomCount * atomMass) <= tolerance)
        probability += std::pow(abundance, atomCount);

    if (++isotopeBegin != isotopeEnd)
    {
        // See if combinations with other isotopes are possible
        int maxCount = int(ceil(residualMass / atomMass));
        int c = atomCount > maxCount ? maxCount : atomCount;
        for (c--; c>=0; c--)
        {
            marginal = marginalProbability(residualMass - atomMass * c,
                                           tolerance,
                                           atomCount - c,
                                           isotopeBegin, isotopeEnd);
            if (marginal > 0)
                probability += std::pow(abundance, c) * marginal *
                               combinatorial(atomCount, c);

        }
    }
    return probability;
}
