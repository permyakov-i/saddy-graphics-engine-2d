/*! \file gridactions.h

    Defines an actions for grids (sad::layouts::Grid)
 */
#pragma once
#include <QObject>

#include <input/controls.h>
#include "abstractactions.h"
#include "../childrenprovider.h"
#include "../layouts/layoutcelledit.h"

class MainPanel;

namespace gui
{

/*! A position of scene node in grids
 */
struct GridPosition
{
    sad::layouts::Grid* Grid; //!<  A grid
    size_t Row;               //!<  A row of related cell
    size_t Col;               //!<  A column of related cell
    size_t Pos;               //!<  A position of cell grid

    /*! Makes new grid position
        \param[in] g grid
        \param[in] row a row
        \param[in] col a column
        \param[in] pos a position
     */
    inline GridPosition(sad::layouts::Grid* g, size_t row, size_t col,  size_t pos)
    : Grid(g), Row(row), Col(col), Pos(pos)
    {
        
    }
};

namespace actions
{

/*! A group of actions, linked to grids (sad::layout::Grid)
 */
class GridActions: public QObject, public gui::actions::AbstractActions
{
Q_OBJECT
public:
    /*! Creates new label actions
        \param[in] parent a parent object
     */
    GridActions(QObject* parent = NULL);
    /*! Sets editor
        \param[in] e editor
     */
    virtual void setEditor(core::Editor* e);
    /*! This class could be inherited
     */
    virtual ~GridActions();
    /*! Returns selected grid, according to editor
        \param[in] grid a current grid
     */
    sad::layouts::Grid* selectedGrid() const;
    /*! Returns list of active grids
        \param[out] list a list of grids
     */
    static void activeGrids(sad::Vector<sad::layouts::Grid*>& grids);
    /*! Adds a grid to list of grids
        \param[in] grid a grid to be added
     */
    void addGridToGridList(sad::layouts::Grid* grid) const;
    /*! Removes last grid from list of grids, updating UI
     */
    void removeLastGrid();
    /*! Tries to insert child to curent grid, thus updating ui
        \param[in] g grid
        \param[in] row a row
        \param[in] col a column
        \param[in] pos a position
        \param[in] node a node
     */
    void insertChildToGrid(sad::layouts::Grid* g,  size_t row, size_t col, size_t pos, sad::SceneNode* node);
    /*! Tries to remove child from grid, affecting UI
        \param[in] g grid
        \param[in] row a row
        \param[in] col a column
        \param[in] pos a position
     */
    void removeChildFromGridInUI(sad::layouts::Grid* g, size_t row, size_t col, size_t pos);
    /*! Removes child from a grid, affecting UI. Note, that this does not affect history.
        \param[in] g grid
        \param[in] row a row
        \param[in] col a column
        \param[in] pos a position
     */
    void removeChildFromGrid(sad::layouts::Grid* g, size_t row, size_t col, size_t pos);
    /*! Inserts new cell editor
        \param[in] row a row
        \param[in] col a column
        \param[in] cell a cell editor
     */
    void insertCellEditor(size_t row,  size_t col, gui::layouts::LayoutCellEdit* cell);
    /*! Tries to update child name
        \param[in] g grid
        \param[in] row a row
        \param[in] col a column
        \param[in] pos a position
        \param[in] name a new name
     */
    void updateChildName(sad::layouts::Grid* g,  size_t row, size_t col, size_t pos, const QString& name);
    /*! Updates cell browser's UI from current vieewed grid
        \param[in] immediate whether it should be invoed now, or postponed via closure
     */
    void updateCellBrowser(bool immediate = false);
    /*! Updates region, when grid is picked
        \param[in] immediate whether it should be invoked now, or postponed via closure
     */
    void updateRegion(bool immediate = false);
    /*! Update current grid properties
        \param[in] immediate whether it should be invoked now, or postoponed
     */
    void updateGridPropertiesInUI(bool immediate = false);
    /*! Returns a cell editor
        \param[in] row a row
        \param[in] col a column
        \return a cell editor
     */
    gui::layouts::LayoutCellEdit* cellEditor(size_t row,  size_t col);
    /*! Seeks for related grids
        \param[in] node a node
        \return a grid position
     */
    static sad::Vector<gui::GridPosition> findRelatedGrids(sad::SceneNode* node);
    /*! Should clear browser for cells
     */
    void clearGridCellsBrowser();
    /*! Highlights adding state in main panel status box
     */
    void higlightAddingState() const;
    /*! Higlights state, when first point of grid should be placed.
        This should occur when user presses "Add By Stretching" button
     */
    void highlightPlaceFirstPointState() const;
    /*! Highlights state, when second point of grid should be placed.
        This should occur when user pressed "Add By Stretching" button
        and already placed top-left corner of screen
     */
    void highlightPlaceSecondPointState() const;
    /*! Highlights adding state in main panel status box
     */
    void higlightMovingState() const;
    /*! Highlights resizing state in main panel status box
     */
    void highlightResizingState() const;
    /*! Cancels adding grid, entering previous state
     */
    void cancelAddGrid();
    /*! Moves center of active or selected node to a mouse position.
        Note, that this will not preserve history, so use it only for
        active node.
        \param[in] e event
     */
    void moveByCenter(const sad::input::MouseMoveEvent& e);
    /*! Moves bottom-right corner of active grid to a mouse position.
        Note, that this will not preserve history, so use it only for
        active node.
        \param[in] e event
     */
    void moveByBottomRightCorner(const sad::input::MouseMoveEvent& e);
    /*! Removes grid from list of grids, flushing selection if need to
        \param[in] g grid
     */
    void removeGridFromGridList(sad::layouts::Grid* g);
    /*! Commits grid adding to a scene
        \param[in] e event
     */
    void commitGridAdd(const sad::input::MousePressEvent& e);
    /*! Commits picking top left point for a new grid, when
        picking "Add By Stretching"
        \param[in] e event
     */
    void commitTopLeftPoint(const sad::input::MousePressEvent& e);
    /*! Forces editor to enable editing area controls
     */
    void enableEditingAreaControls();
public slots:
    /*! Called, when user clicks on "Add" button for grids
     */
    void addGridClicked();
    /*! Called, when user clicks on "Add By Stretching" button for grids
     */
    void addGridByStretchingClicked();
    /*! Called, when user toggles show or hide grids
        \param[in] state a new state for a checkbox
     */
    void showGridsClicked(bool state);
    /*! Called, when cell width is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value for width
     */
    void cellWidthChanged(size_t row, size_t col, sad::layouts::LengthValue newvalue);
    /*! Called, when cell height is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value for width
     */
    void cellHeightChanged(size_t row, size_t col, sad::layouts::LengthValue newvalue);
    /*! Called, when cell horizontal alignment is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value for horizontal alignment
     */
    void cellHorizontalAlignmentChanged(size_t row, size_t col, sad::layouts::HorizontalAlignment newvalue);
    /*! Called, when cell vertical alignment is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value for vertical alignment
     */
    void cellVerticalAlignmentChanged(size_t row, size_t col, sad::layouts::VerticalAlignment newvalue);
    /*! Called, when cell stacking type is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value for stacking type
     */
    void cellStackingTypeChanged(size_t row, size_t col, sad::layouts::StackingType newvalue);
    /*! Called, when cell top padding is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value
     */
    void cellTopPaddingChanged(size_t row, size_t col, double newvalue);
    /*! Called, when cell bottom padding is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value
     */
    void cellBottomPaddingChanged(size_t row, size_t col, double newvalue);
    /*! Called, when cell left padding is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value
     */
    void cellLeftPaddingChanged(size_t row, size_t col, double newvalue);
    /*! Called, when cell right padding is changed
        \param[in] row a row
        \param[in] col a column
        \param[in] newvalue a new value
     */
    void cellRightPaddingChanged(size_t row, size_t col, double newvalue);
    /*! Called, when cell child is added
        \param[in] row a row
        \param[in] col a column
        \param[in] majorid a major id
        \param[in] nodename a name for a node
     */
    void cellChildAdded(size_t row, size_t col, unsigned long long majorid, QString nodename);
    /*! Called, when cell child is removed
        \param[in] row a row
        \param[in] col a column
        \param[in] pos a position, where child is added
     */
    void cellChildRemoved(size_t row, size_t col_t, size_t pos);
    /*! Called, when cell is clearead
        \param[in] row a row
        \param[in] col a column
     */
    void cellCleared(size_t row, size_t col);
    /*! Emitted, when cell children are swapped
        \param[in] row a row
        \param[in] col a column
        \param[in] pos1 a first position of item
        \param[in] pos2 a second position of item
     */
    void cellChildrenSwapped(size_t row, size_t col, size_t pos1, size_t pos2);
private:
    /*! Prepares grid for adding into form
        \return created grid
     */
    sad::layouts::Grid* prepareGridForAdding();
    /*! A provider for grid cells
     */
    gui::ChildrenProvider* m_provider;
    /*! A hash table from actions to local editors for provider
     */
    QHash<size_t, QHash<size_t, gui::layouts::LayoutCellEdit*> > m_cell_editors; 
    /*! A previous machine state for main editor's state machine. This is used
        to store previous state in case of multi-state transitions.
        "Add By Stretching" process is the only one now.
     */
    sad::String m_previous_machine_state;
    /*! Whether we adding a grid by stretching now. Must be used in case, if editor
        is in adding state for grids.
     */
    bool m_is_stretching;
};

}

}