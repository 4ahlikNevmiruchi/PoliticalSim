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

private:
    Ui::AddPartyDialog *ui;
};

#endif // ADDPARTYDIALOG_H
