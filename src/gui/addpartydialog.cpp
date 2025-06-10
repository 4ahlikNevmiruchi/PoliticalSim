#include "AddPartyDialog.h"
#include "IdeologyModel.h"
#include "ui_AddPartyDialog.h"

#include <QDialog>
#include "models/PartyModel.h"

AddPartyDialog::AddPartyDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddPartyDialog) {
    ui->setupUi(this);

    // Connect ideology coordinate spin boxes to update nearest ideology selection
    connect(ui->ideologyXSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        if (!m_ideologyModel) return;
        int closestId = m_ideologyModel->findClosestIdeologyId(ui->ideologyXSpinBox->value(),
                                                               ui->ideologyYSpinBox->value());
        int idx = ui->ideologyComboBox->findData(closestId);
        if (idx != -1) ui->ideologyComboBox->setCurrentIndex(idx);
    });
    connect(ui->ideologyYSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
        if (!m_ideologyModel) return;
        int closestId = m_ideologyModel->findClosestIdeologyId(ui->ideologyXSpinBox->value(),
                                                               ui->ideologyYSpinBox->value());
        int idx = ui->ideologyComboBox->findData(closestId);
        if (idx != -1) ui->ideologyComboBox->setCurrentIndex(idx);
    });
    ui->ideologyComboBox->setEnabled(false); //ideology selection is read-only
}

AddPartyDialog::~AddPartyDialog() {
    delete ui;
}

Party AddPartyDialog::getParty() const {
    // Determine closest ideology based on coordinates
    int ideologyId = -1;
    QString ideologyName;
    if (m_ideologyModel) {
        ideologyId = m_ideologyModel->findClosestIdeologyId(ui->ideologyXSpinBox->value(),
                                                            ui->ideologyYSpinBox->value());
        ideologyName = m_ideologyModel->getIdeologyNameById(ideologyId);
    } else {
        ideologyId = ui->ideologyComboBox->currentData().toInt();
        ideologyName = ui->ideologyComboBox->currentText();
    }
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
    // Select nearest ideology for current coordinates
    int curId = model->findClosestIdeologyId(ui->ideologyXSpinBox->value(),
                                             ui->ideologyYSpinBox->value());
    int idx = ui->ideologyComboBox->findData(curId);
    if (idx != -1) ui->ideologyComboBox->setCurrentIndex(idx);
}
