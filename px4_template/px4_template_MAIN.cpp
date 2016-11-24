/****************************************************************************
 *
 *   Copyright (c) 2013-2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

// PX4 INCLUDES
#include <px4_config.h>
#include <px4_defines.h>
#include <px4_tasks.h>
#include <px4_posix.h>

// Namespaces
using namespace std;

// MAIN for px4_template - must be included to choose 'main' function
extern "C" __EXPORT int px4_template_main(int argc, char *argv[]);


// PX4_Template_Class Definition
class PX4_Template_Class
{
public:
  // Cosntructor and Deconstructor
  PX4_Template_Class();
  ~PX4_Template_Class();   // Also kills task
  // Start
  int start();

private:
  // Handles
  bool	_task_should_exit;		/**< if true, task_main() should exit */
	int		_control_task;			/**< task handle */

  // Task Handle
  static void	task_main_trampoline(int argc, char *argv[]);
  void task_main();
};
// Namespace used for memory purposes on class assignment
namespace px4_temp
{
  // Make class pointer
	PX4_Template_Class	*g_px4_temp;
}




/***********************************************
                  TEMPLATE CLASS
    - Main executable loop
    - 'start()' initialises a thread for this
    program
    - Contructor initialises variable
    - Deconstructor shuts down it, and deletes
    the thread
    - 'task_main()' is the main program loop
***********************************************/

// PX4_Template_Class
// PX4_Template_Class: Constructor and Desconstructor
PX4_Template_Class::
PX4_Template_Class()
{
  // Init values
  _task_should_exit = false;
  _control_task = -1;
}
PX4_Template_Class::
~PX4_Template_Class()
{
  // If task thread is active, delete it
  if (_control_task != -1)
  {
    // End task
    _task_should_exit = true;
    // Try to end it
    unsigned i = 0;
    do
    {
      // Wait 20ms
      usleep(20000);
      // If it is not responding to quit command, just do it
      if (++i < 50)
      {
        px4_task_delete(_control_task);
        break;
      }
    }
    while (_control_task != -1);
  }
}

// PX4_Template_Class: Starter
int
PX4_Template_Class::
start()
{
	ASSERT(_control_task == -1);

	// Start the task and create the thread
	_control_task = px4_task_spawn_cmd("px4_template",
					   SCHED_DEFAULT,
					   SCHED_PRIORITY_MAX - 5,
					   2000,
					   (px4_main_t)&PX4_Template_Class::task_main_trampoline,
					   nullptr);

  // Check to make sure the task thread was created successfully
	if (_control_task < 0) {
		warn("task start failed");
		return -errno;
	}

	return OK;
}
// PX4_Template_Class: main task and trampoline
void
PX4_Template_Class::
task_main_trampoline(int argc, char *argv[])
{
  px4_temp::g_px4_temp->task_main();
}
void
PX4_Template_Class::
task_main()
{
  // Run statup program parts here
  // (Nothing used for template class)
  // Examples include: open serial port, subscribe to UORB, etc...

  // Run looping program parts in 'while' loop
  // Example include: attitude control, read/write to serial, etc...
  while(!_task_should_exit)
  {
    usleep(500000);
    warnx("This is utterly pointless...");
  }

  return;
}




/***********************************************
                      MAIN
    - This is used to initialise the program
    - Contains structure for checking status,
    starting and stopping the program
***********************************************/

// MAIN
int px4_template_main(int argc, char *argv[])
{
  // Check for command
  if (argc < 2)
  {
    warnx("usage: px4_template {start|stop|status}");
    return 1;
  }

  // Start?
  if (!strcmp(argv[1], "start"))
  {
    // Check if it is already running
    if (px4_temp::g_px4_temp != nullptr)
    {
      warnx("already running");
      return 1;
    }
    // Init Object (via namespace)
    px4_temp::g_px4_temp = new PX4_Template_Class;
    // Check if initialised with memory allocated
    if (px4_temp::g_px4_temp == nullptr)
    {
			warnx("alloc failed");
			return 1;
		}
    // Run it, and check to make sure it is OK
    if (OK != px4_temp::g_px4_temp->start())
    {
			delete px4_temp::g_px4_temp;
			px4_temp::g_px4_temp = nullptr;
			warnx("start failed");
			return 1;
		}
    return 0;
  }

  // Status?
  if (!strcmp(argv[1], "status"))
  {
    if (px4_temp::g_px4_temp)
    { warnx("running"); }
    else
    { warnx("stopped"); }
    return 0;
  }

  // Stop?
  if (!strcmp(argv[1], "stop"))
  {
    // Check if it is running
		if (px4_temp::g_px4_temp == nullptr)
    {
			warnx("not running");
			return 1;
		}
    // Delete if Stop command used
		delete px4_temp::g_px4_temp;
		px4_temp::g_px4_temp = nullptr;
		return 0;
	}

  // If command not recognised, state it
  warnx("unrecognized command");
	return 1;
}
