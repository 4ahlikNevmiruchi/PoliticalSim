#include "AddPartyDialog.h"
#include "ui_AddPartyDialog.h"

#include <QDialog>
#include "models/PartyModel.h"

AddPartyDialog::AddPartyDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddPartyDialog) {
    ui->setupUi(this);
}

AddPartyDialog::~AddPartyDialog() {
    delete ui;
}

Party AddPartyDialog::getParty() const {
    return Party{
        m_partyId,
        ui->nameEdit->text(),
        ui->ideologyEdit->text(),
        ui->ideologyXSpinBox->value(),
        ui->ideologyYSpinBox->value()
        //ui->popularitySpin->value()
    };
}


void AddPartyDialog::setParty(const Party &party) {
    m_partyId = party.id;
    ui->nameEdit->setText(party.name);
    ui->ideologyEdit->setText(party.ideology);
    ui->ideologyXSpinBox->setValue(party.ideologyX);
    ui->ideologyYSpinBox->setValue(party.ideologyY);
    //ui->popularitySpin->setValue(party.popularity);
}

int AddPartyDialog::getPartyId() const {
    return m_partyId;
}

int AddPartyDialog::ideologyX() const {
    return ui->ideologyXSpinBox->value();
}

int AddPartyDialog::ideologyY() const {
    return ui->ideologyYSpinBox->value();
}

void AddPartyDialog::setIdeologyX(int x) {
    ui->ideologyXSpinBox->setValue(x);
}

void AddPartyDialog::setIdeologyY(int y) {
    ui->ideologyYSpinBox->setValue(y);
}
