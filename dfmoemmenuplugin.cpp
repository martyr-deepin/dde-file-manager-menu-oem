#include "dfmoemmenuplugin.h"

#include <QDir>
#include <XdgDesktopFile>

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

            QAction *action = new QAction(file.icon(), file.name(), this);

            connect(action, &QAction::triggered, this, [action, file](){
                QStringList files = action->data().toStringList();
                file.startDetached(files);
            });

            actionList.append(action);
        }
    }
}

QList<QAction *> DFMOEMMenuPlugin::additionalMenu(const QStringList &files, const QString &currentDir)
{
    Q_UNUSED(currentDir);

    for (QAction * action : actionList) {
        action->setData(files);
    }

    return actionList;
}
