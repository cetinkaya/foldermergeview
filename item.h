#ifndef ITEM_H
#define ITEM_H

#include <QVector>
#include <QVariant>
#include <QFileInfo>
#include <QIcon>

class Item
{
public:
    Item(const QVector<QVariant> &data, Item *parent);

    void appendChild(Item *child);
    Item *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    Item *parentItem();
    QVariant icon();

private:
    QVector<QVariant> m_itemData;
    Item *m_parentItem;
    QVector<Item*> m_childItems;
};

#endif // ITEM_H
