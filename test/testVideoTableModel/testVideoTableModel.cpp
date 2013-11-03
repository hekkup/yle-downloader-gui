

//#include <QtCore>
//#include <QtGui>
#include <QtCore/QString>
#include <QtTest/QtTest>
//#include <QAbstractTableModel>
//#include <QAbstractItemModel>

#include "VideoTableModel.h"
#include "VideoInfo.h"

/**
 * @brief VideoTableModel unit test
 * @todo test QVariant conversion checks in setData() method
 * @todo test with negative row numbers
 * @todo test dataChanged signal emitted when should
 */
class TestVideoTableModel : public QObject
{
    Q_OBJECT
    
public:
    TestVideoTableModel();

public slots:

    // slots to verify the required signals have been emitted
    void rowsAboutToBeInsertedHandler(const QModelIndex &index, int start, int end);
    void rowsInsertedHandler(const QModelIndex &index, int start, int end);
    void rowsAboutToBeRemovedHandler(const QModelIndex &index, int start, int end);
    void rowsRemovedHandler(const QModelIndex &index, int start, int end);
    void dataChangedHandler(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void constructor();
    void flags();
    void videoAt();
    void videoCount();
    void rowCount();
    void columnCount();
    void headerData();
    void data();
    void setData();
    void insertRows();
    void removeRows();
    void tryToRemoveTooManyRowsFromMiddle();

    void addUrl();
    void url();
    void setKnownDownloadProgress();
    void setUnknownDownloadProgress();
    void downloadProgress();
    void setDownloadState();
    void setVideoFileName();

private:
    VideoTableModel* m_videoTableModel;
    QList<VideoInfo*> m_testVideos;

    // these members are used to verify the correct methods were called
    // during row removing/inserting in subclass implementation
    bool m_rowsAboutToBeInsertedCalled;
    bool m_rowsInsertedCalled;
    bool m_rowsAboutToBeRemovedCalled;
    bool m_rowsRemovedCalled;
    bool m_dataChangedEmitted;
};

TestVideoTableModel::TestVideoTableModel()
{
    m_videoTableModel = NULL;
}

void TestVideoTableModel::rowsAboutToBeInsertedHandler(const QModelIndex &index, int start, int end) {
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->m_rowsAboutToBeInsertedCalled = true;
}

void TestVideoTableModel::rowsInsertedHandler(const QModelIndex &index, int start, int end) {
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->m_rowsInsertedCalled = true;
}

void TestVideoTableModel::rowsAboutToBeRemovedHandler(const QModelIndex &index, int start, int end) {
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->m_rowsAboutToBeRemovedCalled = true;
}

void TestVideoTableModel::rowsRemovedHandler(const QModelIndex &index, int start, int end) {
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->m_rowsRemovedCalled = true;
}

void TestVideoTableModel::dataChangedHandler(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    this->m_dataChangedEmitted = true;
}

void TestVideoTableModel::initTestCase() {
    QVERIFY(NULL == m_videoTableModel);
}

void TestVideoTableModel::cleanupTestCase() {
}

void TestVideoTableModel::init() {
    m_videoTableModel = new VideoTableModel();
    QVERIFY(NULL != m_videoTableModel);

    VideoInfo* v1 = new VideoInfo();
    v1->setUrl(QString("http://localhost/1"));
    v1->setProgress(10);
    v1->setState(VideoInfo::StateLoading);
    v1->setFileName(QString("video1.flv"));
    m_videoTableModel->m_videos.append(v1);

    v1 = new VideoInfo();
    v1->setUrl(QString("http://localhost/2"));
    v1->setProgress(20);
    v1->setState(VideoInfo::StateLoadedOk);
    v1->setFileName(QString("video2.flv"));
    m_videoTableModel->m_videos.append(v1);

    QVERIFY(m_videoTableModel->m_videos.size() == 2);

    connect(m_videoTableModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)),
            this, SLOT(rowsAboutToBeInsertedHandler(QModelIndex, int, int)));
    connect(m_videoTableModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
            this, SLOT(rowsInsertedHandler(QModelIndex, int, int)));
    connect(m_videoTableModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
            this, SLOT(rowsAboutToBeRemovedHandler(QModelIndex, int, int)));
    connect(m_videoTableModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
            this, SLOT(rowsRemovedHandler(QModelIndex, int, int)));
    connect(m_videoTableModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(dataChangedHandler(QModelIndex,QModelIndex)));
}

