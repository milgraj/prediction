#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QString>
#include <QVector>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>

#include "./onesale.hpp"

class DataStore : public QObject
{
  Q_OBJECT
public:
  QVector<OneSale> sale;
  QVector<unsigned int> items;

  explicit DataStore(QObject *parent = nullptr);
  void readTrainData(QString inPath);
  void readItems(QString inPath);
  QVector<QPair<int, int> > getSaleForProduct(unsigned int inItemID, QString resolution = "day");
  QPair<int, int> monthlySalePredictionError(unsigned int inItemID, float inPrediction, float inAverage);
  int predictMonthlySale_totalError(unsigned int inItemID, float inPrediction, float inAverage);
  int predictMonthlySale(unsigned int inItemID, float inPrediction, float inAverage);

private:

signals:

public slots:
};

#endif // DATASTORE_HPP
