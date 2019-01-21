#include "onesale.hpp"

OneSale::OneSale(QObject *parent) : QObject(parent)
{

}

OneSale::OneSale(
                  QDate inDate
                , unsigned int inDateBlock
                , unsigned int inShopID
                , unsigned int inItemID
                , float inItemPrice
                , int inSoldItems
                , QObject *parent
                )
: QObject(parent)
, date(inDate)
, dateBlock(inDateBlock)
, shopID(inShopID)
, itemID(inItemID)
, itemPrice(inItemPrice)
, soldItems(inSoldItems)
{

}

OneSale::OneSale(const OneSale &c)
: QObject(c.parent())
{
  this->date = c.date;
  this->dateBlock = c.dateBlock;
  this->shopID = c.shopID;
  this->itemID = c.itemID;
  this->itemPrice = c.itemPrice;
  this->soldItems = c.soldItems;
}
