#ifndef PARTY_H
#define PARTY_H

#include <QString>

class Party {
public:
    Party(const QString& name, const QString& ideology, double popularity);
    QString getName() const;
    QString getIdeology() const;
    double getPopularity() const;

private:
    QString m_name;
    QString m_ideology;
    double m_popularity;
};

#endif