#ifndef ADDVOTERDIALOG_H
#define ADDVOTERDIALOG_H

#include <QDialog>
#include "models/VoterModel.h"
#include "models/PartyModel.h"

namespace Ui {
class AddVoterDialog;
}

class AddVoterDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddVoterDialog(QWidget *parent = nullptr, PartyModel *partyModel = nullptr);
    ~AddVoterDialog();

    Voter getVoter() const;
    void setVoter(const Voter &voter);

    void setPartyOptions(const QVector<Party>& parties);


private:
    Ui::AddVoterDialog *ui;
    QVector<Party> partyList;
};

#endif // ADDVOTERDIALOG_H
