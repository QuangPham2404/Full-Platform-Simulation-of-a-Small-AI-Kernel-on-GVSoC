# Tutorial 1 Notes

## Content of tutorial

This tutorial continues from Tutorial 0 by creating a new component and add it to the rv64 board in Tutorial 0.

## Programming Notes for CPP

Some notes about OOP programming with CPP, which is used whenever we want to write cpp files to describe our own hardware.

1. **Function decleration in CPP:** `<return_type> <function_name>(<params>)`.

    For example:
     - int sum(int a, int b): function `sum` returning an `int`
     - pointer_object *return_a_pointer_func(input): function `return_a_pointer_func` returning a pointer

2. **Namespace and class in CPP = modules and class in Python**

    - CPP uses the notation `::` to notates this class belongs to this namespace, similar to how Python uses `.` for modules and their classes/functions.
    - Example: `vp:ComponentConf &config` means `&config` is an object in the `ComponentConf` library in the `vp` namespace.

3. **Passing param by reference**

    In CPP, there are 3 main ways to pass values:
    - by value (`func(value)`): a copy is created of value and it is local. Changing it doesnt change the orignal object
    - by pointer (`func(*value)`): passes the pointer to the address of the original object. Going there and changing the value modifies the original object.
    - by referece (`funct(&value)`): passes the original object directly, no value, no pointer.
  
    Also note that the param name for a function is local.

    ```cpp
    // Defining the function
    vp::Component *gv_new(vp::ComponentConf &config)

    //Calling a function and passing in the argument object
    gv_new(myConfig) 
    
    /*
    Here `MyConfig` is a real object outside of the function passed into it.
    However, in the function, the local scope still refers to it as `config` rather than `MyConfig`
    */
    ```

4. **Class and inheritence in CPP**

    Syntax to create a child class with functions:

    ```cpp
    // Class MyComp is a child class that inherits from the parent class vp::Component
    class MyComp : public vp::Component
    {
        public start():
        public stop():
    };
    ```

    Translation to python:

    ```python
    class MyComp(vp.Component):
        def start():
            pass
        def stop():
            pass
    ```

    Syntax to create class with constructor (i.e. the __init__ that is used whenever teh class is instantiated):

    ```cpp
    //Create class MyCPU with constructor taking 2 inputs
    class MyCPU(component_library::CPU_class)
    {
        //Constructor declare what is needed as input for class
        MyCPU(int no_inputs, int *no_outputs); //Here no_ouput is the pointer to the integer no_ouputs
    };
    ```

    Translation to Python:

    ```python
    class MyCPU(component_library.CPU_class):
        def __init__(self, no_inputs, no_outputs):
            # After decleration, Python usually define the attributes
            self.no_inputs = no_inputs
            self.no_outputs = no_outputs
    ```

    Syntax to instantiate a class object in CPP. There are 2 methods: (1) stack allocation - creating an object, (2) heap allocation - creating a pointer to the object (use `new`)

    ```cpp
    //Method 1: Creating a new MyCPU object called new_cpu
    MyCPU new_cpu = MyCPU(<params>)

    // Method 2: Creating a pointer called new_cpu_pointer that points to a MyCPU object
    MyCPU *new_cpu_pointer = new MyCPU(<params>)
    ```

    Translation in Python

    ```python
    new_cpu = MyCPU(<params>) #Generally Python handles the pointer stuff under the hood
    ```