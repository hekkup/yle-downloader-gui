#include "VideoInfo.h"

VideoInfo::VideoInfo(QObject *parent) :
    QObject(parent)
{
    m_url = QString("");
    m_progress = 0;
    m_progressMinimum = 0;
    m_progressMaximum = 100;
    m_progressText = QString("");
    m_videoState = VideoInfo::StateNotStarted;
    m_videoStateStrings[VideoInfo::StateNotStarted] = tr("Not started");
    m_videoStateStrings[VideoInfo::StateStarting] = tr("Starting...");
    m_videoStateStrings[VideoInfo::StateLoading] = tr("Loading...");
    m_videoStateStrings[VideoInfo::StateLoadedOk] = tr("Loaded");
    m_videoStateStrings[VideoInfo::StateCanceled] = tr("Canceled");
    m_videoStateStrings[VideoInfo::StateFailed] = tr("Failed");
    m_fileName = QString("");
}

VideoInfo::~VideoInfo() {
    m_url = "";
    m_progress = 0;
    m_progressMinimum = 0;
    m_progressMaximum = 100;
    m_progressText = "";
    m_videoState = StateNotStarted;
}

QString VideoInfo::url() {
    return m_url;
}

int VideoInfo::progress() {
    return m_progress;
}

int VideoInfo::progressMinimum() {
    return m_progressMinimum;
}

int VideoInfo::progressMaximum() {
    return m_progressMaximum;
}

QString VideoInfo::progressText() {
    return m_progressText;
}

VideoInfo::VideoState VideoInfo::state() {
    return m_videoState;
}

QString VideoInfo::stateString(VideoInfo::VideoState state) {
    if ((state >= VideoInfo::StateNotStarted) && (state < VideoInfo::STATE_COUNT)) {
        return m_videoStateStrings[state];
    } else return QString("");
}

QString VideoInfo::fileName() {
    return m_fileName;
}

void VideoInfo::setUrl(QString url) {
    m_url = url;
}

void VideoInfo::setProgress(int progress) {
    m_progress = progress;
}

void VideoInfo::setProgressMinimum(int progressMinimum) {
    m_progressMinimum = progressMinimum;
}

void VideoInfo::setProgressMaximum(int progressMaximum) {
    m_progressMaximum = progressMaximum;
}

void VideoInfo::setProgressText(QString text) {
    m_progressText = text;
}

void VideoInfo::setState(VideoInfo::VideoState state) {
    if ((state >= VideoInfo::StateNotStarted) && (state < VideoInfo::STATE_COUNT)) {
        m_videoState = state;
    }
}

void VideoInfo::setFileName(QString fileName) {
    m_fileName = fileName;
}
