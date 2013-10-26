#ifndef VIDEOTABLEVIEW_H
#define VIDEOTABLEVIEW_H

#include "VideoTableModel.h"
#include "ProgressBarDelegate.h"

#include <QTableView>
#include <QKeyEvent>
#include <QVector>
#include <QMenu>
#include <QAction>
#include <QScrollBar>
#include <QtCore/qmath.h>

/**
 * @brief Video table view
 */
class VideoTableView : public QTableView
{
    Q_OBJECT
public:
    explicit VideoTableView(QWidget *parent = 0);
    
    /**
     * Reimplementation: when delete key pressed, remove current line or
     * selected lines
     * @overload QWidget::keyPressEvent()
     */
    void keyPressEvent(QKeyEvent *event);

    /**
     * If more than one item selected when right button clicked, don't
     * remove selection
     * @overload QWidget::mousePressEvent()
     */
    void mousePressEvent(QMouseEvent *event);

public slots:

    /**
     * Don't let user select to other than URL column
     * @overload QAbstractItemView::currentChanged()
     */
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

    /**
     * Select the next possible item when rows removed
     * @overload QTableView::rowsRemoved()
     */
    void rowsRemoved(const QModelIndex &parent, int start, int end);

    /**
     * Scroll to the last item when new item inserted.
     * Assuming no rows will be inserted to the middle of the list/table.
     * @overload QTableView::rowsInserted()
     */
    void rowsInserted(const QModelIndex &parent, int start, int end);

    /**
     * Scroll to index. Currently supporting only QAbstractItemView::EnsureVisible
     */
    void scrollTo(const QModelIndex &index, ScrollHint hint);

    /**
     * Show context menu at given point (the point is relative to event receiving
     * widget)
     */
    void showContextMenu(const QPoint &pos);

    /**
     * Remove selected rows
     */
    void removeSelectedRows();

    /**
     * Change currently downloading row
     * @param currentRow currently downloading row
     */
    void changeDownloadingRow(int currentRow);

private:
    ProgressBarDelegate* m_progressBarDelegate; ///< delegate for progress column
    QMenu* m_contextMenu; ///< table context menu
    QAction* m_deleteSelectedAction; ///< delete selected row action in m_contextMenu
    QAction* m_selectAllAction; ///< select all rows action in m_contextMenu
    int m_currentlyDownloadingRow;  ///< currently downloading row; -1 if not downloading
};

#endif // VIDEOTABLEVIEW_H
