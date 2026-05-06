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
#include "TargetSelector.h"

// State structure of a node
typedef enum
{
    DEPLOYING,
    FORMING,
    WAITING
}
NodeState;

struct Output { int id; Point point; };


 /**
  * @brief This class models an autonomous node that exists in a 2-dimensional plane.
  * The node is one of many in a larger swarm. Each node stands guard passively sensing in the assigned environment.
  */
class Autonode: public adevs::Atomic<Output>
{
    public:
        //Autonode();
        Autonode(int id, std::vector<Point>& start_positions, std::vector<Point>& formation);

        // Internal transition function.
        void delta_int();
        // External transition function.
        void delta_ext(double e, const adevs::Bag<Output>& xb);
        // Confluent transition function.
        void delta_conf(const adevs::Bag<Output>& xb);
        // Output function.
        void output_func(adevs::Bag<Output>& yb);
        // Time advance function.
        double ta();
        // Garbage collection
        void gc_output(adevs::Bag<Output>&);
        // Destructor
        ~Autonode();
        
        // Getters
        NodeState getState();
        int get_id();
        double get_xpos();
        double get_ypos();
        double get_target_x();
        double get_target_y();
        vector<Edge> get_cost_matrix();
        double get_time_onStation();
        double get_time_deployment();
        // Setters
        void set_time(double time);
        void set_state(NodeState state);
        void set_formation(vector<Point>& formation);
    protected:
        // State of the node
        NodeState state;
    private:
        // Time until next action
        double delta_time = 1.0;
        double time_deployed;
        double time_start_maneuver;
        double time_active = 0.0;
        double time_onStation;

        // unique id
        int id;
        double x_cur, y_cur, dist_2_xtarget, dist_2_ytarget, distance;
        double x_target, y_target;
        vector<Point> start_positions;
        vector<Point> formation;
        vector<Edge> cost_matrix;
        // Distance between node and formation
        double targetDist = DBL_MAX;
        // Standard rate of travel (cell/sec)
        int speed = 2;
        // Sensor radius
        double r_sensor = 5;
};
#endif