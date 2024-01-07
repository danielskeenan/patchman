/**
 * @file SettingsDialog.h
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidget>

namespace patchman
{

/**
 * Widget to edit ROM search paths.
 */
class SearchPathsWidget: public QWidget
{
Q_OBJECT
public:
    explicit SearchPathsWidget(QWidget *parent = nullptr);

    [[nodiscard]] QList<QString> getPaths() const;

private:
    struct Actions
    {
        QAction *addPath = nullptr;
        QAction *removePath = nullptr;
    };
    Actions actions_;
    QListWidget *listWidget_;

private Q_SLOTS:
    void selectionChanged();
    void addSearchPath();
    void removeSearchPath();
};

/**
 * Show user-adjustable settings.
 */
class SettingsDialog: public QDialog
{
Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

public Q_SLOTS:
    void accept() override;

private:
    struct Widgets
    {
        SearchPathsWidget *searchPaths = nullptr;
    };
    Widgets widgets_;
};

} // patchman

#endif //SETTINGSDIALOG_H
