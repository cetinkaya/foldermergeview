#include "item.h"
#include <QIcon>
#include <QDir>

Item::Item(const QVector<QVariant> &data, Item *parent)
    : m_itemData(data), m_parentItem(parent)
{

}

void Item::appendChild(Item *child)
{
    this->m_childItems.append(child);
}

Item *Item::child(int row)
{
    return this->m_childItems.at(row);
}

int Item::childCount() const
{
    return this->m_childItems.count();
}

int Item::columnCount() const
{
    return m_itemData.count();
}

QVariant Item::data(int column) const
{
    return m_itemData.at(column);
}

int Item::row() const
{
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<Item*>(this));
    }
    return 0;
}

Item *Item::parentItem()
{
    return this->m_parentItem;
}

QVariant Item::icon()
{
    if (this->m_itemData.at(2).toString().length() == 0){
        return QVariant(QIcon(":/files/icons/folder.svg"));
    } else {
        return QVariant(QIcon(":/files/icons/file.svg"));
    }
}
