#include "AddPartyDialog.h"
#include "ui_AddPartyDialog.h"

AddPartyDialog::AddPartyDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddPartyDialog) {
    ui->setupUi(this);
}

AddPartyDialog::~AddPartyDialog() {
    delete ui;
}

Party AddPartyDialog::getParty() const {
    return Party{
        ui->nameEdit->text(),
        ui->ideologyEdit->text(),
        ui->popularitySpin->value()
    };
}