void TestVideoTableModel::cleanup() {
    m_videoTableModel->disconnect();
    delete m_videoTableModel;
}

void TestVideoTableModel::constructor() {
    QVERIFY(NULL != m_videoTableModel);
    delete m_videoTableModel;

    m_videoTableModel = new VideoTableModel();
    QVERIFY(0 == m_videoTableModel->m_videos.size());

    QVERIFY(m_videoTableModel->m_tableHeaderStrings[VideoTableModel::UrlColumn] == "URL");
    QVERIFY(m_videoTableModel->m_tableHeaderStrings[VideoTableModel::ProgressColumn] == "Progress");
    QVERIFY(m_videoTableModel->m_tableHeaderStrings[VideoTableModel::StatusColumn] == "Status");
    QVERIFY(m_videoTableModel->m_addUrlString == "<add URL>");
}

void TestVideoTableModel::flags() {
    QModelIndex index;
    for (int state=0; state < VideoInfo::STATE_COUNT; state++) {
        m_videoTableModel->m_videos.at(0)->setState((VideoInfo::VideoState)state);

        if ((state == VideoInfo::StateNotStarted) || (state == VideoInfo::StateFailed)) {
            index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
            QVERIFY((Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable) == m_videoTableModel->flags(index));
            index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
            QVERIFY(Qt::ItemIsEnabled == m_videoTableModel->flags(index));
            index = m_videoTableModel->index(0, VideoTableModel::StatusColumn);
            QVERIFY(Qt::ItemIsEnabled == m_videoTableModel->flags(index));
        } else {
            index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
            QVERIFY((Qt::ItemIsEnabled | Qt::ItemIsSelectable) == m_videoTableModel->flags(index));
            index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
            QVERIFY(Qt::ItemIsEnabled == m_videoTableModel->flags(index));
            index = m_videoTableModel->index(0, VideoTableModel::StatusColumn);
            QVERIFY(Qt::ItemIsEnabled == m_videoTableModel->flags(index));
        }
    }
}

void TestVideoTableModel::videoAt() {
    for (int row = 0; row < 2; row++) {
        QVERIFY(m_videoTableModel->m_videos.at(row) == m_videoTableModel->videoAt(row));
    }
    QVERIFY(NULL == m_videoTableModel->videoAt(2));
    QVERIFY(NULL == m_videoTableModel->videoAt(-1));
}

void TestVideoTableModel::videoCount() {
    QVERIFY(m_videoTableModel->m_videos.size() == 2);
    QVERIFY(m_videoTableModel->videoCount() == 2);

    delete m_videoTableModel->m_videos.takeLast();
    QVERIFY(m_videoTableModel->m_videos.size() == 1);
    QVERIFY(m_videoTableModel->videoCount() == 1);

    delete m_videoTableModel->m_videos.takeLast();
    QVERIFY(m_videoTableModel->m_videos.size() == 0);
    QVERIFY(m_videoTableModel->videoCount() == 0);
}

void TestVideoTableModel::rowCount() {
    QVERIFY(m_videoTableModel->m_videos.size() == 2);
    QVERIFY(m_videoTableModel->rowCount() == 3);

    delete m_videoTableModel->m_videos.takeLast();
    QVERIFY(m_videoTableModel->m_videos.size() == 1);
    QVERIFY(m_videoTableModel->rowCount() == 2);

    delete m_videoTableModel->m_videos.takeLast();
    QVERIFY(m_videoTableModel->m_videos.size() == 0);
    QVERIFY(m_videoTableModel->rowCount() == 1);
}

