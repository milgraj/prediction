#include "datastore.hpp"

DataStore::DataStore(QObject *parent) : QObject(parent)
{

}

void
DataStore::readTrainData(QString inPath)
{
  QFile file(inPath);
  if (!file.open(QIODevice::ReadOnly))
  {
    exit(-1);
  }

  unsigned int counter = 0;
  int block = -1;
  QStringList values;
  while(!file.atEnd())
  {
    QString line = file.readLine();
    if(counter++ > 0)
    {
      values = line.split(',');
      if(values[1].toInt() != block)
      {
        qWarning() << "Block" << values[1].toInt();
        block = values[1].toInt();
      }
      sale.push_back(
                      OneSale (
                                QDate (
                                        values[0].split(".")[2].toInt()
                                      , values[0].split(".")[1].toInt()
                                      , values[0].split(".")[0].toInt()
                                      )
                              , values[1].toUInt()
                              , values[2].toUInt()
                              , values[3].toUInt()
                              , values[4].toFloat()
                              , int(values[5].toFloat())
                              )
                    );
    }
  }
  qWarning() << "Train data reading finished";
}

void
DataStore::readItems(QString inPath)
{
  QFile file(inPath);
  if (!file.open(QIODevice::ReadOnly))
  {
    exit(-1);
  }

  unsigned int counter = 0;
  QStringList values;
  while(!file.atEnd())
  {
    QString line = file.readLine();
    if(counter++ > 0)
    {
      values = line.split(",");
      items.push_back(values[values.length() - 2].toUInt());
    }
  }

  qWarning() << "Iteams reading finished";
}

QVector<QPair<int, int> >
DataStore::getSaleForProduct(unsigned int inItemID, QString resolution)
{
  QVector<QPair<int, int> > result;
  if(resolution == "day")
  {
    for(int i = 0; i < sale.size(); ++i)
    {
      if(sale[i].itemID == inItemID)
      {
        qWarning() << sale[i].dateBlock << sale[i].itemID << sale[i].itemPrice << sale[i].soldItems;
      }
    }
  }
  else if(resolution == "month")
  {
    int monthly_sale = 0;
    unsigned int month = 0;
    for(int i = 0; i < sale.size(); ++i)
    {
      if(month == sale[i].dateBlock)
      {
        if(sale[i].itemID == inItemID)
        {
          monthly_sale += sale[i].soldItems;
        }
      }
      else
      {
        result.push_back(qMakePair(month, monthly_sale));
        month = sale[i].dateBlock;
        monthly_sale = 0;
      }
    }
    result.push_back(qMakePair(month, monthly_sale));
  }
  return result;
}

QPair<int, int>
DataStore::monthlySalePredictionError(unsigned int inItemID, float inPrediction, float inAverage)
{
  QVector<QPair<int, int> > monthly_sale = getSaleForProduct(inItemID, "month");
  cv::KalmanFilter KF(4, 2, 0);

  float data[16] = {1,0.5,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1};
  cv::Mat transition_matrix = cv::Mat(4, 4, CV_32F, data);
  KF.transitionMatrix = transition_matrix;
  cv::Mat measurement(2,1, CV_32F);
  measurement.setTo(cv::Scalar(0));

  KF.statePre.at<float>(0) = 0;
  KF.statePre.at<float>(1) = 0;
  KF.statePre.at<float>(2) = 0;
  KF.statePre.at<float>(3) = 0;
  setIdentity(KF.measurementMatrix);
  setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-4));
  setIdentity(KF.measurementNoiseCov, cv::Scalar::all(10));
  setIdentity(KF.errorCovPost, cv::Scalar::all(.1));

  //qWarning() << "monthy sale size" << monthly_sale.size();
  for(int i = 0; i < monthly_sale.size() - 1; ++i)
  {
    // First predict, to update the internal statePre variable
    cv::Mat prediction = KF.predict();

    measurement.at<float>(0) = 0;
    measurement.at<float>(1) = monthly_sale[i].second;

    // The update phase
    cv::Mat estimated = KF.correct(measurement);
  }
  cv::Mat prediction = KF.predict();
  float predicted_sale = prediction.at<float>(0, 1);
  float correction = (monthly_sale[monthly_sale.size()-2].second + monthly_sale[monthly_sale.size()-3].second) / 2;
  return qMakePair(qAbs(int(float(predicted_sale) * inPrediction + float(correction) * inAverage) - monthly_sale.back().second), monthly_sale.back().second);
}

