#ifndef ELEMENTRANGE_H
#define ELEMENTRANGE_H

class ElementRange
{
public:
    int atomNumber;
    int minCount;
    int maxCount;
    ElementRange() : atomNumber(0), minCount(0), maxCount(0) {}
    bool operator ==(const ElementRange& arg) const
    {
        return atomNumber == arg.atomNumber &&
               minCount == arg.minCount &&
               maxCount == arg.maxCount;
    }
};

#endif // ELEMENTRANGE_H
