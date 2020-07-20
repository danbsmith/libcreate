/**
Software License Agreement (BSD)

\file      wheeldrop.cpp
\authors   Jacob Perron <jperron@sfu.ca>
\copyright Copyright (c) 2018, Autonomy Lab (Simon Fraser University), All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of Autonomy Lab nor the names of its contributors may
   be used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include "create/create.h"

#include <iostream>
#include "poll.h"

int main(int argc, char** argv) {
  // Select robot. Assume Create 2 unless argument says otherwise
  create::RobotModel model = create::RobotModel::CREATE_2;
  std::string port = "/dev/ttyUSB0";
  int baud = 115200;
  if (argc > 1 && std::string(argv[1]) == "create1") {
    model = create::RobotModel::CREATE_1;
    baud = 57600;
    std::cout << "Running driver for Create 1" << std::endl;
  }
  else {
    std::cout << "Running driver for Create 2" << std::endl;
  }

  // Construct robot object
  create::Create robot(model);

  // Connect to robot
  if (robot.connect(port, baud))
    std::cout << "Connected to robot" << std::endl;
  else {
    std::cout << "Failed to connect to robot on port " << port.c_str() << std::endl;
    return 1;
  }

  // Switch to Full mode
  robot.setMode(create::MODE_FULL);
  
  std::string should_continue; 
  std::cout << "This example program will run all of the Create's motors to demonstrate overcurrent sensing.  Press 'y' to continue: ";
  std::cin >> should_continue;
  if(should_continue[0] != 'y') return 0;
  std::cout << "The test can be stopped by pressing 'enter'" << std::endl;
  struct pollfd just_stdin;
  just_stdin.fd = 0;
  just_stdin.events = POLLIN;
  robot.setAllMotors(1.0, 1.0, 0.0);
  robot.driveWheels(1.0, 1.0);
  while (poll(&just_stdin, 1, 0) == 0) {
    // Get overcurrent status
    const bool wheel_overcurrent = robot.isWheelOvercurrent();
    const bool mainbrush_overcurrent = robot.isMainBrushOvercurrent();
    const bool sidebrush_overcurrent = robot.isSideBrushOvercurrent();

    // Print status
    std::cout << "\rOvercurrent status (wheel, main brush, and side brush): [ " <<
      wheel_overcurrent <<
      ", " <<
      mainbrush_overcurrent <<
      ", " <<
      sidebrush_overcurrent <<
      " ]";

    usleep(10000);  // 10 Hz
  }
  robot.driveWheels(0.0, 0.0);
  robot.setAllMotors(0.0, 0.0, 0.0);
  return 0;
}
