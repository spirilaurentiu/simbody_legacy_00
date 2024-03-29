/* -------------------------------------------------------------------------- *
 *                       Simbody(tm): SimTKcommon                             *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK biosimulation toolkit originating from           *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org/home/simbody.  *
 *                                                                            *
 * Portions copyright (c) 2008-12 Stanford University and the Authors.        *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#include "SimTKcommon.h"

#include <iostream>

#define ASSERT(cond) {SimTK_ASSERT_ALWAYS(cond, "Assertion failed");}

using std::cout;
using std::endl;
using namespace SimTK;
using namespace std;

bool isParallel;

class SetFlagTask : public ParallelExecutor::Task {
public:
    SetFlagTask(Array_<int>& flags, int& count) : flags(flags), count(count) {
    }
    void execute(int index) {
        flags[index]++;
        localCount.upd()++;
        ASSERT(ParallelExecutor::isWorkerThread() == isParallel);
    }
    void initialize() {
        localCount.upd() = 0;
        ASSERT(ParallelExecutor::isWorkerThread() == isParallel);
    }
    void finish() {
        count += localCount.get();
        ASSERT(ParallelExecutor::isWorkerThread() == isParallel);
    }
private:
    Array_<int>& flags;
    int& count;
    ThreadLocal<int> localCount;
};

void testParallelExecution() {
    const int numFlags = 100;
    Array_<int> flags(numFlags);
    isParallel = (ParallelExecutor::getNumProcessors() > 1);
    ParallelExecutor executor;
    ASSERT(!ParallelExecutor::isWorkerThread());
    for (int i = 0; i < 100; ++i) {
        int count = 0;
        SetFlagTask task(flags, count);
        for (int j = 0; j < numFlags; ++j)
            flags[j] = 0;
        executor.execute(task, numFlags-10);
        ASSERT(count == numFlags-10);
        for (int j = 0; j < numFlags; ++j)
            ASSERT(flags[j] == (j < numFlags-10 ? 1 : 0));
    }
    ASSERT(!ParallelExecutor::isWorkerThread());
}

void testSingleThreadedExecution() {
    const int numFlags = 100;
    Array_<int> flags(numFlags);
    isParallel = false;
    ParallelExecutor executor(1); // Specify only a single thread.
    int count = 0;
    SetFlagTask task(flags, count);
    for (int j = 0; j < numFlags; ++j)
        flags[j] = 0;
    executor.execute(task, numFlags-10);
    ASSERT(count == numFlags-10);
    for (int j = 0; j < numFlags; ++j)
        ASSERT(flags[j] == (j < numFlags-10 ? 1 : 0));
}

int main() {
    try {
        testParallelExecution();
        testSingleThreadedExecution();
    } catch(const std::exception& e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }
    cout << "Done" << endl;
    return 0;
}
