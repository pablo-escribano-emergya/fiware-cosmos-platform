/* ****************************************************************************
*
* FILE                     WorkerContainer.cpp
*
* DESCRIPTION			   Widget to visualize samson workers' data
*
*/

#include "WorkerContainer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>

namespace samson{

WorkerContainer::WorkerContainer(QWidget* parent): QWidget(parent)
{
    mainLayout = new QVBoxLayout();
}

void WorkerContainer::setData(std::vector<WorkerData>& workersData)
{
    for (unsigned int i = 0; i<workersData.size(); i++)
    {
        WorkerViewer* workerTmp = findWorker(workersData[i].worker_id);
        if(workerTmp)
        {
            //there is already a widget for this worker. Update data.
            workerTmp->setData(workersData[i]);
            
        }
        else
        {
            //create new widget
            workerTmp =  new WorkerViewer(workersData[i].worker_id);
            workerViewers.push_back(workerTmp);
            mainLayout->addWidget(workerTmp);
            mainLayout->activate();
        }
    }
}

WorkerViewer* WorkerContainer::findWorker(std::string id)
{
    bool found = false;
    WorkerViewer* workerTmp = NULL;
    for(unsigned int i = 0; i< workerViewers.size() && !found; i++)
    {
        if (workerViewers[i]->worker_id == id)
        {
            found = true;
            workerTmp = workerViewers[i];
        }
    }
    return workerTmp;
}  
 

} //namespace
