#ifndef ADDPARTYDIALOG_H
#define ADDPARTYDIALOG_H

#include <QDialog>
#include "models/PartyModel.h"
#include "models/IdeologyModel.h"

namespace Ui {
class AddPartyDialog;
}

/**
 * @brief Dialog window for adding or editing a political party.
 *
 * Allows the user to input a party name and an ideological position (X and Y coordinates on a political compass). Based on the entered coordinates, it automatically selects the nearest matching ideology.
 */
class AddPartyDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs an AddPartyDialog.
     * @param parent Parent widget (optional).
     *
     * Initializes the dialog's UI components.
     */
    explicit AddPartyDialog(QWidget *parent = nullptr);

    /** @brief Destructor for AddPartyDialog. */
    ~AddPartyDialog();

    /**
     * @brief Returns the party information entered in the dialog.
     * @return A Party struct containing the entered name and coordinates, with the nearest ideology's ID and name set automatically.
     */
    Party getParty() const;

    /**
     * @brief Loads an existing party's data into the dialog fields.
     * @param party The Party to populate into the form (for editing).
     */
    void setParty(const Party &party);

    /**
     * @brief Retrieves the ID of the party being edited.
     * @return The party's ID, or -1 if the dialog is for a new party.
     */
    int getPartyId() const;

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
     * @param x The economic axis coordinate to set.
     */
    void setIdeologyX(int x);

    /**
     * @brief Sets the ideology Y-axis field in the dialog.
     * @param y The social axis coordinate to set.
     */
    void setIdeologyY(int y);

    /**
     * @brief Sets the IdeologyModel used for determining the nearest ideology.
     * @param model Pointer to the IdeologyModel for ideology lookups.
     *
     * The ideology model provides data to compute the closest matching ideology based on the dialog's coordinates.
     */
    void setIdeologyModel(const IdeologyModel* model);

private:
    Ui::AddPartyDialog* ui;                                         ///< Pointer to the UI form instance.
    int m_partyId = -1;                                             ///< ID of the party being edited; -1 if new.
    const IdeologyModel* m_ideologyModel = nullptr;                 ///< Pointer to the IdeologyModel used to calculate the nearest ideology.
};

#endif // ADDPARTYDIALOG_H
