/**
 * @file EnrRackPreview.h
 *
 * @author Dan Keenan
 * @date 9/27/25
 * @copyright GNU GPLv3
 */

#ifndef SRC_PATCHMAN_ENR_ENRRACKPREVIEW_H
#define SRC_PATCHMAN_ENR_ENRRACKPREVIEW_H

#include "patchlib/Enr.h"

#include <QListView>

namespace patchman {

namespace detail {
class EnrRackPreviewModel : public QAbstractListModel {
  Q_OBJECT
public:
  explicit EnrRackPreviewModel(EnrRack *rack, QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
  EnrRack *rack_;

private Q_SLOTS:
  void lugChanged(unsigned int lug);
};
} // namespace detail

class EnrRackPreview : public QListView {
  Q_OBJECT
public:
  explicit EnrRackPreview(EnrRack *rack, QWidget *parent = nullptr);

public Q_SLOTS:
  void selectLugs(const std::set<int> &lugs);

private:
  detail::EnrRackPreviewModel *model_;
};

} // namespace patchman

#endif // SRC_PATCHMAN_ENR_ENRRACKPREVIEW_H
