#include "Party.h"

Party::Party(const QString& name, const QString& ideology, double popularity)
    : m_name(name), m_ideology(ideology), m_popularity(popularity) {}

QString Party::getName() const {
    return m_name;
}

QString Party::getIdeology() const {
    return m_ideology;
}

double Party::getPopularity() const {
    return m_popularity;
}