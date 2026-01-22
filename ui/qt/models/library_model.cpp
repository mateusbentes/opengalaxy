#include "library_model.h"

namespace opengalaxy {
namespace ui {

LibraryModel::LibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    gameNames << "Game 1" << "Game 2" << "Game 3";
}

LibraryModel::~LibraryModel()
{
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return gameNames.size();
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= gameNames.size())
        return QVariant();

    if (role == GameNameRole)
        return gameNames.at(index.row());

    return QVariant();
}

} // namespace ui
} // namespace opengalaxy
