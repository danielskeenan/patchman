/**
 * @file AboutDialog.cpp
 *
 * @author Dan Keenan
 * @date 12/15/2023
 * @copyright GNU GPLv3
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "AboutDialog.h"
#include "patchman_config.h"

namespace patchman
{

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto *iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(qApp->windowIcon().pixmap(48));
    layout->addWidget(iconLabel);

    auto *titleLabel = new QLabel(QString("<span style=\"font-size: x-large; font-weight: bold;\">%1</span>")
                                      .arg(qApp->applicationDisplayName()), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    auto *versionLabel = new QLabel(qApp->applicationVersion(), this);
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);

    if (std::strlen(config::kProjectCommitTag) == 0) {
        // Untagged build means dev build.
        auto *commitShaLabel = new QLabel(tr("Dev Build from %1").arg(QString("<a href=\"%1\">%2</a>")
                                                                          .arg(QString(config::kProjectCommitShaDetailUrl)
                                                                                   .arg(config::kProjectCommitSha))
                                                                          .arg(config::kProjectCommitSha)), this);
        commitShaLabel->setAlignment(Qt::AlignCenter);
        commitShaLabel->setOpenExternalLinks(true);
        layout->addWidget(commitShaLabel);
    }

    auto *homepageLabel = new QLabel(tr("<a href=\"%1\">%1</a>").arg(config::kProjectHomepageUrl), this);
    homepageLabel->setAlignment(Qt::AlignCenter);
    homepageLabel->setOpenExternalLinks(true);
    layout->addWidget(homepageLabel);

    auto *aboutQtLabel = new QLabel(QString("<a href=\"about:qt\">%1</a>").arg(tr("About Qt")), this);
    aboutQtLabel->setAlignment(Qt::AlignCenter);
    connect(aboutQtLabel, &QLabel::linkActivated, this, &AboutDialog::linkActivated);
    layout->addWidget(aboutQtLabel);

    auto *about3rdPartyLabel = new QLabel(
        QString("<a href=\"file:///%1/../%2/3rd_party.html\">3rd Party Software</a>")
            .arg(qApp->applicationDirPath(), config::kHelpPath), this);
    about3rdPartyLabel->setAlignment(Qt::AlignCenter);
    about3rdPartyLabel->setOpenExternalLinks(true);
    layout->addWidget(about3rdPartyLabel);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AboutDialog::close);
    layout->addWidget(buttonBox);
}

void AboutDialog::linkActivated(const QString &link)
{
    if (link == "about:qt") {
        QMessageBox::aboutQt(this);
    }
}

} // patchman
