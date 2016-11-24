# PX4 Code Template

This is a template of the px4 code - used to create a task and thread.
Use it when building a px4 module.


# Directory Structure

* **CMakeLists.txt**:         CMakeLists file used to link the module to the PX4.
* **px4_template_MAIN.cpp**:  C++ PX4 code template file.


# Changes to the template for custom code for usage

Note: DIR = Directory/Module Name

**CMakeLists**: 
* Change 'MODULE' parameter to 'modules__DIR'
* Change 'MAIN' parameter to 'DIR'
* Add source files ('.c', '.cpp') to 'SRCS' parameter list

**PX4 Code Template**:

_Required Changes_
* Change all instances of 'PX4_Template_Class' to desired Class name
* Change all instances of 'px4_template' to 'DIR'
* Change all instances of 'px4_temp' to desired Namespace name


# Adding custom code to the device

**Class Declaration Code**:
* Add new instances to the Class like any other class

**Code Execution**
* Add required code into the class function 'task_main()'
* Looping code should be in 'while(!_task_should_exit)'



