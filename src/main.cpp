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
#include "TargetSelector.h"
#include "SimulationWidget.h"
#include <QApplication>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
using namespace adevs;

std::vector<Point> pointsOnCircle(int n) {
    const double spacing = 10.0;
    std::vector<Point> points;
    
    if (n <= 0) return points;

    // 1. Calculate the required perimeter and radius
    double perimeter = n * spacing;
    double radius = perimeter / (2.0 * M_PI);

    // 2. Generate points using polar coordinates
    for (int i = 0; i < n; ++i) {
        // Angle in radians: (current_point / total_points) * 2PI
        double angle = i * (2.0 * M_PI / n);
        
        double x = floor(radius * std::cos(angle));
        double y = floor(radius * std::sin(angle));
        
        points.push_back({x, y});
    }
    return points;
}

std::vector<Point> pointsOnRectangle(int n, int segmentsWide) {
    const double spacing = 10.0;
    std::vector<Point> points;

    // Validation: n must be even, at least 4, and wide enough to accommodate segmentsWide
    if (n % 2 != 0)
    {
        points.push_back({0, 0});
    }
    else if (n < 4 || segmentsWide >= n / 2) {
        std::cerr << "Invalid point count for a rectangle." << std::endl;
        return points;
    }

    int segmentsHigh = (n / 2) - segmentsWide;
    double width = segmentsWide * spacing;
    double height = segmentsHigh * spacing;

    double offsetX = width / 2.0;
    double offsetY = height / 2.0;

    // Traverse the perimeter
    double curX = 0, curY = 0;

    for (int i = 0; i < n; ++i) {
        points.push_back({curX - offsetX, curY - offsetY});

        if (i < segmentsWide) { 
            curX += spacing; // Top edge
        } else if (i < segmentsWide + segmentsHigh) { 
            curY += spacing; // Right edge
        } else if (i < (2 * segmentsWide) + segmentsHigh) { 
            curX -= spacing; // Bottom edge
        } else { 
            curY -= spacing; // Left edge
        }
    }

    return points;
}

int main(int argc, char** argv)
{
    bool enable_gui = true;

    int sim_length = 60; // runtime of sim in seconds
    int env_length = 100;
    int env_width = 100;
    int nNodes = 20;
    unsigned int seed = -1; // Set seed iff seed > 0. Else random.
    srand(time(NULL));
    
    // dynamic list of autonomous nodes
    vector<Autonode*> nodeList;
    nodeList.reserve(nNodes+1); // Ensure pointers in QT app are not dangling pointers
    
    // Create a digraph model with deployer and node models
    
    // dynamic list of target formation
    vector<Point> rand_formation;
    vector<Point> circle_formation;
    vector<Point> rect_formation;

    vector<Point> circPoints = pointsOnCircle(nNodes);
    for (const auto& p : circPoints)
    {
        circle_formation.push_back(p);
    }

    int wide = ceil(nNodes/3.0);
    vector<Point> rectPoints = pointsOnRectangle(nNodes, wide);
    for (const auto& p : rectPoints)
    {
        rect_formation.push_back(p);
    }

    for (int i=0; i < nNodes; i++)
    {
        rand_formation.push_back(Point{(double)(rand() % env_length - env_length/2.0), (double)(rand() % env_width - env_width/2.0)});
    }

    if (circle_formation.empty() || rect_formation.empty() || rand_formation.empty()) return 1;
    vector<vector<Point>> formations = {circle_formation, rand_formation, rect_formation};

    Deployer* deployer = new Deployer(seed, env_length, env_width, nNodes, nodeList, formations);
    SimpleDigraph<Output>* model = new SimpleDigraph<Output>();
    ofstream formationData("Plots/LCANsim_v1-3-animus_formData.csv");
    formationData<<"Index, x_coord, y_coord"<<endl;
    formationData<<"Circle Formation"<<endl;
    for (int i=0; i < circle_formation.size(); i++)
    {
        formationData<<i<<", "<<circle_formation[i].x<<", "<<circle_formation[i].y<<endl;
    }
    formationData<<"Rectangle Formation"<<endl;
    for (int i=0; i < rect_formation.size(); i++)
    {
        formationData<<i<<", "<<rect_formation[i].x<<", "<<rect_formation[i].y<<endl;
    }
    formationData<<"Random Formation"<<endl;
    for (int i=0; i < rand_formation.size(); i++)
    {
        formationData<<i<<", "<<rand_formation[i].x<<", "<<rand_formation[i].y<<endl;
    }
    formationData<<"Node Targets"<<endl;
    for (int i=0; i < nodeList.size(); i++)
    {
        formationData<<nodeList[i]->get_id()<<", "<<nodeList[i]->get_target_x()<<", "<<nodeList[i]->get_target_y()<<endl;
    }
    formationData.close();

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
    
    Simulator<Output>* sim = new Simulator<Output>(model);
    
    if (enable_gui)
    {
        QApplication app(argc, argv);
        
        SimulationWidget* window = new SimulationWidget(sim, nodeList, env_length, env_width, sim_length);
        window->resize(800,800);
        window->setWindowTitle("LCANsim Animation");
        window->show();
        
        return app.exec();
    }
    else
    {
        ofstream dataFile("Plots/LCANsim_v1-3-animus_data.csv");
        dataFile<<"Node, x_pos, y_pos, target_x, target_y"<<endl;
        while (sim->nextEventTime() < sim_length)
        {
            sim->execNextEvent();
            for (int i=0; i<nodeList.size(); i++)
            {
                //cout<<"Node "<<nodeList[i]->get_id()<<" is at location ("<<nodeList[i]->get_xpos()<<", "<<nodeList[i]->get_ypos()<<") and moving to target ("<<nodeList[i]->get_target_x()<<", "<<nodeList[i]->get_target_y()<<")"<<endl;
                dataFile<<nodeList[i]->get_id()<<", "<<nodeList[i]->get_xpos()<<", "<<nodeList[i]->get_ypos()<<", "<<nodeList[i]->get_target_x()<<", "<<nodeList[i]->get_target_y()<<endl;
            }
            //cout<<"Sim Event Time: "<<sim->nextEventTime()<<endl;
        }
        cout<<"\nSimulation exited."<<endl;
        dataFile.close();
    }

    ofstream costMatrixFile("Plots/cost_matrices_v1-3-animus.csv");
    costMatrixFile<<"Node, Target id, Square Distance"<<endl;
    ofstream timeFile("Plots/LCANsim_v1-3-animus_time.csv");
    timeFile<<"Node, Time Deployment, Time on Station"<<endl;
    for (int i=0; i<nodeList.size(); i++)
    {
        costMatrixFile<<"Node id: "<<nodeList[i]->get_id()<<endl;
        vector<Edge> matrix = nodeList[i]->get_cost_matrix();
        for (const auto& edge : matrix)
        {
            costMatrixFile<<"["<<edge.node_id<<", "<<edge.target_id<<", "<<edge.dist_sq<<endl;
        }
        timeFile<<nodeList[i]->get_id()<<", "<<nodeList[i]->get_time_deploy()<<", "<<nodeList[i]->get_time_onStation()<<endl;
    }
    costMatrixFile.close();
    timeFile.close();
    return 0;
}
