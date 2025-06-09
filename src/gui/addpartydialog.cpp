#include "AddPartyDialog.h"
#include "IdeologyModel.h"
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
    int ideologyId = ui->ideologyComboBox->currentData().toInt();
    QString ideologyName = ui->ideologyComboBox->currentText();

    return Party{
        m_partyId,
        ui->nameEdit->text(),
        ideologyId,
        ideologyName,
        ui->ideologyXSpinBox->value(),
        ui->ideologyYSpinBox->value()
    };
}


void AddPartyDialog::setParty(const Party &party) {
    m_partyId = party.id;
    ui->nameEdit->setText(party.name);
    ui->ideologyXSpinBox->setValue(party.ideologyX);
    ui->ideologyYSpinBox->setValue(party.ideologyY);

    // Match ideologyId in the comboBox
    int index = ui->ideologyComboBox->findData(party.ideologyId);
    if (index != -1) {
        ui->ideologyComboBox->setCurrentIndex(index);
    }
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

void AddPartyDialog::setIdeologyModel(const IdeologyModel* model) {
    m_ideologyModel = model;
    ui->ideologyComboBox->clear();

    if (!model) return;

    for (const Ideology& i : model->getIdeologies()) {
        ui->ideologyComboBox->addItem(i.name, i.id); // text, user data
    }
}
