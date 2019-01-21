#ifndef ONESALE_HPP
#define ONESALE_HPP

#include <QObject>
#include <QDate>

class OneSale : public QObject
{
  Q_OBJECT
public:
  QDate date;
  unsigned int dateBlock;
  unsigned int shopID;
  unsigned int itemID;
  float itemPrice;
  int soldItems;

  explicit OneSale(QObject *parent = nullptr);
  explicit OneSale(QDate inDate, unsigned int inDateBlock, unsigned int inShopID, unsigned int inItemID, float inItemPrice, int inSoldItems, QObject *parent = nullptr);
  OneSale(const OneSale &c);

private:
signals:

public slots:
};

#endif // ONESALE_HPP
