#ifndef VOTER_H
#define VOTER_H

#include <QString>

struct Voter {
    int id = -1;
    QString name;
    QString ideology;
    int partyId = -1;
    QString partyName;
    int ideologyX = 0;
    int ideologyY = 0;

    Voter() = default;

    Voter(const QString& name, const QString& ideology, int partyId)
        : name(name), ideology(ideology), partyId(partyId) {}

    Voter(int _id, const QString& _name, const QString& _ideology,
          int _partyId, const QString& _partyName,
          int _ideologyX, int _ideologyY)
        : id(_id),
        name(_name),
        ideology(_ideology),
        partyId(_partyId),
        partyName(_partyName),
        ideologyX(_ideologyX),
        ideologyY(_ideologyY)
    {}
};

#endif // VOTER_H
