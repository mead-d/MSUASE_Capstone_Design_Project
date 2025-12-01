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
x_pos(x),
y_pos(y),
formation(formation)
{forming=true;}

// Internal transition function.
void Autonode::delta_int()
{
    // Identify target position in formation -> shortest distance from current position.
    if (forming == true)
    {
        double meter;
        x_target = formation[0];
        y_target = formation[1];
        for (int k=2; k < formation.size(); k+=2)
        {
            meter = sqrt(pow((x_target-x_pos),2)+pow((y_target-y_pos),2));
            if (meter < targetDist)
            {
                targetDist = meter;
                x_target = formation[k];
                y_target = formation[k+1];
            }
        }
        forming = false;
    }
    int x_dist = x_target - x_pos;
    int y_dist = y_target - y_pos;
    // Check if the target is to the left of node. Subtract velocity component in x direction.
    if  (x_dist < 0)
    {
        if (x_dist < speed)
        {
            x_pos = x_target;
        }
        else
        {
            x_pos -= speed;
        }
    }
    // Check if the target is to the right of node. Add velocity component in x direction.
    else if (x_dist > 0)
    {
        if (x_dist < speed)
        {
            x_pos = x_target;
        }
        else
        {
            x_pos += speed;
        }
    }
    // Check if the target is below the node. Subtract velocity component in the y direction.
    if (y_dist < 0)
    {
        if (y_dist < speed)
        {
            y_pos = y_target;
        }
        else
        {
            y_pos -= speed;
        }
    }
    // Check if the target is above the node. Add velocity componenet in the y direction.
    else if (y_dist > 0)
    {
        if (y_dist < speed)
        {
            y_pos = y_target;
        }
        else
        {
            y_pos += speed;
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
    return 1.0;
    //return time;
}

// Garbage collection
void Autonode::gc_output(adevs::Bag<int>&){}

Autonode::~Autonode(){}

double Autonode::get_xpos()
{
    return x_pos;
}
double Autonode::get_ypos()
{
    return y_pos;
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
void Autonode::set_formation(std::vector<int> formation)
{
    formation = formation;
}