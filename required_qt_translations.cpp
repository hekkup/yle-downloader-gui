
#include <QtCore>

// We mention here those builtin Qt strings we want to appear in our translation files.

void containerForQtStringsToTranslate() {
    const char* qtStringsToTranslate[] = {
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&Yes"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&No"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&OK"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&Cancel"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "OK"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "Cancel")
    };
    Q_UNUSED(qtStringsToTranslate);
}
