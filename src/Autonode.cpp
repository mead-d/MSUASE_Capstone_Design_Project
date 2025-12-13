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
using namespace adevs;
using namespace std;

Autonode::Autonode(int id, int x, int y, std::vector<int> formation):Atomic<int>(),
id(id),
formation(formation)
{
    state=DEPLOYING;
    start_pos[0] = x;
    start_pos[1] = y;
}

// Internal transition function.
void Autonode::delta_int()
{
    // Identify target position in formation -> shortest distance from current position.
    if (state == DEPLOYING)
    {
        x_target = formation[id*2-1];
        y_target = formation[id*2];
        x_cur = start_pos[0];
        y_cur = start_pos[1];
        state = FORMING;
        cout<<"Target position for node "<<id<<": ("<<x_target<<", "<<y_target<<")"<<endl;
    }
    else if (state == FORMING)
    {
        cout<<"Node "<<id<<" moving into target formation."<<endl;
        dist_2_xtarget = x_target - x_cur;
        dist_2_ytarget = y_target - y_cur;
        distance = sqrt(pow(dist_2_xtarget,2)+pow(dist_2_ytarget,2));
        vector<double> delta_pos = {speed*dist_2_xtarget/distance, speed*dist_2_ytarget/distance};
        if (sqrt(pow(delta_pos[0],2)+pow(delta_pos[1],2)) >= distance)
        {
            x_cur = x_target;
            y_cur = y_target;
        } else
        {
            x_cur = x_cur + delta_pos[0];
            y_cur = y_cur + delta_pos[1];
        }
        if (x_cur == x_target && y_cur == y_target)
        {
            state = WAITING;
        }
    }
}

// External transition function.
void Autonode::delta_ext(double e, const adevs::Bag<int>& xb){}

// Confluent transition function.
void Autonode::delta_conf(const adevs::Bag<int>& xb)
{
    delta_int();
    delta_ext(0.0, xb);
}

// Output function.
void Autonode::output_func(adevs::Bag<int>& yb){}

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
void Autonode::gc_output(adevs::Bag<int>&)
{

}

Autonode::~Autonode(){}

NodeState Autonode::getState(){return state;}

double Autonode::get_xpos()
{
    return x_cur;
}
double Autonode::get_ypos()
{
    return y_cur;
}
double Autonode::get_target_x()
{
    return x_target;
}
double Autonode::get_target_y()
{
    return y_target;
}
int Autonode::get_id()
{
    return id;
}
void Autonode::set_time(double time)
{
    time = time;
}
void Autonode::set_formation(vector<int> formation)
{
    formation = formation;
}