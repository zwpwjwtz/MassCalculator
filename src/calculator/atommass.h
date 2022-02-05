#ifndef ATOMMASS_H
#define ATOMMASS_H


class AtomMass
{
public:
    static double electronMass();
    static double mass(int atomNumber, int massNumber);
    static double monoisotopicMass(int atomNumber);
    static double averageMass(int atomNumber);
};

#endif // ATOMMASS_H
