#include "VideoTableView.h"

VideoTableView::VideoTableView(QWidget *parent) :
    QTableView(parent)
{
    m_progressBarDelegate = new ProgressBarDelegate();
    if (m_progressBarDelegate) {
        setItemDelegateForColumn(VideoTableModel::ProgressColumn,
            m_progressBarDelegate);
    }

    setContextMenuPolicy(Qt::CustomContextMenu);
    m_contextMenu = new QMenu();
    QAction* deleteSelectedAction = m_contextMenu->addAction(tr("Delete"));
    deleteSelectedAction->setShortcut(QKeySequence::Delete);

    this->setColumnHidden(VideoTableModel::FileNameColumn, true);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(deleteSelectedAction, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));
}

void VideoTableView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Delete) && this->state() != QTableView::EditingState) {
        removeSelectedRows();
    } else {
        QTableView::keyPressEvent(event);
    }
}

void VideoTableView::mousePressEvent(QMouseEvent *event) {
    if ((event->button() == Qt::RightButton) && (this->selectedIndexes().size() > 1)) {
        emit customContextMenuRequested(event->pos());
    } else {
        QTableView::mousePressEvent(event);
    }
}

void VideoTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    if (current.column() != VideoTableModel::UrlColumn) {
        QModelIndex urlColumnIndex = this->model()->index(current.row(), VideoTableModel::UrlColumn);
        this->setCurrentIndex(urlColumnIndex);
        scrollTo(urlColumnIndex, QAbstractItemView::EnsureVisible);
    }
    QAbstractItemView::currentChanged(current, previous);
}

void VideoTableView::rowsRemoved(const QModelIndex &parent, int start, int end) {
    Q_UNUSED(parent)
    Q_UNUSED(end)
    int newActiveRowAfterRemoval = start;
    if (newActiveRowAfterRemoval >= this->model()->rowCount()) {
        newActiveRowAfterRemoval = this->model()->rowCount() - 1;
    }
    QModelIndex newActiveRow = this->model()->index(newActiveRowAfterRemoval, VideoTableModel::UrlColumn);
    this->setCurrentIndex(newActiveRow);
}

void VideoTableView::rowsInserted(const QModelIndex &parent, int start, int end) {
    QTableView::rowsInserted(parent, start, end);
    this->scrollToBottom();
}

void VideoTableView::scrollTo(const QModelIndex &index, ScrollHint hint) {
    int viewportHeight = this->viewport()->size().height();
    // assuming all rows have same height
    int numberOfFullyVisibleRows = qFloor((float)viewportHeight / (float)this->rowHeight(0));
    switch (hint) {
        case QAbstractItemView::EnsureVisible: {
            int bottomScrollValue = index.row() + 1 - numberOfFullyVisibleRows;
            if (bottomScrollValue < 0) {
                bottomScrollValue = 0;
            }
            int topScrollValue = index.row();
            if (topScrollValue < verticalScrollBar()->value()) {
                verticalScrollBar()->setValue(topScrollValue);
            } else if (bottomScrollValue > verticalScrollBar()->value()) {
                verticalScrollBar()->setValue(bottomScrollValue);
            }
        }
        break;
    default:
        QTableView::scrollTo(index, hint);
    }
}

void VideoTableView::showContextMenu(const QPoint &pos) {
    this->m_contextMenu->popup(this->mapToGlobal(pos));
}

void VideoTableView::removeSelectedRows() {
    // first get row numbers -- can be in any order
    QModelIndexList indexList = this->selectedIndexes();
    QListIterator<QModelIndex> listIterator(indexList);
    QList<int> rowsToBeRemoved;
    while (listIterator.hasNext()) {
        QModelIndex index = listIterator.next();
        int row = index.row();
        if (row < (this->model()->rowCount() - 1)) {
            rowsToBeRemoved << index.row();
        }
    }
    // remove rows starting from bottom to not change row numbers
    qSort(rowsToBeRemoved.begin(), rowsToBeRemoved.end());
    QListIterator<int> rowIterator(rowsToBeRemoved);
    rowIterator.toBack();
    while (rowIterator.hasPrevious()) {
        int row = rowIterator.previous();
        this->model()->removeRow(row);
    }
}
