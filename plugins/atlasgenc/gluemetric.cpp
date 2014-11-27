#include "gluemetric.h"
#include "gluetask.h"

#include <QtCore/QLinkedList>

#include <cstdio>

GlueMetric::GlueMetric()
{

}

GlueMetric::~GlueMetric()
{

}

double GlueMetric::maxMerge(const QVector<GlueEntry>& entries, GlueOrder order, int index)
{
    bool maxexists = false;
    double max = 0;
    //printf("Performing max merge: ");
    for(size_t i = 0; i < order.Images.size(); i++)
    {
        double maxcandidate = entries[order.Images[i]][index];
        if (maxexists == false || maxcandidate > max) {
            //printf("%lf > %lf\n", maxcandidate, max);
            max = maxcandidate;
            maxexists = true;
        } else {
            //printf("%lf < %lf\n", maxcandidate, max);
        }
    }
    return max;
}

double GlueMetric::sumMerge(const QVector<GlueEntry>& entries, GlueOrder order, int index)
{
    double result = 0;
    for(size_t i = 0; i < order.Images.size(); i++)
    {
        result += entries[order.Images[i]][index];

    }
    return result;
}

QVector<GlueOrder> GlueMetric::findMinMetricOrder(const QVector<GlueEntry>& entries)
{
    QVector<GlueOrder> result;
    MaybeNumber min;
    for(size_t i1 = 0; i1 < entries.size(); i1++)
    {
        for(size_t i2 = i1 + 1; i2 < entries.size(); i2++)
        {
            QVector<GlueOrder> combinations;
            combinations << GlueOrder(i1, i2, HORIZONTAL);
            combinations << GlueOrder(i1, i2, VERTICAL);

            for(size_t j = 0; j < combinations.size(); j++)
            {
                double metric = this->getMetric(entries, combinations[j]);
                if (min > metric)
                {
                    min.setValue(metric);
                    result = QVector<GlueOrder>();
                    result << combinations[j];
                }
                else
                {
                    if (min == metric)
                    {
                        result << combinations[j];
                    }
                }
            }
        }
    }
    return result;
}

QVector<GlueMetric::OrdersAndSize> GlueMetric::findOrder(const QVector<GlueEntry> & entries)
{
    QVector<GlueMetric::OrdersAndSize> finishedtasks;
    if (entries.size())
    {
        if (entries.size() == 1) {
            const GlueEntry & entry = entries[0];
            finishedtasks << GlueMetric::OrdersAndSize(
                QVector<GlueOrder>(),
                entry
            );
        }
        else
        {
            QLinkedList<GlueTask> tasks;
            tasks << GlueTask(entries, QVector<GlueOrder>());
            while(tasks.size())
            {
                GlueTask task = *(tasks.begin());
                tasks.removeFirst();
                if (task.Entries.size() == 1)
                {
                    finishedtasks << GlueMetric::OrdersAndSize(task.Orders, task.Entries[0]);
                }
                else
                {
					QVector<GlueOrder> orders = this->findMinMetricOrder(task.Entries);
					size_t sizeorders = orders.size();
                    for(size_t j = 0; j < sizeorders; j++)
                    {
						GlueTask newtask =  task.applyOrder(orders[j]); 
                        tasks << newtask;
                        char c = ((orders[j].Mode == HORIZONTAL) ? 'H' : 'V');
                        printf("(%d,%d,%c) + {", orders[j].Images[0],orders[j].Images[1],  c);
						for(size_t k = 0; k < task.Orders.size(); k++)
						{
                            printf("(%d,%d,%c) ", task.Orders[k].Images[0], task.Orders[k].Images[1], (task.Orders[k].Mode == HORIZONTAL) ? 'H' : 'V' );
						}
                        printf("}x%d = {", task.Entries.size());

                        for(size_t k = 0; k < newtask.Orders.size(); k++)
                        {
                            printf("(%d,%d,%c) ", newtask.Orders[k].Images[0], newtask.Orders[k].Images[1], (newtask.Orders[k].Mode == HORIZONTAL) ? 'H' : 'V' );
                        }

                        printf("}x%d \n", newtask.Entries.size());
					}
                }
            }
        }
    }
    return finishedtasks;
}

QPair<MaybeNumber, GlueMetric::OrdersAndSize>
GlueMetric::findMinimumOrder(
    const QPair<MaybeNumber, GlueMetric::OrdersAndSize> start,
    const QVector<GlueMetric::OrdersAndSize>  & results
)
{
    MaybeNumber min = start.first;
    GlueMetric::OrdersAndSize resultorder = start.second;
    for(size_t i = 0; i < results.size(); i++)
    {
		double width = results[i].second[0];
		double height = results[i].second[1];
        double max_value = std::max(width, height);
        if (min > max_value)
        {
            resultorder = results[i];
            min.setValue(max_value);
        }
    }
    return QPair<MaybeNumber, GlueMetric::OrdersAndSize>(min, resultorder);
}
