#ifndef PROCESSUTILS_H
#define PROCESSUTILS_H

#include <QProcess>

namespace ProcessUtils
{

void killProcessTree(QProcess& parentProcess);

}

#endif // PROCESSUTILS_H
