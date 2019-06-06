## 深度文管上下文菜单 OEM 支持插件

在启用此插件支持后，在深度文件管理器载入此插件时，会识别指定目录下的 `.desktop` 文件，并将其视为扩展菜单项，加入到文件管理器的上下文菜单中。

### OEM 方式

每个定制菜单项为一个独立的 Application 类型的 `.desktop` 文件，至少需要包含 `Name` 和 `Exec` 字段，`Name` 的内容为添加到上下文菜单中时将会显示的名称，`Exec` 的内容为当单击此新增的菜单项时将会执行的行为（遵循 desktop 文件规范标准，可以使用 `%u` 等参数）。

OEM 厂商需要将待添加的 `desktop` 文件放置到 `/usr/share/deepin/dde-file-manager/oem-menuextensions/` 位置，在下次启动文件管理器时[^1]，选中任意一个或多个文件并触发上下文菜单，将可以看到新增的项目位于其中。

### 示例 `.desktop` 文件

可以将下面的示例存储为 `test.desktop` 并放置到 `/usr/share/deepin/dde-file-manager/oem-menuextensions/` 中，关闭现有的所有文件管理器窗口并打开新的文件管理器窗口，即可看到效果。

``` ini
[Desktop Entry]
Type=Application
Exec=/home/wzc/Temp/test.sh %U
Icon=utilities-terminal
Name=示例菜单项名称
```

### 菜单 OEM 支持插件安装方式

在安装所有所需依赖（`qtbase5-dev`, `libdde-file-manager-dev` 和 `libqt5xdg-dev`）的情况，在 Deepin 中即可通过 `debuild -uc -us` 直接构建 deb 包，可以通过构建的 deb 包安装该支持插件到对应的目录下。

若需要手动编译安装和调试，可以直接使用 QtCreator 或其他喜欢的开发环境进行开发和构建，项目使用 `qmake` 作为构建系统，可以通过其生成 Makefile 并使用它编译生成对应的插件库 `dde-file-manager-menu-oem-plugin.so` ，生成后将其放置或链接至 `/usr/lib/x86_64-linux-gnu/dde-file-manager/plugins/menu` （`$$[QT_INSTALL_LIBS]/dde-file-manager/plugins/menu/`）即可，下次启动文件管理器时将会载入此上下文菜单 OEM 支持插件。

[^1]: 注：这是由于菜单列表仅在 OEM 插件加载时识别并记录一次。在部分平台，深度文件管理器存在加速启动用的进程常驻后台，您可能需要结束所有文件管理器进程。类似的，对于桌面，也需要结束桌面的相应进程才能使桌面使用新增的菜单。