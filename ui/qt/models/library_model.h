#ifndef LIBRARY_MODEL_H
#define LIBRARY_MODEL_H

#include <QAbstractListModel>

namespace opengalaxy {
namespace ui {

class LibraryModel : public QAbstractListModel {
    Q_OBJECT

  public:
    explicit LibraryModel(QObject *parent = nullptr);
    ~LibraryModel();

    enum Roles { GameNameRole = Qt::UserRole + 1 };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  private:
    QStringList gameNames;
};

} // namespace ui
} // namespace opengalaxy

#endif // LIBRARY_MODEL_H
