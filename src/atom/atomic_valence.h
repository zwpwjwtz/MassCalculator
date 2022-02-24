#ifndef ATOMIC_VALENCE_H
#define ATOMIC_VALENCE_H

#define MC_ATOMIC_VALENCE_ELEMENT_MAX          92
#define MC_ATOMIC_VALENCE_STATE_MAX            6

extern const int mc_atom_valence[MC_ATOMIC_VALENCE_ELEMENT_MAX + 1]
                                [MC_ATOMIC_VALENCE_STATE_MAX];

#endif // ATOMIC_VALENCE_H
