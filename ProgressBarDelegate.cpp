#include "ProgressBarDelegate.h"

ProgressBarDelegate::ProgressBarDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

ProgressBarDelegate::~ProgressBarDelegate() {
}

void ProgressBarDelegate::paint(QPainter *painter,
    const QStyleOptionViewItem &viewItem, const QModelIndex &index) const
{
    // only draw real video entries
    if (index.data().isValid()) {
        QStyleOptionProgressBar progressBar;
        progressBar.rect = viewItem.rect;
        progressBar.minimum = index.data(VideoTableModel::ProgressMinimumRole).toInt();
        progressBar.maximum = index.data(VideoTableModel::ProgressMaximumRole).toInt();
        int progress = index.data(Qt::DisplayRole).toInt();
        if ((progress >= 0) && (progress <= 100)) {
            progressBar.progress = progress;
        } else if (progress < 0) {
            progressBar.progress = 0;
        } else if (progress > 100) {
            progressBar.progress = 100;
        }
        progressBar.text = index.data(VideoTableModel::ProgressTextRole).toString();
        //progressBar.text = QString::number(progressBar.progress) + "%";
        progressBar.textVisible = true;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
            &progressBar, painter);
    }
}
