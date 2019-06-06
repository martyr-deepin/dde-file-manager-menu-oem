#ifndef DFMOEMMENUPLUGIN_H
#define DFMOEMMENUPLUGIN_H

#include <dde-file-manager-plugins/menuinterface.h>

class DFMOEMMenuPlugin : public MenuInterface
{
    Q_OBJECT
    Q_INTERFACES(MenuInterface)
    Q_PLUGIN_METADATA(IID MenuInterface_iid)

public:
    DFMOEMMenuPlugin();

    QList<QAction*> additionalMenu(const QStringList &files, const QString& currentDir) override;
    QList<QAction*> additionalEmptyMenu(const QString &currentDir, bool onDesktop = false) override;

//    static QList<QAction *> loadMenuExtensionActions(const DUrlList &urlList, const DUrl &currentUrl, bool onDesktop = false);
//    static QList<QAction *> jsonToActions(const QJsonArray &data, const DUrlList &urlList, const DUrl &currentUrl,
//                                          const QString &menuExtensionType, const bool onDesktop);

private:
    QList<QAction *> actionList;
};

#endif // DFMOEMMENUPLUGIN_H
