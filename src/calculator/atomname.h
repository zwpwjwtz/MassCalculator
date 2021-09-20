#ifndef ATOMNAME_H
#define ATOMNAME_H

#include <string>


class AtomName
{
public:
    static int numberFromAbbreviation(const std::string& abbr);
};

#endif // ATOMNAME_H
