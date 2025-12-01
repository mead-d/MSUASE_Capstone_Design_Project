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

 #ifndef __deployer_h_
 #define __deployer_h_
 #include "adevs.h"
 #include "Autonode.h"
 #include <stdlib.h>
 #include <vector>

 /**
  * @brief The deployer is the mechanism that creates and drops autonomous nodes off in the environment en masse.
  * The deployer will also drop off replacement nodes.
  */

class Deployer: public adevs::Atomic<int>
{
    public:
        /**
         * The deployer requires a seed as input to determine a node's starting position.
         * An empty list of nodes, and environment dimensions are necessary as well.
         */
        Deployer(unsigned int seed, int env_length, int env_width, std::vector<Autonode*>& nodeList);

        // Internal transition
        void delta_int();
        // External transition. Will execute when batch of new nodes ordered.
        void delta_ext(double e, const adevs::Bag<int>& xb);
        // Confluent transition.
        void delta_conf(const adevs::Bag<int>& xb);
        // Output function produces the deployed nodes
        void output_func(adevs::Bag<int>& yb);
        // Time advance function.
        double ta();
        // Garbage collection
        void gc_output(adevs::Bag<int>&);
    private:
        //adevs::rv seed;
        // Time until next action
        double time = 1.0;
        // Environment dimensions
        int env_length, env_width;
        // List and Num of nodes to deploy
        //std::vector<Autonode*> nodeList;
        std::vector<int> formation;
        int nNodes = 6;
        int nodeID;
};

#endif