void TestVideoTableModel::columnCount() {
    QVERIFY(m_videoTableModel->columnCount() == VideoTableModel::COLUMN_COUNT);
}

void TestVideoTableModel::headerData() {
    QVERIFY(m_videoTableModel->m_tableHeaderStrings[VideoTableModel::UrlColumn] ==
            m_videoTableModel->headerData(VideoTableModel::UrlColumn, Qt::Horizontal, Qt::DisplayRole).toString());
    QVERIFY(m_videoTableModel->m_tableHeaderStrings[VideoTableModel::ProgressColumn] ==
            m_videoTableModel->headerData(VideoTableModel::ProgressColumn, Qt::Horizontal, Qt::DisplayRole).toString());
    QVERIFY(m_videoTableModel->m_tableHeaderStrings[VideoTableModel::StatusColumn] ==
            m_videoTableModel->headerData(VideoTableModel::StatusColumn, Qt::Horizontal, Qt::DisplayRole).toString());
    QVERIFY(!m_videoTableModel->headerData(VideoTableModel::COLUMN_COUNT, Qt::Horizontal, Qt::DisplayRole).isValid());
}

void TestVideoTableModel::data() {
    QModelIndex index;
    QVariant data;
    VideoInfo videoInfo;

    // Qt::DisplayRole

    index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toString() == "http://localhost/1");
    index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toInt() == 10);
    index = m_videoTableModel->index(0, VideoTableModel::StatusColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toString() == videoInfo.stateString(VideoInfo::StateLoading));
    index = m_videoTableModel->index(0, VideoTableModel::FileNameColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toString() == "video1.flv");

    index = m_videoTableModel->index(1, VideoTableModel::UrlColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toString() == "http://localhost/2");
    index = m_videoTableModel->index(1, VideoTableModel::ProgressColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toInt() == 20);
    index = m_videoTableModel->index(1, VideoTableModel::StatusColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toString() == videoInfo.stateString(VideoInfo::StateLoadedOk));
    index = m_videoTableModel->index(1, VideoTableModel::FileNameColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toString() == "video2.flv");

    index = m_videoTableModel->index(2, VideoTableModel::UrlColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(data.toString() == "<add URL>");
    index = m_videoTableModel->index(2, VideoTableModel::ProgressColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::StatusColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::FileNameColumn);
    data = index.data(Qt::DisplayRole);
    QVERIFY(!data.isValid());

    // Qt::EditRole

    index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(data.toString() == "http://localhost/1");
    index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(0, VideoTableModel::StatusColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(0, VideoTableModel::FileNameColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());

    index = m_videoTableModel->index(1, VideoTableModel::UrlColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(data.toString() == "http://localhost/2");
    index = m_videoTableModel->index(1, VideoTableModel::ProgressColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(1, VideoTableModel::StatusColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(1, VideoTableModel::FileNameColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());

    index = m_videoTableModel->index(2, VideoTableModel::UrlColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::ProgressColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::StatusColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::FileNameColumn);
    data = index.data(Qt::EditRole);
    QVERIFY(!data.isValid());

    // Qt::UserRole

    index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(data.toInt() == 10);
    index = m_videoTableModel->index(0, VideoTableModel::StatusColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(data.toInt() == (int)VideoInfo::StateLoading);
    index = m_videoTableModel->index(0, VideoTableModel::FileNameColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());

    index = m_videoTableModel->index(1, VideoTableModel::UrlColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(1, VideoTableModel::ProgressColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(data.toInt() == 20);
    index = m_videoTableModel->index(1, VideoTableModel::StatusColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(data.toInt() == (int)VideoInfo::StateLoadedOk);
    index = m_videoTableModel->index(1, VideoTableModel::FileNameColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());

    index = m_videoTableModel->index(2, VideoTableModel::UrlColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::ProgressColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::StatusColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());
    index = m_videoTableModel->index(2, VideoTableModel::FileNameColumn);
    data = index.data(Qt::UserRole);
    QVERIFY(!data.isValid());

    // VideoTableModel::ProgressMinimumRole, ProgressMaximumRole
    // and ProgressTextRole
    index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
    QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());
    QVERIFY(m_videoTableModel->setData(index, QVariant(10), VideoTableModel::ProgressMinimumRole));
    QVERIFY(10 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());

    QVERIFY(100 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());
    QVERIFY(m_videoTableModel->setData(index, QVariant(90), VideoTableModel::ProgressMaximumRole));
    QVERIFY(90 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());

    QVERIFY("" == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).toString());
    QVERIFY(m_videoTableModel->setData(index, QVariant("50%"), VideoTableModel::ProgressTextRole));
    QVERIFY("50%" == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole));

    index = m_videoTableModel->index(2, VideoTableModel::ProgressColumn);
    QVERIFY(false == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).isValid());
    QVERIFY(false == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).isValid());
    QVERIFY(false == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).isValid());

    // Qt::ToolTipRole
    index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
    QVERIFY("Video file name\nvideo1.flv" == m_videoTableModel->data(index, Qt::ToolTipRole).toString());
    index = m_videoTableModel->index(1, VideoTableModel::UrlColumn);
    QVERIFY("Video file name\nvideo2.flv" == m_videoTableModel->data(index, Qt::ToolTipRole).toString());
    index = m_videoTableModel->index(2, VideoTableModel::UrlColumn);
    QVERIFY(false == m_videoTableModel->data(index, Qt::ToolTipRole).isValid());
}

void TestVideoTableModel::setData() {
    QModelIndex index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
    QVERIFY(m_videoTableModel->setData(index, QVariant("hello"), Qt::EditRole));
    QVERIFY("hello" == m_videoTableModel->m_videos.at(0)->url());

    index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant(50), Qt::EditRole));
    QVERIFY(10 == m_videoTableModel->m_videos.at(0)->progress());
    QVERIFY(m_videoTableModel->setData(index, QVariant(50), Qt::UserRole));
    QVERIFY(50 == m_videoTableModel->m_videos.at(0)->progress());

    index = m_videoTableModel->index(0, VideoTableModel::StatusColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant(VideoInfo::StateCanceled), Qt::EditRole));
    QVERIFY(VideoInfo::StateLoading == m_videoTableModel->m_videos.at(0)->state());
    QVERIFY(m_videoTableModel->setData(index, QVariant(VideoInfo::StateCanceled), Qt::UserRole));
    QVERIFY(VideoInfo::StateCanceled == m_videoTableModel->m_videos.at(0)->state());

    index = m_videoTableModel->index(0, VideoTableModel::FileNameColumn);
    QVERIFY(m_videoTableModel->setData(index, QVariant("hei.mpg"), Qt::EditRole));
    QVERIFY("hei.mpg" == m_videoTableModel->m_videos.at(0)->fileName());

    // try setting invalid QVariant: data shouldn't change
    index = m_videoTableModel->index(0, VideoTableModel::UrlColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant::Invalid, Qt::EditRole));
    QVERIFY("hello" == m_videoTableModel->m_videos.at(0)->url());

    index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant::Invalid, Qt::UserRole));
    QVERIFY(50 == m_videoTableModel->m_videos.at(0)->progress());

    index = m_videoTableModel->index(0, VideoTableModel::StatusColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant::Invalid, Qt::UserRole));
    QVERIFY(VideoInfo::StateCanceled == m_videoTableModel->m_videos.at(0)->state());

    index = m_videoTableModel->index(0, VideoTableModel::FileNameColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant::Invalid, Qt::EditRole));
    QVERIFY("hei.mpg" == m_videoTableModel->m_videos.at(0)->fileName());

    // try setting into the empty placeholder row: should add when
    // setting new URL, not in other cases
    m_rowsAboutToBeInsertedCalled = false;
    m_rowsInsertedCalled = false;
    index = m_videoTableModel->index(2, VideoTableModel::StatusColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant(VideoInfo::StateNotStarted), Qt::UserRole));
    QVERIFY(false == m_rowsAboutToBeInsertedCalled);
    QVERIFY(false == m_rowsInsertedCalled);

    index = m_videoTableModel->index(2, VideoTableModel::ProgressColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant(3), Qt::UserRole));
    QVERIFY(false == m_rowsAboutToBeInsertedCalled);
    QVERIFY(false == m_rowsInsertedCalled);

    index = m_videoTableModel->index(2, VideoTableModel::FileNameColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant("heippa.flv"), Qt::EditRole));
    QVERIFY(false == m_rowsAboutToBeInsertedCalled);
    QVERIFY(false == m_rowsInsertedCalled);

    index = m_videoTableModel->index(2, VideoTableModel::UrlColumn);
    QVERIFY(m_videoTableModel->setData(index, QVariant("new row"), Qt::EditRole));
    QVERIFY("new row" == m_videoTableModel->m_videos.at(2)->url());
    QVERIFY(m_rowsAboutToBeInsertedCalled);
    QVERIFY(m_rowsInsertedCalled);
    QVERIFY(3 == m_videoTableModel->m_videos.size());


    // minimum and maximum values of progress bar must be changed via
    // VideoTableModel::ProgressMinimumRole, ProgressMaximumRole
    // + text with ProgressTextRole
    index = m_videoTableModel->index(0, VideoTableModel::ProgressColumn);
    QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());
    QVERIFY(m_videoTableModel->setData(index, QVariant(10), VideoTableModel::ProgressMinimumRole));
    QVERIFY(10 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());

    QVERIFY(100 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());
    QVERIFY(m_videoTableModel->setData(index, QVariant(90), VideoTableModel::ProgressMaximumRole));
    QVERIFY(90 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());

    QVERIFY("" == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).toString());
    QVERIFY(m_videoTableModel->setData(index, QVariant("moro"), VideoTableModel::ProgressTextRole));
    QVERIFY("moro" == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).toString());


    index = m_videoTableModel->index(3, VideoTableModel::ProgressColumn);
    QVERIFY(false == m_videoTableModel->setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole));
    QVERIFY(false == m_videoTableModel->setData(index, QVariant(100), VideoTableModel::ProgressMaximumRole));
    QVERIFY(false == m_videoTableModel->setData(index, QVariant("heippa"), VideoTableModel::ProgressTextRole));
}

