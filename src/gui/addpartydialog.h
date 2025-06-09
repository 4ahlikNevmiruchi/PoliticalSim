#ifndef ADDPARTYDIALOG_H
#define ADDPARTYDIALOG_H

#include <QDialog>
#include "models/PartyModel.h"
#include "models/IdeologyModel.h"

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

    void setIdeologyModel(const IdeologyModel* model);

private:
    Ui::AddPartyDialog *ui;
    int m_partyId = -1;
    const IdeologyModel* m_ideologyModel = nullptr;
};

#endif // ADDPARTYDIALOG_H
