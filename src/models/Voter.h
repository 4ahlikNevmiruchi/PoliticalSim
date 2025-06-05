#ifndef VOTER_H
#define VOTER_H

#include <QString>

struct Voter {
    int id = -1;
    QString name;
    QString ideology;
    int partyId = -1;
    QString partyName;

    Voter() = default;

    Voter(const QString& name, const QString& ideology, int partyId)
        : name(name), ideology(ideology), partyId(partyId) {}

    Voter(int id, const QString& name, const QString& ideology, int partyId, const QString& partyName = QString())
        : id(id), name(name), ideology(ideology), partyId(partyId), partyName(partyName) {}
};

#endif // VOTER_H