void TestVideoTableModel::insertRows() {
    m_rowsAboutToBeInsertedCalled = false;
    m_rowsInsertedCalled = false;

    QVERIFY(m_videoTableModel->insertRows(m_videoTableModel->rowCount(), 1));
    QVERIFY(m_rowsAboutToBeInsertedCalled);
    QVERIFY(m_rowsInsertedCalled);
    QVERIFY(m_videoTableModel->rowCount() == 4);
    QVERIFY(m_videoTableModel->m_videos.size() == 3);
}

void TestVideoTableModel::removeRows() {
    // try removing the next row after the last row
    m_rowsAboutToBeRemovedCalled = false;
    m_rowsRemovedCalled = false;
    QVERIFY(false == m_videoTableModel->removeRows(m_videoTableModel->rowCount(), 1));
    QVERIFY(false == m_rowsAboutToBeRemovedCalled);
    QVERIFY(false == m_rowsRemovedCalled);
    QVERIFY(m_videoTableModel->rowCount() == 3);
    QVERIFY(m_videoTableModel->m_videos.size() == 2);

    // remove the 1st row
    m_rowsAboutToBeRemovedCalled = false;
    m_rowsRemovedCalled = false;
    QVERIFY(m_videoTableModel->removeRows(0, 1));
    QVERIFY(m_rowsAboutToBeRemovedCalled);
    QVERIFY(m_rowsRemovedCalled);
    QVERIFY(m_videoTableModel->rowCount() == 2);
    QVERIFY(m_videoTableModel->m_videos.size() == 1);

    m_rowsAboutToBeRemovedCalled = false;
    m_rowsRemovedCalled = false;
    QVERIFY(m_videoTableModel->removeRows(0, 1));
    QVERIFY(m_rowsAboutToBeRemovedCalled);
    QVERIFY(m_rowsRemovedCalled);
    QVERIFY(m_videoTableModel->rowCount() == 1);
    QVERIFY(m_videoTableModel->m_videos.size() == 0);

    // try removing from empty model
    m_rowsAboutToBeRemovedCalled = false;
    m_rowsRemovedCalled = false;
    QVERIFY(false == m_videoTableModel->removeRows(0, 1));
    QVERIFY(false == m_rowsAboutToBeRemovedCalled);
    QVERIFY(false == m_rowsRemovedCalled);
    QVERIFY(m_videoTableModel->rowCount() == 1);
    QVERIFY(m_videoTableModel->m_videos.size() == 0);
}