int
DataStore::predictMonthlySale_totalError(unsigned int inItemID, float inPrediction, float inAverage)
{
  int error = 0;
  QVector<QPair<int, int> > monthly_sale = getSaleForProduct(inItemID, "month");
  cv::KalmanFilter KF(4, 2, 0);

  float data[16] = {1,0.5,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1};
  cv::Mat transition_matrix = cv::Mat(4, 4, CV_32F, data);
  KF.transitionMatrix = transition_matrix;
  cv::Mat measurement(2,1, CV_32F);
  measurement.setTo(cv::Scalar(0));

  KF.statePre.at<float>(0) = 0;
  KF.statePre.at<float>(1) = 0;
  KF.statePre.at<float>(2) = 0;
  KF.statePre.at<float>(3) = 0;
  setIdentity(KF.measurementMatrix);
  setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-4));
  setIdentity(KF.measurementNoiseCov, cv::Scalar::all(10));
  setIdentity(KF.errorCovPost, cv::Scalar::all(.1));

  for(int i = 0; i < monthly_sale.size(); ++i)
  {
    // First predict, to update the internal statePre variable
    cv::Mat prediction = KF.predict();
    float predicted_sale = prediction.at<float>(0, 1);

    float correction = 0.0f;
    if(i > 1)
      correction = (monthly_sale[i-1].second + monthly_sale[i-2].second) / 2;
    //qWarning() << int(predicted_sale) << int(float(predicted_sale) * inPrediction + float(correction) * inAverage) << monthly_sale[i].second;

    measurement.at<float>(0) = 0;
    measurement.at<float>(1) = monthly_sale[i].second;

    // The update phase
    cv::Mat estimated = KF.correct(measurement);

    error += qAbs(int(float(predicted_sale) * inPrediction + float(correction) * inAverage) - monthly_sale[i].second);
  }
  return error;
}

int
DataStore::predictMonthlySale(unsigned int inItemID, float inPrediction, float inAverage)
{
  QVector<QPair<int, int> > monthly_sale = getSaleForProduct(inItemID, "month");
  cv::KalmanFilter KF(4, 2, 0);

  float data[16] = {1,0.5,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1};
  cv::Mat transition_matrix = cv::Mat(4, 4, CV_32F, data);
  KF.transitionMatrix = transition_matrix;
  cv::Mat measurement(2,1, CV_32F);
  measurement.setTo(cv::Scalar(0));

  KF.statePre.at<float>(0) = 0;
  KF.statePre.at<float>(1) = 0;
  KF.statePre.at<float>(2) = 0;
  KF.statePre.at<float>(3) = 0;
  setIdentity(KF.measurementMatrix);
  setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-4));
  setIdentity(KF.measurementNoiseCov, cv::Scalar::all(10));
  setIdentity(KF.errorCovPost, cv::Scalar::all(.1));

  //qWarning() << "monthy sale size" << monthly_sale.size();
  for(int i = 0; i < monthly_sale.size(); ++i)
  {
    // First predict, to update the internal statePre variable
    cv::Mat prediction = KF.predict();

    measurement.at<float>(0) = 0;
    measurement.at<float>(1) = monthly_sale[i].second;

    // The update phase
    cv::Mat estimated = KF.correct(measurement);
  }
  cv::Mat prediction = KF.predict();
  float predicted_sale = prediction.at<float>(0, 1);
  float correction = (monthly_sale[monthly_sale.size()-1].second + monthly_sale[monthly_sale.size()-2].second) / 2;
  return int(float(predicted_sale) * inPrediction + float(correction) * inAverage);
}
