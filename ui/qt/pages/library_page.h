#ifndef LIBRARY_PAGE_H
#define LIBRARY_PAGE_H

#include <QWidget>
#include <QGridLayout>
#include "../widgets/game_card.h"

class LibraryPage : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryPage(QWidget *parent = nullptr);

private:
    void setupGrid();
    QGridLayout *gridLayout;
};

#endif // LIBRARY_PAGE_H
