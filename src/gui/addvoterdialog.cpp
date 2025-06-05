#include "AddVoterDialog.h"
#include "ui_addvoterdialog.h"

namespace Ui {
class AddVoterDialog;
}

AddVoterDialog::AddVoterDialog(QWidget *parent, PartyModel *partyModel)
    : QDialog(parent), ui(new Ui::AddVoterDialog)
{
    ui->setupUi(this);

    if (partyModel) {
        for (int row = 0; row < partyModel->rowCount(); ++row) {
            QModelIndex index = partyModel->index(row, 0);  // Name column
            QString partyName = partyModel->data(index, Qt::DisplayRole).toString();
            int partyId = partyModel->data(index, Qt::UserRole).toInt();
            ui->partyComboBox->addItem(partyName, partyId);
        }
    }
}

AddVoterDialog::~AddVoterDialog() {
    delete ui;
}

Voter AddVoterDialog::getVoter() const {
    Voter v;
    v.name = ui->nameEdit->text();
    v.ideology = ui->ideologyEdit->text();
    v.partyId = ui->partyComboBox->currentData().toInt();
    return v;
}

void AddVoterDialog::setVoter(const Voter &voter) {
    ui->nameEdit->setText(voter.name);
    ui->ideologyEdit->setText(voter.ideology);

    for (int i = 0; i < ui->partyComboBox->count(); ++i) {
        if (ui->partyComboBox->itemData(i).toInt() == voter.partyId) {
            ui->partyComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void AddVoterDialog::setPartyOptions(const QVector<Party>& parties) {
    partyList = parties;
    ui->partyComboBox->clear();

    for (const Party& p : parties) {
        ui->partyComboBox->addItem(p.name, p.id);  // Display name, store ID
    }
}
