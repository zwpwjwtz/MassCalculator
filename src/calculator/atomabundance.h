#ifndef ATOMABUNDANCE_H
#define ATOMABUNDANCE_H


class AtomAbundance
{
public:
    static int mostAbundantMassNumber(int atomNumber);

    static float relativeAbundance(int atomNumber, int massNumber);
    static float relativeNormalizedAbundance(int atomNumber, int massNumber);
};

#endif // ATOMABUNDANCE_H
