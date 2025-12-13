/**
 * Copyright (c) 2013, James Nutaro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies, 
 * either expressed or implied, of the FreeBSD Project.
 *
 * Bugs, comments, and questions can be sent to nutaro@gmail.com
 */
#include "adevs.h"
#include "Autonode.h"
#include "Deployer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;
using namespace adevs;

int main(int argc, char** argv)
{
    int env_length = 20;
    int env_width = 20;
    
    unsigned int seed = -1; // Set seed iff seed > 0. Else random.

    std::ofstream dataFile("Plots/LCANsim_v1-1_data.csv");
    dataFile<<"Node, x_pos, y_pos, target_x, target_y\n";

    // dynamic list of autonomous nodes
    vector<Autonode*> nodeList;

    // Create a digraph model with deployer and node models
    Deployer* deployer = new Deployer(seed, env_length, env_width, nodeList);
    SimpleDigraph<int>* model = new SimpleDigraph<int>();
    model->add(deployer);
    for (int i=0; i < nodeList.size(); i++)
    {
        model->add(nodeList[i]);
    }
    for (int a=0; a < nodeList.size(); a++)
    {
        for (int b=0; b < nodeList.size(); b++)
        {
            if (a != b)
            {
                model->couple(nodeList[a], nodeList[b]);
            }
        }
    }

    Simulator<int>* sim = new Simulator<int>(model);
    int sim_length = 20; // runtime of sim in seconds
    //cout<<"Simulation Start Time: "<<sim->nextEventTime()<<endl;
    while (sim->nextEventTime() < sim_length)
    {
        sim->execNextEvent();
        for (int i=0; i<nodeList.size(); i++)
        {
            //cout<<"Node "<<nodeList[i]->get_id()<<" is at location ("<<nodeList[i]->get_xpos()<<", "<<nodeList[i]->get_ypos()<<") and moving to target ("<<nodeList[i]->get_target_x()<<", "<<nodeList[i]->get_target_y()<<")"<<endl;
            dataFile<<nodeList[i]->get_id()<<", "<<nodeList[i]->get_xpos()<<", "<<nodeList[i]->get_ypos()<<", "<<nodeList[i]->get_target_x()<<", "<<nodeList[i]->get_target_y()<<"\n";
        }
        //cout<<"Sim Event Time: "<<sim->nextEventTime()<<endl;
    }
    cout<<"\nSimulation exited."<<endl;
    dataFile.close();
    return 0;
}