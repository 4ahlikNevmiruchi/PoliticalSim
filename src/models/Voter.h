#ifndef VOTER_H
#define VOTER_H

#include <QString>

struct Voter {
    int id = -1;
    QString name;
    QString ideology;
    int partyId = -1;
    QString partyName;
};

#endif // VOTER_H
