#include "VideoTableView.h"

VideoTableView::VideoTableView(QWidget *parent) :
    QTableView(parent)
{
    m_currentlyDownloadingRow = -1;
    m_progressBarDelegate = new ProgressBarDelegate();
    if (m_progressBarDelegate) {
        setItemDelegateForColumn(VideoTableModel::ProgressColumn,
            m_progressBarDelegate);
    }

    setContextMenuPolicy(Qt::CustomContextMenu);
    m_contextMenu = new QMenu();
    m_deleteSelectedAction = m_contextMenu->addAction(tr("Delete"));
    m_deleteSelectedAction->setShortcut(QKeySequence::Delete);
    m_selectAllAction = m_contextMenu->addAction(tr("Select all"));
    m_selectAllAction->setShortcut(QKeySequence::SelectAll);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_deleteSelectedAction, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));
    connect(m_selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
}

void VideoTableView::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Delete) && (this->state() != QTableView::EditingState)) {
        removeSelectedRows();
    } else if (event->matches(QKeySequence::InsertParagraphSeparator) && (this->state() != QTableView::EditingState)) {
        QModelIndex urlIndex = this->model()->index(this->currentIndex().row(), VideoTableModel::UrlColumn);
        this->edit(urlIndex);
    } else {
        QTableView::keyPressEvent(event);
    }
}

void VideoTableView::mousePressEvent(QMouseEvent *event) {
    if ((event->button() == Qt::RightButton) && (this->selectedIndexes().size() > 1)) {
        emit customContextMenuRequested(event->pos());
    } else if (event->button() == Qt::LeftButton) {
        QModelIndex clickedIndex = this->indexAt(event->pos());
        if (clickedIndex.row() == (this->model()->rowCount() - 1)) {
            QModelIndex urlIndex;
            if (clickedIndex.column() != VideoTableModel::UrlColumn) {
                urlIndex = this->model()->index(clickedIndex.row(), VideoTableModel::UrlColumn);
            } else {
                urlIndex = clickedIndex;
            }
            this->clearSelection();
            this->edit(urlIndex);
        } else {
            QTableView::mousePressEvent(event);
        }
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
    //
    // Enable delete rows item in context menu IF
    //   1. one row is selected AND it's not downloading
    //   2. some rows are selected AND at least one of them is not downloading
    //
    // while showing menu: check again when currently downloading row changes
    //
    QModelIndex currentlyLoadingIndex = this->model()->index(m_currentlyDownloadingRow, VideoTableModel::UrlColumn);

    m_deleteSelectedAction->setEnabled(false);

    // one row is selected
    if (this->selectedIndexes().count() == 1) {
        // enable delete action if row is not loading
        // AND the row is not the last row
        if ((currentlyLoadingIndex.row() != this->selectedIndexes().first().row()) &&
                (this->selectedIndexes().first().row() != (this->model()->rowCount() - 1)))
        {
            m_deleteSelectedAction->setEnabled(true);
        }

    }

    // many rows selected
    if (this->selectedIndexes().count() > 1) {
        // enable delete action always (just delete not downloading rows)
        m_deleteSelectedAction->setEnabled(true);
    }

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
        QModelIndex stateIndex = this->model()->index(row, VideoTableModel::StatusColumn);
        VideoInfo::VideoState state = (VideoInfo::VideoState)this->model()->data(stateIndex, Qt::UserRole).toInt();
        // allow removing if not downloading
        if ((row < ((VideoTableModel*)this->model())->videoCount()) &&
            (VideoInfo::StateStarting != state) &&
            (VideoInfo::StateLoading != state))
        {
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

// slot
void VideoTableView::changeDownloadingRow(int currentRow) {
    this->m_currentlyDownloadingRow = currentRow;
    //qDebug() << "VideoTableView:: current row changed to " + QString::number(currentRow);
    if (this->m_contextMenu->isVisible()) {
        // if one row is selected and it is the same as the current row, disable delete
        if (this->selectedIndexes().count() == 1) {
            if (this->selectedIndexes().first().row() == m_currentlyDownloadingRow) {
                m_deleteSelectedAction->setEnabled(false);
            } else {
                m_deleteSelectedAction->setEnabled(true);
            }
        }
        // if many rows are selected, don't disable delete
    }
}