void TestVideoTableModel::tryToRemoveTooManyRowsFromMiddle() {
    m_rowsAboutToBeRemovedCalled = false;
    m_rowsRemovedCalled = false;
    QVERIFY(false == m_videoTableModel->removeRows(1, 2));
    QVERIFY(false == m_rowsAboutToBeRemovedCalled);
    QVERIFY(false == m_rowsRemovedCalled);
    QVERIFY(m_videoTableModel->rowCount() == 3);
    QVERIFY(m_videoTableModel->m_videos.size() == 2);
}

void TestVideoTableModel::addUrl() {
    QString newUrl = "newUrl";
    QVERIFY(2 == m_videoTableModel->addUrl(newUrl));
    QVERIFY(m_videoTableModel->m_videos.size() == 3);
    QModelIndex index = m_videoTableModel->index(2, VideoTableModel::UrlColumn);
    QVERIFY("newUrl" == m_videoTableModel->data(index, Qt::DisplayRole));

    // shouldn't be able to add empty / whitespace only strings
    newUrl = "";
    QVERIFY(-1 == m_videoTableModel->addUrl(newUrl));
    QVERIFY(m_videoTableModel->m_videos.size() == 3);

    newUrl = "    ";
    QVERIFY(-1 == m_videoTableModel->addUrl(newUrl));
    QVERIFY(m_videoTableModel->m_videos.size() == 3);
}

