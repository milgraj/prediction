#include <QCoreApplication>
#include <QPair>
#include <datastore.hpp>

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  // 1. Read data
  DataStore data_store;
  data_store.readItems(argv[1]);
  data_store.readTrainData(argv[2]);

  // 2. Calculate best parameters
  int bins = 11;
  QVector<float> errors(bins, 0.0f);
  for(int i = 0; i < 100/*data_store.items.size()*/; ++i)
  {
    for(int f = 0; f < bins; ++f)
    {
      float prediction_weight = float(f) / (bins - 1);
      float average_weight = 1.0f - float(f) / (bins - 1);
      errors[f] += data_store.predictMonthlySale_totalError(data_store.items[i], prediction_weight, average_weight);
    }
    qWarning() << "Progress" << float(i) / float(data_store.items.size());
  }

  int min_idx = 0;
  for(int i = 0; i < errors.size(); ++i)
  {
    if(errors[i] < errors[min_idx])
    {
      min_idx = i;
    }
    qWarning() << "f" << float(i) / (bins - 1) << ":" << 1.0f - float(i) / (bins - 1) << "error" << errors[i];
  }

  // 3. Calculate final prediction error
  qWarning() << "FINAL PREDICTION ERROR";
  int total_error = 0;
  int total_sold = 0;
  for(int i = 0; i < data_store.items.size(); ++i)
  {
    QPair<int, int> final_prediction_error = data_store.monthlySalePredictionError(data_store.items[i], min_idx / (bins - 1), 1.0f - float(min_idx) / (bins - 1));
    total_error += final_prediction_error.first;
    total_error += final_prediction_error.second;
    qWarning() << "Item ID" << data_store.items[i] << ", error" << final_prediction_error.first << ", correct value" << final_prediction_error.second;
  }
  qWarning() << "Correct prediction: " << 1.0f - float(total_error) / float(total_sold);

   4. Prepare esults for submission
  qWarning() << "SUBMISSION";
  qWarning() << "ID,item_cnt_month";
  for(int i = 0; i < data_store.items.size(); ++i)
  {
    int final_prediction_error = data_store.predictMonthlySale(data_store.items[i], min_idx / (bins - 1), 1.0f - float(min_idx) / (bins - 1));
    qWarning().nospace() << data_store.items[i] << "," << final_prediction_error;
  }

  return a.exec();
}
