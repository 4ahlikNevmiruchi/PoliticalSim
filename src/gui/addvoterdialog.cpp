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
    }
    ui->partyComboBox->setVisible(false);

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

AddVoterDialog::~AddVoterDialog() {
    delete ui;
}

Voter AddVoterDialog::getVoter() const {
    Voter v;
    v.name = ui->nameEdit->text();
    // Automatically determine closest ideology and party
    if (m_ideologyModel) {
        v.ideologyId = m_ideologyModel->findClosestIdeologyId(ideologyX(), ideologyY());
        v.ideology = m_ideologyModel->getIdeologyNameById(v.ideologyId);
    } else {
        v.ideologyId = ui->ideologyComboBox->currentData().toInt();
        v.ideology = ui->ideologyComboBox->currentText();
    }
    v.ideologyX = ideologyX();
    v.ideologyY = ideologyY();
    if (m_partyModel) {
        int closestPartyId = -1;
        double minDistance = std::numeric_limits<double>::max();
        for (const Party& p : m_partyModel->getAllParties()) {
            double dx = p.ideologyX - v.ideologyX;
            double dy = p.ideologyY - v.ideologyY;
            double dist = dx * dx + dy * dy;
            if (dist < minDistance) {
                minDistance = dist;
                closestPartyId = p.id;
            }
        }
        v.partyId = closestPartyId;
        v.partyName = (closestPartyId != -1
                           ? m_partyModel->getPartyAt(m_partyModel->getAllParties().indexOf(
                                                          *std::find_if(m_partyModel->getAllParties().begin(), m_partyModel->getAllParties().end(),
                                                                        [closestPartyId](const Party& p){ return p.id == closestPartyId; })))
                                 .name
                           : QString());  // find party name for completeness
    } else {
        v.partyId = -1;
        v.partyName.clear();
    }
    return v;
}

void AddVoterDialog::setVoter(const Voter &voter) {
    m_voterId = voter.id;
    ui->nameEdit->setText(voter.name);
    setIdeologyX(voter.ideologyX);
    setIdeologyY(voter.ideologyY);

    // Set ideology combo from id (show stored ideology)
    int ideologyIndex = ui->ideologyComboBox->findData(voter.ideologyId);
    if (ideologyIndex != -1)
        ui->ideologyComboBox->setCurrentIndex(ideologyIndex);

    // int partyIndex = ui->partyComboBox->findData(voter.partyId);
    // if (partyIndex != -1)
    //     ui->partyComboBox->setCurrentIndex(partyIndex);
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

void AddVoterDialog::setIdeologyModel(const IdeologyModel* model) {
    m_ideologyModel = model;
    ui->ideologyComboBox->clear();

    if (!model) return;

    for (const Ideology& i : model->getIdeologies()) {
        ui->ideologyComboBox->addItem(i.name, i.id);
    }
    // Select nearest ideology based on current coordinates
    int curId = model->findClosestIdeologyId(ui->ideologyXSpinBox->value(),
                                             ui->ideologyYSpinBox->value());
    int idx = ui->ideologyComboBox->findData(curId);
    if (idx != -1) ui->ideologyComboBox->setCurrentIndex(idx);
}
