/**
 * @file D192RackPreview.h
 *
 * @author Dan Keenan
 * @date 9/27/25
 * @copyright GNU GPLv3
 */

#ifndef SRC_PATCHMAN_D192_D192RACKPREVIEW_H
#define SRC_PATCHMAN_D192_D192RACKPREVIEW_H

#include "patchlib/D192.h"

#include <QTableView>

namespace patchman {

namespace detail {
class D192RackPreviewModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit D192RackPreviewModel(D192Rack *rack, QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  [[nodiscard]] QModelIndex lugIndex(int lug) const;

private:
  D192Rack *rack_;

private Q_SLOTS:
  void lugChanged(unsigned int lug);
};
} // namespace detail

class D192RackPreview : public QTableView {
  Q_OBJECT
public:
  explicit D192RackPreview(D192Rack *rack, QWidget *parent = nullptr);

public Q_SLOTS:
  void selectLug(int lug);

private:
  detail::D192RackPreviewModel *model_;
};

} // namespace patchman

#endif // SRC_PATCHMAN_D192_D192RACKPREVIEW_H
