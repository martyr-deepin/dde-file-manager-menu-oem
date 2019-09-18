#include "dfmoemmenuplugin.h"
#include "dfileservices.h"

#include <QDir>
#include <QDebug>
#include <XdgDesktopFile>
#include <QUrl>
#include <QMenu>

static QStringList AllMenuTypes {
    "SingleFile",
//    "MultiFiles",
    "SingleDir",
//    MultiDirs,
    "MultiFileDirs",
    "EmptyArea"
};

#define MENU_TYPE_KEY "X-DFM-MenuTypes"
#define MIME_TYPE_KEY "X-DFM-MimeTypes"

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

            // the XdgDesktopFile::icon() empty fallback is not really an empty fallback, so we need to check it manually.
            QString iconStr = file.localizedValue("Icon").toString();
            QAction *action = new QAction(iconStr.isEmpty() ? QIcon() : file.icon(), file.name(), this);
            QStringList entryActionList = file.actions();
            if (!entryActionList.isEmpty()) {
                QMenu *menu = new QMenu();
                for (const QString &actionName : entryActionList) {
                    QAction *subAction = new QAction(file.actionIcon(actionName), file.actionName(actionName), this);
                    connect(subAction, &QAction::triggered, this, [subAction, actionName, file](){
                        QStringList files = subAction->data().toStringList();
                        file.actionActivate(actionName, files);
                    });
                    menu->addAction(subAction);
                }
                action->setMenu(menu);
            }

            connect(action, &QAction::triggered, this, [action, file](){
                QStringList files = action->data().toStringList();
                file.startDetached(files);
            });

            actionList.append(action);
            action->setProperty(MIME_TYPE_KEY, file.mimeTypes());

            for (const QString &oneType : menuTypes) {
                actionListByType[oneType].append(action);
            }
        }
    }
}

void appendParentMineType(const QStringList &parentmimeTypes,  QStringList& mimeTypes)
{
    if (parentmimeTypes.size()==0)
        return;

    for (const QString &mtName : parentmimeTypes) {
        QMimeDatabase db;
        QMimeType mt = db.mimeTypeForName(mtName);
        mimeTypes.append(mt.name());
        mimeTypes.append(mt.aliases());
        QStringList pmts = mt.parentMimeTypes();
        appendParentMineType(pmts, mimeTypes);
    }
}

QList<QAction *> DFMOEMMenuPlugin::additionalMenu(const QStringList &files, const QString &currentDir)
{
    Q_UNUSED(currentDir);

    QString menuType = "Unknown";
    QUrl url;

    if (files.count() == 1) {
        url.setUrl(files.first());
        menuType = QFileInfo(url.toLocalFile()).isDir() ? "SingleDir" : "SingleFile";
    } else {
        menuType = "MultiFileDirs";
    }

    // Add file list data.
    for (QAction * action : actionList) {
        action->setData(files);
        if (action->menu()) {
            for (QAction * subAction : action->menu()->actions()) {
                subAction->setData(files);
            }
        }
    }

    QList<QAction *> actions = actionListByType[menuType];
    if (url.isEmpty())
        return  actions;

    const DAbstractFileInfoPointer &file_info = DFileService::instance()->createFileInfo(this, url);
    if (!file_info)
        return {};

    QStringList fileMimeTypes;
    fileMimeTypes.append(file_info->mimeType().name());
    fileMimeTypes.append(file_info->mimeType().aliases());

    const QMimeType &mt = file_info->mimeType();
    appendParentMineType(mt.parentMimeTypes(), fileMimeTypes);
    fileMimeTypes.removeAll({});

    auto isSupport = [](const QString &mt, const QStringList &fileMimeTypes)->bool{
        foreach(const QString &fmt, fileMimeTypes){
            if (fmt.contains(mt, Qt::CaseInsensitive))
                return true;
        }
        return false;
    };


    for (auto it = actions.begin(); it != actions.end(); ) {
        QAction * action = *it;

        if(action) {
            QStringList supportMimeTypes =  action->property(MIME_TYPE_KEY).toStringList();
            supportMimeTypes.removeAll({});
            bool match = supportMimeTypes.size() == 0; // no types ==> *
            for (QString mt : supportMimeTypes) {
                if (fileMimeTypes.contains(mt, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }

                int starPos = mt.indexOf("*");
                if (starPos >=0 && isSupport(mt.left(starPos), fileMimeTypes)) {
                    match = true;
                    break;
                }
            }

            if (!match) {
                it = actions.erase(it);
                continue;
            }
        }

        ++it;
    }

    return actions;
}

QList<QAction *> DFMOEMMenuPlugin::additionalEmptyMenu(const QString &currentDir, bool onDesktop)
{
    Q_UNUSED(currentDir);
    Q_UNUSED(onDesktop);

    return actionListByType["EmptyArea"];
}
