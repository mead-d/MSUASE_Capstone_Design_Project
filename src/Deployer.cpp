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
#include "Deployer.h"
#include <cmath>
using namespace adevs;
using namespace std;

Deployer::Deployer(unsigned int seed, int env_length, int env_width, vector<Autonode*>& nodeList):Atomic<int>(),
env_length(env_length),
env_width(env_width)
{
    state = EVACUATING;
    if (seed > 0)
    {
        srand(seed);
    }
    vector<int> formation = {-2,2,0,2,2,2,-2,-2,0,-2,2,-2};
    for (int i=0; i < nNodes; i++)
    {
        // Create autonode and append to vector
        nodeList.push_back(new Autonode(i+1, (rand() % env_length - env_length/2), (rand() % env_width - env_width/2), formation));

        // Add autonode activation time (1 sec)
        nodeList[i]->set_time(1.0);
    }
    //cout<<"Autonodes deployed: "<<nodeList.size()<<endl;
    state = STANDBY;
}

// Internal transition
void Deployer::delta_int(){}
// External transition. Will execute when batch of new nodes ordered.
void Deployer::delta_ext(double e, const adevs::Bag<int>& xb) {}
// Confluent transition.
void Deployer::delta_conf(const adevs::Bag<int>& xb)
{
    delta_int();
    delta_ext(0.0, xb);
}
// Output function produces the deployed nodes
void Deployer::output_func(adevs::Bag<int>& yb) {}
// Time advance function.
double Deployer::ta()
{
    if (state == EVACUATING)
    {
        delta_time = 1.0;
    }
    else if (state == STANDBY)
    {
        delta_time = DBL_MAX;
    }
    return delta_time;
}
// Garbage collection
void Deployer::gc_output(adevs::Bag<int>&){}

DeployerState Deployer::getState(){return state;}