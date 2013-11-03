
#include <QtCore>

// We mention here those builtin Qt strings we want to appear in our translation files.

void containerForQtStringsToTranslate() {
    const char* qtStringsToTranslate[] = {
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&Yes"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&No"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&OK"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "&Cancel"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "OK"),
        QT_TRANSLATE_NOOP("QDialogButtonBox", "Cancel"),
        // QLineEdit context menu
        QT_TRANSLATE_NOOP("QLineEdit", "&Undo"),
        QT_TRANSLATE_NOOP("QLineEdit", "&Redo"),
        QT_TRANSLATE_NOOP("QLineEdit", "Cu&t"),
        QT_TRANSLATE_NOOP("QLineEdit", "&Copy"),
        QT_TRANSLATE_NOOP("QLineEdit", "&Paste"),
        QT_TRANSLATE_NOOP("QLineEdit", "Delete"),
        QT_TRANSLATE_NOOP("QLineEdit", "Select All"),
    };
    Q_UNUSED(qtStringsToTranslate);
}
