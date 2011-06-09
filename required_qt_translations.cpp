
#include <QtCore>

// We mention here those builtin Qt strings we want to appear in our translation files.

void containerForQtStringsToTranslate() {
    const char* qtStringsToTranslate[] = {
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&Yes"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&No")
    };
    Q_UNUSED(qtStringsToTranslate);
}
