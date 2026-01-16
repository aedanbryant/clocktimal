#ifndef CLOCKTIMAL_H
#define CLOCKTIMAL_H


// User defined metric states
#define MOVECOUNT       (1 << 0)
#define TICKCOUNT       (1 << 1)
#define SIMLULCOUNT     (1 << 2)
#define SIMTICKCOUNT    (1 << 3)
#define NONMOVECOUNT    (TICKCOUNT | SIMLULCOUNT | SIMTICKCOUNT)


#endif