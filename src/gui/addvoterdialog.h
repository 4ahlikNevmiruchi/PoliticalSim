#ifndef ADDVOTERDIALOG_H
#define ADDVOTERDIALOG_H

#include <QDialog>
#include "models/VoterModel.h"
#include "models/PartyModel.h"
#include "models/IdeologyModel.h"

namespace Ui {
class AddVoterDialog;
}

class AddVoterDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddVoterDialog(QWidget *parent = nullptr, PartyModel* partyModel = nullptr);

    ~AddVoterDialog();

    Voter getVoter() const;
    void setVoter(const Voter &voter);
    void setPartyOptions(const QVector<Party>& parties);

    int ideologyX() const;
    int ideologyY() const;

    void setIdeologyX(int x);
    void setIdeologyY(int y);

    void setIdeologyModel(const IdeologyModel* model);


private:
    Ui::AddVoterDialog *ui;
    QVector<Party> partyList;
    PartyModel* m_partyModel = nullptr;
    const IdeologyModel* m_ideologyModel = nullptr;
    int m_voterId = -1;

};

#endif // ADDVOTERDIALOG_H