void TestVideoTableModel::url() {
    QVERIFY("http://localhost/1" == m_videoTableModel->url(0));
    QVERIFY("http://localhost/2" == m_videoTableModel->url(1));
    QVERIFY("" == m_videoTableModel->url(2));
    QVERIFY("" == m_videoTableModel->url(-1));
}

void TestVideoTableModel::setKnownDownloadProgress() {
    // set progress text (default)
    for (int i=0; i < 2; i++) {
        QVERIFY(m_videoTableModel->setKnownDownloadProgress(i, 15 + i));

        QString expectedText = QString::number(15 + i) + "%";
        QModelIndex index = m_videoTableModel->index(i, VideoTableModel::ProgressColumn);
        QVERIFY((15 + i) == m_videoTableModel->data(index, VideoTableModel::ProgressRole).toInt());
        QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());
        QVERIFY(100 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());
        QVERIFY(expectedText == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).toString());
    }

    // don't set progress text
    for (int i=0; i < 2; i++) {
        QVERIFY(m_videoTableModel->setKnownDownloadProgress(i, 20 + i, false));

        QModelIndex index = m_videoTableModel->index(i, VideoTableModel::ProgressColumn);
        QVERIFY((20 + i) == m_videoTableModel->data(index, VideoTableModel::ProgressRole).toInt());
        QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());
        QVERIFY(100 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());
        QVERIFY("" == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).toString());
    }

    // row number too high
    QVERIFY(!m_videoTableModel->setKnownDownloadProgress(2, 16));
    QVERIFY(m_videoTableModel->rowCount() == 3);
}

