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
#include <cmath>
#include <vector>
#include "TargetSelector.h"

using namespace adevs;
using namespace std;

Autonode::Autonode(int id, std::vector<Point>& swarm_positions, std::vector<std::vector<Point>>& formations):Atomic<Output>(),
id(id),
swarm_positions(swarm_positions),
formations(formations)
{
    set_formation(formations[0]);
    set_state(DEPLOYING);
    x_cur = swarm_positions[id].x;
    y_cur = swarm_positions[id].y;
}

// Internal transition function.
void Autonode::delta_int()
{
    // Identify target position in formation -> shortest distance from current position.
    if (state == DEPLOYING)
    {
        time_deployed = time(NULL);
        //std::cout << "Node " << id << " sees " << formation.size() << " targets." << std::endl;

        Data target_index = getMyTarget_greedy(id, swarm_positions, formation);
        if (target_index.target != -1)
        {
            x_target = formation[target_index.target].x;
            y_target = formation[target_index.target].y;
            cost_matrix = target_index.cost_matrix;
            set_state(FORMING);
            time_selection = time(NULL) + delta_time;
            time_start_maneuver = time_selection - time_deployed;
            //cout<<"Target position for node "<<id<<": ("<<x_target<<", "<<y_target<<")"<<endl;
        }
    }
    else if (state == FORMING)
    {
        //cout<<"Node "<<id<<" moving into target formation."<<endl;
        dist_2_xtarget = x_target - x_cur;
        dist_2_ytarget = y_target - y_cur;
        distance = sqrt(pow(dist_2_xtarget,2)+pow(dist_2_ytarget,2));
        vector<double> delta_pos = {speed*dist_2_xtarget/distance, speed*dist_2_ytarget/distance};
        if (sqrt(pow(delta_pos[0],2)+pow(delta_pos[1],2)) >= distance)
        {
            x_cur = x_target;
            y_cur = y_target;
            time_active += delta_time;
        } else
        {
            x_cur = x_cur + delta_pos[0];
            y_cur = y_cur + delta_pos[1];
            time_active += delta_time;
        }
        if (x_cur == x_target && y_cur == y_target)
        {
            set_state(WAITING);
            time_waiting = 0;
            time_onStation = time_active + time_start_maneuver;
            time_active = 0.0;
        }
    }
    else if (state == WAITING && !swarm_state.empty())
    {
        int nodes_waiting = 0;
        for (int i=0; i < swarm_state.size(); i++)
        {
            if (swarm_state[i] == WAITING)
            {
                nodes_waiting++;
            }
        }
        if (nodes_waiting == swarm_state.size() && time_waiting < time_idle)
        {
            time_waiting += delta_time;
        }
        else if (nodes_waiting == swarm_state.size() && time_waiting == time_idle)
        {
            for (int i=0; i < formations.size(); i++)
            {
                if (formations[i] == formation)
                {
                    if (i == formations.size()-1)
                    {
                        set_formation(formations[0]);
                    } else
                    {
                        set_formation(formations[i+1]);
                    }
                    break;
                }
            }
        }
    }
}

// External transition function.
void Autonode::delta_ext(double e, const adevs::Bag<Output>& xb)
{
    if (!swarm_positions.empty())
    {
        swarm_positions.clear();
    }
    if (!swarm_state.empty())
    {
        swarm_state.clear();
    }
    Bag<Output>::const_iterator i = xb.begin();
    for (; i != xb.end(); i++)
    {
        for (int k=0; k < xb.size(); k++)
        {
            if ((*i).id == k)
            {
                swarm_positions.push_back((*i).point);
                swarm_state.push_back((*i).state);
            }
        }
    }
}

// Confluent transition function.
void Autonode::delta_conf(const adevs::Bag<Output>& xb)
{
    delta_int();
    delta_ext(0.0, xb);
}

// Output function.
void Autonode::output_func(adevs::Bag<Output>& yb)
{
    yb.insert(Output{get_id(), getState(), Point{get_xpos(), get_ypos()}});
}

// Time advance function.
double Autonode::ta()
{
    if (state == DEPLOYING || state == FORMING)
    {
        delta_time = 1.0;
    }
    else if (state == WAITING)
    {
        delta_time = DBL_MAX;
    }
    return delta_time;
}

// Garbage collection
void Autonode::gc_output(adevs::Bag<Output>&){}

Autonode::~Autonode(){}
