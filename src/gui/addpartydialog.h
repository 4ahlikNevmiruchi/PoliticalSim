#ifndef ADDPARTYDIALOG_H
#define ADDPARTYDIALOG_H

#include <QDialog>
#include "models/PartyModel.h"

namespace Ui {
class AddPartyDialog;
}

class AddPartyDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddPartyDialog(QWidget *parent = nullptr);
    ~AddPartyDialog();

    Party getParty() const;
    void setParty(const Party &party);
    int getPartyId() const;

    int ideologyX() const;
    int ideologyY() const;

    void setIdeologyX(int x);
    void setIdeologyY(int y);

private:
    Ui::AddPartyDialog *ui;
    int m_partyId = -1;
};

#endif // ADDPARTYDIALOG_H
