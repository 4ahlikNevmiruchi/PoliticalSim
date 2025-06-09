#include "AddVoterDialog.h"
#include "ui_addvoterdialog.h"

namespace Ui {
class AddVoterDialog;
}

AddVoterDialog::AddVoterDialog(QWidget *parent, PartyModel *partyModel)
    : QDialog(parent),
    ui(new Ui::AddVoterDialog),
    m_partyModel(partyModel)
{
    ui->setupUi(this);

    if (m_partyModel) {
        for (int row = 0; row < m_partyModel->rowCount(); ++row) {
            QModelIndex index = m_partyModel->index(row, 0);
            QString partyName = m_partyModel->data(index, Qt::DisplayRole).toString();
            int partyId = m_partyModel->data(index, Qt::UserRole).toInt();

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
    v.ideologyX = ideologyX();
    v.ideologyY = ideologyY();
    v.partyId = ui->partyComboBox->currentData().toInt();
    return v;
}

void AddVoterDialog::setVoter(const Voter &voter) {
    m_voterId = voter.id;
    ui->nameEdit->setText(voter.name);
    ui->ideologyEdit->setText(voter.ideology);
    setIdeologyX(voter.ideologyX);
    setIdeologyY(voter.ideologyY);
    int index = ui->partyComboBox->findData(voter.partyId);
    if (index != -1)
        ui->partyComboBox->setCurrentIndex(index);
}

void AddVoterDialog::setPartyOptions(const QVector<Party>& parties) {
    partyList = parties;
    ui->partyComboBox->clear();

    for (const Party& p : parties) {
        ui->partyComboBox->addItem(p.name, p.id);
    }
}

int AddVoterDialog::ideologyX() const {
    return ui->ideologyXSpinBox->value();
}

int AddVoterDialog::ideologyY() const {
    return ui->ideologyYSpinBox->value();
}

void AddVoterDialog::setIdeologyX(int x) {
    ui->ideologyXSpinBox->setValue(x);
}

void AddVoterDialog::setIdeologyY(int y) {
    ui->ideologyYSpinBox->setValue(y);
}
