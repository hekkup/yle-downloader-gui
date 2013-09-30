#include "ProcessUtils.h"

#include <QtCore>

#ifdef Q_OS_WIN32
#include <Windows.h>
#include <Tlhelp32.h>
#else
#include <sys/types.h>
#include <signal.h>
#endif

namespace ProcessUtils
{

namespace
{

#ifdef Q_OS_WIN32
void killProcessTree_Windows(Q_PID pid);
void killProcessTree_Windows(DWORD targetProcess, const QMap<DWORD, DWORD>& processParents);
void killSingleProcess_Windows(DWORD targetProcess);


/* We use the current process list and find the given process
   and all its children (transitively). Then we kill them in unspecified order.
   This is not race-condition-proof but good enough for this app. */
void killProcessTree_Windows(Q_PID pid)
{
    DWORD targetProcess = static_cast<_PROCESS_INFORMATION*>(pid)->dwProcessId;

    HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshotHandle == INVALID_HANDLE_VALUE) {
        qWarning() << "Failed to obtain system process list";
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (!Process32First(snapshotHandle, &pe)) {
        qWarning() << "Failed to obtain first process entry";
        CloseHandle(snapshotHandle);
        return;
    }

    QMap<DWORD, DWORD> processParents;

    bool more = true;
    while (more) {
        processParents.insert(pe.th32ProcessID, pe.th32ParentProcessID);
        more = Process32Next(snapshotHandle, &pe);
    }

    if (GetLastError() != ERROR_NO_MORE_FILES) {
        qWarning() << "Failed to obtain a process entry";
        CloseHandle(snapshotHandle);
        return;
    }

    killProcessTree_Windows(targetProcess, processParents);

    CloseHandle(snapshotHandle);
}

void killProcessTree_Windows(DWORD rootProcess, const QMap<DWORD, DWORD>& processParents)
{
    QSet<DWORD> pidsFound;
    pidsFound.insert(rootProcess);

    bool moreFound;
    do {
        moreFound = false;
        foreach (DWORD pid, processParents.keys()) {
            if (pidsFound.contains(processParents[pid]) && !pidsFound.contains(pid)) {
                pidsFound.insert(pid);
                moreFound = true;
            }
        }
    } while (moreFound);

    foreach (DWORD pid, pidsFound) {
        killSingleProcess_Windows(pid);
    }
}

void killSingleProcess_Windows(DWORD processId)
{
    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, false, processId);
    if (processHandle == NULL) {
        qWarning() << "Failed to open PID " << processId;
        return;
    }

    if (!TerminateProcess(processHandle, 127)) {
        qWarning() << "Failed to terminate PID " << processId;
    }

    CloseHandle(processHandle);
}

#endif

}

void killProcessTree(QProcess& parentProcess)
{
#ifdef Q_OS_WIN32
    killProcessTree_Windows(parentProcess.pid());
#else
    // yle-dl on Unix (Linux) only terminates cleanly on SIGINT
    kill(parentProcess.pid(), SIGINT);
#endif
}

}
