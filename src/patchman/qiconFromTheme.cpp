/**
 * @file qiconFromTheme.cpp
 *
 * @author Dan Keenan
 * @date 9/28/25
 * @copyright GNU GPLv3
 */

#include "qiconFromTheme.h"
#include <unordered_map>
#include <vector>
#include <QDebug>

namespace patchman {
static std::unordered_map<ThemeIcon, std::vector<QString>> kThemeIcons{
    {ThemeIcon::AddressBookNew, {QStringLiteral("address-book-new")}},
    {ThemeIcon::ApplicationExit, {QStringLiteral("application-exit")}},
    {ThemeIcon::AppointmentNew, {QStringLiteral("appointment-new")}},
    {ThemeIcon::CallStart, {QStringLiteral("call-start")}},
    {ThemeIcon::CallStop, {QStringLiteral("call-stop")}},
    {ThemeIcon::ContactNew, {QStringLiteral("contact-new")}},
    {ThemeIcon::DocumentNew, {QStringLiteral("document-new")}},
    {ThemeIcon::DocumentOpen, {QStringLiteral("document-open")}},
    {ThemeIcon::DocumentOpenRecent, {QStringLiteral("document-open-recent")}},
    {ThemeIcon::DocumentPageSetup, {QStringLiteral("document-page-setup")}},
    {ThemeIcon::DocumentPrint, {QStringLiteral("document-print")}},
    {ThemeIcon::DocumentPrintPreview, {QStringLiteral("document-print-preview")}},
    {ThemeIcon::DocumentProperties, {QStringLiteral("document-properties")}},
    {ThemeIcon::DocumentRevert, {QStringLiteral("document-revert")}},
    {ThemeIcon::DocumentSave, {QStringLiteral("document-save")}},
    {ThemeIcon::DocumentSaveAs, {QStringLiteral("document-save-as")}},
    {ThemeIcon::DocumentSend, {QStringLiteral("document-send")}},
    {ThemeIcon::EditClear, {QStringLiteral("edit-clear")}},
    {ThemeIcon::EditCopy, {QStringLiteral("edit-copy")}},
    {ThemeIcon::EditCut, {QStringLiteral("edit-cut")}},
    {ThemeIcon::EditDelete, {QStringLiteral("edit-delete")}},
    {ThemeIcon::EditFind, {QStringLiteral("edit-find")}},
    {ThemeIcon::EditPaste, {QStringLiteral("edit-paste")}},
    {ThemeIcon::EditRedo, {QStringLiteral("edit-redo")}},
    {ThemeIcon::EditSelectAll, {QStringLiteral("edit-select-all")}},
    {ThemeIcon::EditUndo, {QStringLiteral("edit-undo")}},
    {ThemeIcon::FolderNew, {QStringLiteral("folder-new")}},
    {ThemeIcon::FormatIndentLess, {QStringLiteral("format-indent-less")}},
    {ThemeIcon::FormatIndentMore, {QStringLiteral("format-indent-more")}},
    {ThemeIcon::FormatJustifyCenter, {QStringLiteral("format-justify-center")}},
    {ThemeIcon::FormatJustifyFill, {QStringLiteral("format-justify-fill")}},
    {ThemeIcon::FormatJustifyLeft, {QStringLiteral("format-justify-left")}},
    {ThemeIcon::FormatJustifyRight, {QStringLiteral("format-justify-right")}},
    {ThemeIcon::FormatTextDirectionLtr, {QStringLiteral("format-text-direction-ltr")}},
    {ThemeIcon::FormatTextDirectionRtl, {QStringLiteral("format-text-direction-rtl")}},
    {ThemeIcon::FormatTextBold, {QStringLiteral("format-text-bold")}},
    {ThemeIcon::FormatTextItalic, {QStringLiteral("format-text-italic")}},
    {ThemeIcon::FormatTextUnderline, {QStringLiteral("format-text-underline")}},
    {ThemeIcon::FormatTextStrikethrough, {QStringLiteral("format-text-strikethrough")}},
    {ThemeIcon::GoDown, {QStringLiteral("go-down")}},
    {ThemeIcon::GoHome, {QStringLiteral("go-home")}},
    {ThemeIcon::GoNext, {QStringLiteral("go-next")}},
    {ThemeIcon::GoPrevious, {QStringLiteral("go-previous")}},
    {ThemeIcon::GoUp, {QStringLiteral("go-up")}},
    {ThemeIcon::HelpAbout, {QStringLiteral("help-about")}},
    {ThemeIcon::HelpFaq, {QStringLiteral("help-faq"), QStringLiteral("help-contents")}},
    {ThemeIcon::InsertImage, {QStringLiteral("insert-image")}},
    {ThemeIcon::InsertLink, {QStringLiteral("insert-link")}},
    {ThemeIcon::InsertText, {QStringLiteral("insert-text")}},
    {ThemeIcon::ListAdd, {QStringLiteral("list-add")}},
    {ThemeIcon::ListRemove, {QStringLiteral("list-remove")}},
    {ThemeIcon::MailForward, {QStringLiteral("mail-forward")}},
    {ThemeIcon::MailMarkImportant, {QStringLiteral("mail-mark-important")}},
    {ThemeIcon::MailMarkRead, {QStringLiteral("mail-mark-read")}},
    {ThemeIcon::MailMarkUnread, {QStringLiteral("mail-mark-unread")}},
    {ThemeIcon::MailMessageNew, {QStringLiteral("mail-message-new")}},
    {ThemeIcon::MailReplyAll, {QStringLiteral("mail-reply-all")}},
    {ThemeIcon::MailReplySender, {QStringLiteral("mail-reply-sender")}},
    {ThemeIcon::MailSend, {QStringLiteral("mail-send")}},
    {ThemeIcon::MediaEject, {QStringLiteral("media-eject")}},
    {ThemeIcon::MediaPlaybackPause, {QStringLiteral("media-playback-pause")}},
    {ThemeIcon::MediaPlaybackStart, {QStringLiteral("media-playback-start")}},
    {ThemeIcon::MediaPlaybackStop, {QStringLiteral("media-playback-stop")}},
    {ThemeIcon::MediaRecord, {QStringLiteral("media-record")}},
    {ThemeIcon::MediaSeekBackward, {QStringLiteral("media-seek-backward")}},
    {ThemeIcon::MediaSeekForward, {QStringLiteral("media-seek-forward")}},
    {ThemeIcon::MediaSkipBackward, {QStringLiteral("media-skip-backward")}},
    {ThemeIcon::MediaSkipForward, {QStringLiteral("media-skip-forward")}},
    {ThemeIcon::ObjectRotateLeft, {QStringLiteral("object-rotate-left")}},
    {ThemeIcon::ObjectRotateRight, {QStringLiteral("object-rotate-right")}},
    {ThemeIcon::ProcessStop, {QStringLiteral("process-stop")}},
    {ThemeIcon::SystemLockScreen, {QStringLiteral("system-lock-screen")}},
    {ThemeIcon::SystemLogOut, {QStringLiteral("system-log-out")}},
    {ThemeIcon::SystemSearch, {QStringLiteral("system-search")}},
    {ThemeIcon::SystemReboot, {QStringLiteral("system-reboot")}},
    {ThemeIcon::SystemShutdown, {QStringLiteral("system-shutdown")}},
    {ThemeIcon::ToolsCheckSpelling, {QStringLiteral("tools-check-spelling")}},
    {ThemeIcon::ViewFullscreen, {QStringLiteral("view-fullscreen")}},
    {ThemeIcon::ViewRefresh, {QStringLiteral("view-refresh")}},
    {ThemeIcon::ViewRestore, {QStringLiteral("view-restore")}},
    {ThemeIcon::WindowClose, {QStringLiteral("window-close")}},
    {ThemeIcon::WindowNew, {QStringLiteral("window-new")}},
    {ThemeIcon::ZoomFitBest, {QStringLiteral("zoom-fit-best")}},
    {ThemeIcon::ZoomIn, {QStringLiteral("zoom-in")}},
    {ThemeIcon::ZoomOut, {QStringLiteral("zoom-out")}},

    {ThemeIcon::AudioCard, {QStringLiteral("audio-card")}},
    {ThemeIcon::AudioInputMicrophone, {QStringLiteral("audio-input-microphone")}},
    {ThemeIcon::Battery, {QStringLiteral("battery")}},
    {ThemeIcon::CameraPhoto, {QStringLiteral("camera-photo")}},
    {ThemeIcon::CameraVideo, {QStringLiteral("camera-video")}},
    {ThemeIcon::CameraWeb, {QStringLiteral("camera-web")}},
    {ThemeIcon::Computer, {QStringLiteral("computer")}},
    {ThemeIcon::DriveHarddisk, {QStringLiteral("drive-harddisk")}},
    {ThemeIcon::DriveOptical, {QStringLiteral("drive-optical")}},
    {ThemeIcon::InputGaming, {QStringLiteral("input-gaming")}},
    {ThemeIcon::InputKeyboard, {QStringLiteral("input-keyboard")}},
    {ThemeIcon::InputMouse, {QStringLiteral("input-mouse")}},
    {ThemeIcon::InputTablet, {QStringLiteral("input-tablet")}},
    {ThemeIcon::MediaFlash, {QStringLiteral("media-flash")}},
    {ThemeIcon::MediaOptical, {QStringLiteral("media-optical")}},
    {ThemeIcon::MediaTape, {QStringLiteral("media-tape")}},
    {ThemeIcon::MultimediaPlayer, {QStringLiteral("multimedia-player")}},
    {ThemeIcon::NetworkWired, {QStringLiteral("network-wired")}},
    {ThemeIcon::NetworkWireless, {QStringLiteral("network-wireless")}},
    {ThemeIcon::Phone, {QStringLiteral("phone")}},
    {ThemeIcon::Printer, {QStringLiteral("printer")}},
    {ThemeIcon::Scanner, {QStringLiteral("scanner")}},
    {ThemeIcon::VideoDisplay, {QStringLiteral("video-display")}},

    {ThemeIcon::AppointmentMissed, {QStringLiteral("appointment-missed")}},
    {ThemeIcon::AppointmentSoon, {QStringLiteral("appointment-soon")}},
    {ThemeIcon::AudioVolumeHigh, {QStringLiteral("audio-volume-high")}},
    {ThemeIcon::AudioVolumeLow, {QStringLiteral("audio-volume-low")}},
    {ThemeIcon::AudioVolumeMedium, {QStringLiteral("audio-volume-medium")}},
    {ThemeIcon::AudioVolumeMuted, {QStringLiteral("audio-volume-muted")}},
    {ThemeIcon::BatteryCaution, {QStringLiteral("battery-caution")}},
    {ThemeIcon::BatteryLow, {QStringLiteral("battery-low")}},
    {ThemeIcon::DialogError, {QStringLiteral("dialog-error")}},
    {ThemeIcon::DialogInformation, {QStringLiteral("dialog-information")}},
    {ThemeIcon::DialogPassword, {QStringLiteral("dialog-password")}},
    {ThemeIcon::DialogQuestion, {QStringLiteral("dialog-question")}},
    {ThemeIcon::DialogWarning, {QStringLiteral("dialog-warning")}},
    {ThemeIcon::FolderDragAccept, {QStringLiteral("folder-drag-accept")}},
    {ThemeIcon::FolderOpen, {QStringLiteral("folder-open")}},
    {ThemeIcon::FolderVisiting, {QStringLiteral("folder-visiting")}},
    {ThemeIcon::ImageLoading, {QStringLiteral("image-loading")}},
    {ThemeIcon::ImageMissing, {QStringLiteral("image-missing")}},
    {ThemeIcon::MailAttachment, {QStringLiteral("mail-attachment")}},
    {ThemeIcon::MailUnread, {QStringLiteral("mail-unread")}},
    {ThemeIcon::MailRead, {QStringLiteral("mail-read")}},
    {ThemeIcon::MailReplied, {QStringLiteral("mail-replied")}},
    {ThemeIcon::MediaPlaylistRepeat, {QStringLiteral("media-playlist-repeat")}},
    {ThemeIcon::MediaPlaylistShuffle, {QStringLiteral("media-playlist-shuffle")}},
    {ThemeIcon::NetworkOffline, {QStringLiteral("network-offline")}},
    {ThemeIcon::PrinterPrinting, {QStringLiteral("printer-printing")}},
    {ThemeIcon::SecurityHigh, {QStringLiteral("security-high")}},
    {ThemeIcon::SecurityLow, {QStringLiteral("security-low")}},
    {ThemeIcon::SoftwareUpdateAvailable,
     {QStringLiteral("software-update-available"), QStringLiteral("update-medium")}},
    {ThemeIcon::SoftwareUpdateUrgent,
     {QStringLiteral("software-update-urgent"), QStringLiteral("update-high")}},
    {ThemeIcon::SyncError, {QStringLiteral("sync-error")}},
    {ThemeIcon::SyncSynchronizing, {QStringLiteral("sync-synchronizing")}},
    {ThemeIcon::UserAvailable, {QStringLiteral("user-available")}},
    {ThemeIcon::UserOffline, {QStringLiteral("user-offline")}},
    {ThemeIcon::WeatherClear, {QStringLiteral("weather-clear")}},
    {ThemeIcon::WeatherClearNight, {QStringLiteral("weather-clear-night")}},
    {ThemeIcon::WeatherFewClouds, {QStringLiteral("weather-few-clouds")}},
    {ThemeIcon::WeatherFewCloudsNight, {QStringLiteral("weather-few-clouds-night")}},
    {ThemeIcon::WeatherFog, {QStringLiteral("weather-fog")}},
    {ThemeIcon::WeatherShowers, {QStringLiteral("weather-showers")}},
    {ThemeIcon::WeatherSnow, {QStringLiteral("weather-snow")}},
    {ThemeIcon::WeatherStorm, {QStringLiteral("weather-storm")}},
};

QIcon qiconFromTheme(ThemeIcon iconName)
{
    const auto icon = [iconName]() {
        QIcon checkIcon;
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
        // Use an icon from the system theme.
        checkIcon = QIcon::fromTheme(iconName);
#endif
        // Check synonyms until we find something.
        const auto &iconNames = kThemeIcons.at(iconName);
        auto iconNameIt = iconNames.cbegin();
        while (checkIcon.isNull() && iconNameIt != iconNames.cend()) {
            checkIcon = qiconFromTheme(*iconNameIt);
            ++iconNameIt;
        }
        return checkIcon;
    }();
    if (icon.isNull()) {
        qWarning() << "Failed to find icon" << kThemeIcons.at(iconName).front();
    }

    return icon;
}

} // namespace patchman
