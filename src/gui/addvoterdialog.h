#ifndef ADDVOTERDIALOG_H
#define ADDVOTERDIALOG_H

#include <QDialog>
#include "models/VoterModel.h"
#include "models/PartyModel.h"
#include "models/IdeologyModel.h"

namespace Ui {
class AddVoterDialog;
}

/**
 * @brief Dialog window for adding or editing a voter.
 *
 * Allows the user to enter a voter's name and ideological coordinates (X and Y). The dialog automatically determines and assigns the nearest matching party and ideology.
 */
class AddVoterDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs an AddVoterDialog.
     * @param parent Parent widget (optional).
     * @param partyModel Pointer to the PartyModel for finding the nearest party (optional).
     *
     * Initializes the dialog's UI components.
     */
    explicit AddVoterDialog(QWidget *parent = nullptr, PartyModel *partyModel = nullptr);

    /** @brief Destructor for AddVoterDialog. */
    ~AddVoterDialog();

    /**
     * @brief Returns the voter information entered in the dialog.
     * @return A Voter struct containing the entered name and coordinates, with the nearest party and ideology assigned.
     */
    Voter getVoter() const;

    /**
     * @brief Loads an existing voter's data into the dialog fields.
     * @param voter The Voter whose data will populate the form.
     */
    void setVoter(const Voter &voter);

    /**
     * @brief Provides the list of parties available for assignment.
     * @param parties A list of Party structures to use for determining the closest party.
     */
    void setPartyOptions(const QVector<Party>& parties);

    /**
     * @brief Returns the current X-axis ideology value from the dialog.
     * @return The X coordinate value.
     */
    int ideologyX() const;

    /**
     * @brief Returns the current Y-axis ideology value from the dialog.
     * @return The Y coordinate value.
     */
    int ideologyY() const;

    /**
     * @brief Sets the ideology X-axis field in the dialog.
     * @param x Value to set.
     */
    void setIdeologyX(int x);

    /**
     * @brief Sets the ideology Y-axis field in the dialog.
     * @param y Value to set.
     */
    void setIdeologyY(int y);

    /**
     * @brief Sets the IdeologyModel used for determining the nearest ideology.
     * @param model Pointer to the IdeologyModel to use for ideology lookups.
     */
    void setIdeologyModel(const IdeologyModel* model);

private:
    Ui::AddVoterDialog* ui;                                 ///< UI form instance.
    QVector<Party> partyList;                               ///< Cached party list used for party proximity matching.
    PartyModel* m_partyModel = nullptr;                     ///< Pointer to the PartyModel.
    const IdeologyModel* m_ideologyModel = nullptr;         ///< Pointer to the IdeologyModel.
    int m_voterId = -1;                                     ///< ID of the voter being edited; -1 if new.
};

#endif // ADDVOTERDIALOG_H
