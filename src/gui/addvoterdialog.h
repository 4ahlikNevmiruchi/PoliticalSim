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

private:
    Ui::AddVoterDialog *ui;
};

#endif // ADDVOTERDIALOG_H
