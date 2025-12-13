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

#ifndef _Autonode_h_
#define _Autonode_h_
#include "adevs.h"
#include <vector>

// State structure of a node
typedef enum
{
    DEPLOYING,
    FORMING,
    WAITING
}
NodeState;


 /**
  * @brief This class models an autonomous node that exists in a 2-dimensional plane.
  * The node is one of many in a larger swarm. Each node stands guard passively sensing in the assigned environment.
  */
class Autonode: public adevs::Atomic<int>
{
    public:
        Autonode();
        Autonode(int id, int x, int y, std::vector<int> formation);

        // Internal transition function.
        void delta_int();
        // External transition function.
        void delta_ext(double e, const adevs::Bag<int>& xb);
        // Confluent transition function.
        void delta_conf(const adevs::Bag<int>& xb);
        // Output function.
        void output_func(adevs::Bag<int>& yb);
        // Time advance function.
        double ta();
        // Garbage collection
        void gc_output(adevs::Bag<int>&);
        // Destructor
        ~Autonode();
        
        // Getters
        NodeState getState();
        double get_xpos();
        double get_ypos();
        double get_target_x();
        double get_target_y();
        int get_id();
        // Setters
        void set_time(double time);
        void set_formation(vector<int> x);
    protected:
        // State of the node
        NodeState state;
    private:
        // Time until next action
        double delta_time = 1.0;
        // unique id
        int id;
        // Node position & target position
        vector<double> start_pos = {0,0};
        double x_cur, y_cur, dist_2_xtarget, dist_2_ytarget, distance;
        int x_target=0, y_target=0;
        // Array of swarm target positions
        std::vector<int> formation;
        // Distance between node and formation
        double targetDist = DBL_MAX;
        // Standard rate of travel (cell/sec)
        int speed = 2;
        // Sensor radius
        double r_sensor = 3;
};


#endif