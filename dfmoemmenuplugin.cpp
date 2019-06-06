#include "dfmoemmenuplugin.h"

#include <QDir>
#include <QDebug>
#include <XdgDesktopFile>
#include <QUrl>

static QStringList AllMenuTypes {
    "SingleFile",
//    "MultiFiles",
    "SingleDir",
//    MultiDirs,
    "MultiFileDirs",
    "EmptyArea"
};

#define MENU_TYPE_KEY "X-DFM-MenuTypes"

DFMOEMMenuPlugin::DFMOEMMenuPlugin()
{
    QDir oemPath("/usr/share/deepin/dde-file-manager/oem-menuextensions/");

    if (oemPath.exists()) {
        for (const QFileInfo &fileInfo : oemPath.entryInfoList({"*.desktop"})) {
            XdgDesktopFile file;
            file.load(fileInfo.filePath());
            if (!file.isValid()) {
                continue;
            }

            QStringList menuTypes;
            if (file.contains(MENU_TYPE_KEY)) {
                menuTypes = file.value(MENU_TYPE_KEY).toString().split(';', QString::SkipEmptyParts);
                for (const QString &oneType : menuTypes) {
                    if (!AllMenuTypes.contains(oneType)) {
                        menuTypes.removeAll(oneType);
                    }
                }
            } else {
                menuTypes = AllMenuTypes;
            }

            if (menuTypes.isEmpty()) {
                qDebug() << "[OEM Menu Support] Entry will probably not be shown due to empty or have no valid"
                         << MENU_TYPE_KEY << "key in the desktop file.";
                qDebug() << "[OEM Menu Support] Details:" << fileInfo.filePath() << "with entry name" << file.name();
            }

            QAction *action = new QAction(file.icon(), file.name(), this);

            connect(action, &QAction::triggered, this, [action, file](){
                QStringList files = action->data().toStringList();
                file.startDetached(files);
            });

            actionList.append(action);

            for (const QString &oneType : menuTypes) {
                actionListByType[oneType].append(action);
            }
        }
    }
}

QList<QAction *> DFMOEMMenuPlugin::additionalMenu(const QStringList &files, const QString &currentDir)
{
    Q_UNUSED(currentDir);

    QString menuType = "Unknown";

    if (files.count() == 1) {
        QUrl url(files.first());
        menuType = QFileInfo(url.toLocalFile()).isDir() ? "SingleDir" : "SingleFile";
    } else {
        menuType = "MultiFileDirs";
    }

    // Add file list data.
    for (QAction * action : actionList) {
        action->setData(files);
    }

    return actionListByType[menuType];
}

QList<QAction *> DFMOEMMenuPlugin::additionalEmptyMenu(const QString &currentDir, bool onDesktop)
{
    Q_UNUSED(currentDir);
    Q_UNUSED(onDesktop);

    return actionListByType["EmptyArea"];
}
