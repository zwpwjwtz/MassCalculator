#ifndef FORMULAMASS_H
#define FORMULAMASS_H

#include <string>


class FormulaMass
{
public:
    static double toMass(const std::string& formula, bool* error = nullptr);
};

#endif // FORMULAMASS_H