void TestVideoTableModel::setUnknownDownloadProgress() {
    // set some progress text
    for (int i=0; i < 2; i++) {
        QString progress = QString::number(i+10) + " hours";
        QVERIFY(m_videoTableModel->setUnknownDownloadProgress(i, progress));

        QModelIndex index = m_videoTableModel->index(i, VideoTableModel::ProgressColumn);
        QVERIFY(-1 == m_videoTableModel->data(index, VideoTableModel::ProgressRole).toInt());
        QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());
        QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());
        QVERIFY(progress == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).toString());
    }

    // set empty string as text
    for (int i=0; i < 2; i++) {
        QString progress = "";
        QVERIFY(m_videoTableModel->setUnknownDownloadProgress(i, progress));

        QModelIndex index = m_videoTableModel->index(i, VideoTableModel::ProgressColumn);
        QVERIFY(-1 == m_videoTableModel->data(index, VideoTableModel::ProgressRole).toInt());
        QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMinimumRole).toInt());
        QVERIFY(0 == m_videoTableModel->data(index, VideoTableModel::ProgressMaximumRole).toInt());
        QVERIFY(progress == m_videoTableModel->data(index, VideoTableModel::ProgressTextRole).toString());
    }

    // row number too high
    QVERIFY(!m_videoTableModel->setUnknownDownloadProgress(2, QString("5 hours")));
    QVERIFY(m_videoTableModel->rowCount() == 3);
}

void TestVideoTableModel::downloadProgress() {
    for (int row=0; row < 2; row++) {
        for (int knownProgress=0; knownProgress < 2; knownProgress++) {
            for (int progress=-10; progress < 110; progress++) {
                if (knownProgress) {
                    m_videoTableModel->setKnownDownloadProgress(row, progress);
                    QVERIFY(progress == m_videoTableModel->downloadProgress(row));
                } else {
                    m_videoTableModel->setUnknownDownloadProgress(row, QString("sometext"));
                    QVERIFY(-1 == m_videoTableModel->downloadProgress(row));
                }
            }
        }
    }
    // row number too high
    for (int row=0; row < 2; row++) {
        m_videoTableModel->setKnownDownloadProgress(row, 10 + row);
    }
    QVERIFY(0 == m_videoTableModel->downloadProgress(2));
}

void TestVideoTableModel::setDownloadState() {
    for (int i=0; i < 2; i++) {
        QVERIFY(m_videoTableModel->setDownloadState(i, VideoInfo::StateStarting));

        QModelIndex index = m_videoTableModel->index(i, VideoTableModel::StatusColumn);
        QVERIFY(VideoInfo::StateStarting == (VideoInfo::VideoState)(m_videoTableModel->data(index, Qt::UserRole).toInt()));
    }

    // row number too high
    QVERIFY(!m_videoTableModel->setDownloadState(2, VideoInfo::StateStarting));
}

void TestVideoTableModel::setVideoFileName() {
    for (int i=0; i < 2; i++) {
        QString fileName = "video" + QString::number(i) + ".flv";
        QVERIFY(m_videoTableModel->setVideoFileName(i, fileName));

        QModelIndex index = m_videoTableModel->index(i, VideoTableModel::FileNameColumn);
        QVERIFY(fileName == m_videoTableModel->data(index).toString());
    }
}

QTEST_APPLESS_MAIN(TestVideoTableModel)

#include "testVideoTableModel.moc"
