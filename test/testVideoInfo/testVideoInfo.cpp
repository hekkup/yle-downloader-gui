#include "VideoInfo.h"

#include <QtCore/QString>
#include <QtTest/QtTest>

class TestVideoInfo : public QObject
{
    Q_OBJECT
    
public:
    TestVideoInfo();
    
private Q_SLOTS:
    void init();
    void cleanup();

    void constructor();
    void getAndSetUrl();
    void getAndSetProgress();
    void getAndSetProgressMinAndMax();
    void getAndSetProgressText();
    void getAndSetState();
    void stateString();
    void getAndSetFileName();

private:
    VideoInfo* m_videoInfo;
};

TestVideoInfo::TestVideoInfo() {
}

void TestVideoInfo::init() {
    m_videoInfo = new VideoInfo();
}

void TestVideoInfo::cleanup() {
    if (m_videoInfo) {
        delete m_videoInfo;
        m_videoInfo = NULL;
    }
}

void TestVideoInfo::constructor() {
    QVERIFY("" == m_videoInfo->m_url);
    QVERIFY(0 == m_videoInfo->m_progress);
    QVERIFY(0 == m_videoInfo->m_progressMinimum);
    QVERIFY(100 == m_videoInfo->m_progressMaximum);
    QVERIFY("" == m_videoInfo->m_progressText);
    QVERIFY(0 == m_videoInfo->m_videoState);
    QVERIFY("" == m_videoInfo->m_fileName);

    QVERIFY("Not started" == m_videoInfo->m_videoStateStrings[VideoInfo::StateNotStarted]);
    QVERIFY("Starting..." == m_videoInfo->m_videoStateStrings[VideoInfo::StateStarting]);
    QVERIFY("Loading..." == m_videoInfo->m_videoStateStrings[VideoInfo::StateLoading]);
    QVERIFY("Loaded" == m_videoInfo->m_videoStateStrings[VideoInfo::StateLoadedOk]);
    QVERIFY("Canceled" == m_videoInfo->m_videoStateStrings[VideoInfo::StateCanceled]);
    QVERIFY("Failed" == m_videoInfo->m_videoStateStrings[VideoInfo::StateFailed]);

}

void TestVideoInfo::getAndSetUrl() {
    m_videoInfo->setUrl("hello");
    QVERIFY("hello" == m_videoInfo->m_url);
    QVERIFY("hello" == m_videoInfo->url());

    m_videoInfo->setUrl("");
    QVERIFY("" == m_videoInfo->m_url);
    QVERIFY("" == m_videoInfo->url());
}

void TestVideoInfo::getAndSetProgress() {
    m_videoInfo->setProgress(1);
    QVERIFY(1 == m_videoInfo->m_progress);
    QVERIFY(1 == m_videoInfo->progress());

    m_videoInfo->setProgress(200);
    QVERIFY(200 == m_videoInfo->m_progress);
    QVERIFY(200 == m_videoInfo->progress());

    m_videoInfo->setProgress(-10);
    QVERIFY(-10 == m_videoInfo->m_progress);
    QVERIFY(-10 == m_videoInfo->progress());
}

void TestVideoInfo::getAndSetProgressMinAndMax() {
    m_videoInfo->setProgressMinimum(10);
    QVERIFY(10 == m_videoInfo->m_progressMinimum);
    QVERIFY(10 == m_videoInfo->progressMinimum());
    m_videoInfo->setProgressMinimum(20);
    QVERIFY(20 == m_videoInfo->m_progressMinimum);
    QVERIFY(20 == m_videoInfo->progressMinimum());

    m_videoInfo->setProgressMaximum(30);
    QVERIFY(30 == m_videoInfo->m_progressMaximum);
    QVERIFY(30 == m_videoInfo->progressMaximum());
    m_videoInfo->setProgressMaximum(40);
    QVERIFY(40 == m_videoInfo->m_progressMaximum);
    QVERIFY(40 == m_videoInfo->progressMaximum());
}

void TestVideoInfo::getAndSetProgressText() {
    m_videoInfo->setProgressText(QString("viiskyt prossaa"));
    QVERIFY("viiskyt prossaa" == m_videoInfo->m_progressText);
    QVERIFY("viiskyt prossaa" == m_videoInfo->progressText());

    m_videoInfo->setProgressText(QString("1:20"));
    QVERIFY("1:20" == m_videoInfo->m_progressText);
    QVERIFY("1:20" == m_videoInfo->progressText());
}

void TestVideoInfo::getAndSetState() {
    m_videoInfo->setState(VideoInfo::StateStarting);
    QVERIFY(VideoInfo::StateStarting == m_videoInfo->m_videoState);
    QVERIFY(VideoInfo::StateStarting == m_videoInfo->state());

    m_videoInfo->setState(VideoInfo::STATE_COUNT);
    QVERIFY(VideoInfo::StateStarting == m_videoInfo->m_videoState);
    QVERIFY(VideoInfo::StateStarting == m_videoInfo->state());

    m_videoInfo->setState((VideoInfo::VideoState)-1);
    QVERIFY(VideoInfo::StateStarting == m_videoInfo->m_videoState);
    QVERIFY(VideoInfo::StateStarting == m_videoInfo->state());
}

void TestVideoInfo::stateString() {
    QVERIFY("Not started" == m_videoInfo->stateString(VideoInfo::StateNotStarted));
    QVERIFY("Starting..." == m_videoInfo->stateString(VideoInfo::StateStarting));
    QVERIFY("Loading..." == m_videoInfo->stateString(VideoInfo::StateLoading));
    QVERIFY("Loaded" == m_videoInfo->stateString(VideoInfo::StateLoadedOk));
    QVERIFY("Canceled" == m_videoInfo->stateString(VideoInfo::StateCanceled));
    QVERIFY("Failed" == m_videoInfo->stateString(VideoInfo::StateFailed));

    QVERIFY("" == m_videoInfo->stateString((VideoInfo::VideoState)-1));
    QVERIFY("" == m_videoInfo->stateString(VideoInfo::STATE_COUNT));
}

void TestVideoInfo::getAndSetFileName() {
    QVERIFY("" == m_videoInfo->fileName());
    m_videoInfo->setFileName(QString("video.flv"));
    QVERIFY("video.flv" == m_videoInfo->fileName());
    m_videoInfo->setFileName(QString(""));
    QVERIFY("" == m_videoInfo->fileName());
}

QTEST_APPLESS_MAIN(TestVideoInfo)

#include "testVideoInfo.moc"
