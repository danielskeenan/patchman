/**
 * @file DarkModeHandler.h
 *
 * @author Dan Keenan
 * @date 9/28/25
 * @copyright GNU GPLv3
 */

#ifndef SRC_PATCHMAN_DARKMODEHANDLER_H
#define SRC_PATCHMAN_DARKMODEHANDLER_H

#include <QObject>
#include <QString>

namespace patchman {

class DarkModeHandler : public QObject
{
    Q_OBJECT
public:
    explicit DarkModeHandler(QObject *parent = nullptr);
    ~DarkModeHandler() override;
    [[nodiscard]] bool isDarkMode() const;
    void updateIconTheme() const;
    [[nodiscard]] const QString &lightIconTheme() const { return lightIconTheme_; }
    void setLightIconTheme(const QString &lightIconTheme) { lightIconTheme_ = lightIconTheme; }
    [[nodiscard]] const QString &darkIconTheme() const { return darkIconTheme_; }
    void setDarkIconTheme(const QString &darkIconTheme) { darkIconTheme_ = darkIconTheme; }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QString lightIconTheme_;
    QString darkIconTheme_;
};

} // namespace patchman

#endif //SRC_PATCHMAN_DARKMODEHANDLER_H
