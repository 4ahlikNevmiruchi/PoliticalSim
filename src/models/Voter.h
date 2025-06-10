#ifndef VOTER_H
#define VOTER_H

#include <QString>

/**
 * @brief Data structure representing a voter (citizen).
 */
struct Voter {
    int id = -1;                ///< Unique voter ID (database primary key).
    QString name;               ///< Name of the voter.
    int ideologyId = -1;        ///< ID of the voter's ideology.
    QString ideology;           ///< Name of the voter's ideology.
    int partyId = -1;           ///< ID of the party the voter is affiliated with.
    QString partyName;          ///< Name of the party the voter is affiliated with.
    int ideologyX = 0;          ///< X (economic) axis coordinate of the voter's ideology.
    int ideologyY = 0;          ///< Y (social) axis coordinate of the voter's ideology.

    /** @brief Default constructor. Initializes a Voter with default values. */
    Voter() = default;

    /** @brief Constructs a Voter with the given name, ideology name, and party ID (for a new unsaved voter). */
    Voter(const QString& name, const QString& ideology, int partyId)
        : name(name), ideology(ideology), partyId(partyId) {}

    /** @brief Constructs a Voter with all fields specified (typically from a database record). */
    Voter(int _id, const QString& _name, const QString& _ideology,
          int _ideologyId, int _partyId, const QString& _partyName,
          int _ideologyX, int _ideologyY)
        : id(_id),
        name(_name),
        ideologyId(_ideologyId),
        ideology(_ideology),
        partyId(_partyId),
        partyName(_partyName),
        ideologyX(_ideologyX),
        ideologyY(_ideologyY)
    {}
};

#endif // VOTER_H
