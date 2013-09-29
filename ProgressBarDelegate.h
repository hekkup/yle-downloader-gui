#ifndef PROGRESSBARDELEGATE_H
#define PROGRESSBARDELEGATE_H

#include "VideoTableModel.h"

#include <QStyledItemDelegate>
#include <QStyleOptionProgressBar>
#include <QApplication>
#include <QPainter>

/**
 * @brief Progress bar delegate to be used in VideoTableView progress column.
 */
class ProgressBarDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ProgressBarDelegate(QObject *parent = 0);
    ~ProgressBarDelegate();

public slots:
    void paint(QPainter *painter, const QStyleOptionViewItem &viewItem, const QModelIndex &index) const;
};

#endif // PROGRESSBARDELEGATE_H
