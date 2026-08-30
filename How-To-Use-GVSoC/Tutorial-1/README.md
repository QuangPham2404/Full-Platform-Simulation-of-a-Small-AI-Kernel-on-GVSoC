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

    a. Syntax to create a child class with functions:

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

    b. Syntax to create class with constructor (i.e. the __init__ that is used whenever teh class is instantiated). Note that even though decleration of constructor must be inside the class, its defintion can be OUTSIDE. The same applies to the method functions declared in the class. This is to keep the class declration clean and readble.

    ```cpp
    //Create class MyCPU with constructor taking 2 inputs
    class MyCPU(component_library::CPU_class)
    {
        //Constructor declare what is needed as input for class
        MyCPU(int no_inputs, int *no_outputs); //Here no_ouput is the pointer to the integer no_ouputs
    };

    //Define constructor outside of class
    MyCPU::MyCPU(int no_inputs, in *no_outputs)
        // Set up base class - like super()__init__ in Python
        : component_library::CPU_class(no_inputs, *no_outputs)
    {
        ...
    }
    ```

    Translation to Python:

    ```python
    class MyCPU(component_library.CPU_class):
        def __init__(self, no_inputs, no_outputs):
            # After decleration, Python usually define the attributes
            self.no_inputs = no_inputs
            self.no_outputs = no_outputs
    ```

    c. Syntax to instantiate a class object in CPP. There are 2 methods: (1) stack allocation - creating an object, (2) heap allocation - creating a pointer to the object (use `new`)

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

    d. Syntax to access members/methods of a class: (1) if we have the object use `object.member`, (2) if we have the pointer to the object use `object->member`. Also, in CPP, the keyword `this` refers to the class object that is calling the members/methods. To call a method on a member use `member.method_called(params)`

    ```cpp
    MyCPU::MyCPU(int no_inputs, int *no_outputs)
    {
        this->no_inputs.get_no_inputs();
        this->no_outputs = get_pointer_to_no_outputs();
    }
    ```
    
## Success output

Normal run with `make run`:

```txt
/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch/build/install/bin/gvrun --target-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch --target=my_system --work-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch/build/work --parameter binary=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch/build/test/test run
Received request at offset 0x0, size 0x4, is_write 0
Hello, got 0x12345678 from my comp
```

Trace run with `make run runner_args="--trace=insn"` (excerpt):

```txt
(.venv) stvn@DESKTOP-PU3LLP9:~/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch$ make run runner_args="--trace=insn"
/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch/build/install/bin/gvrun --target-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch --target=my_system --work-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch/build/work --parameter binary=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/1_how_to_write_a_component_from_scratch/build/test/test run --trace=insn
30000: 3: [/soc/host/insn                ] _start:5                         M 0000000000000c14 auipc               sp, 0x0           sp=0000000000000c14
40000: 4: [/soc/host/insn                ] _start:5                         M 0000000000000c18 addi                sp, sp, fffffffffffffe7c  sp=0000000000000a90  sp:0000000000000c14
50000: 5: [/soc/host/insn                ] _start:8                         M 0000000000000c1c auipc               t0, 0x0                   t0=0000000000000c1c
60000: 6: [/soc/host/insn                ] _start:8                         M 0000000000000c20 addi                t0, t0, 1a                t0=0000000000000c36  t0:0000000000000c1c
70000: 7: [/soc/host/insn                ] _start:9                         M 0000000000000c24 csrrw               0, t0, mtvec              t0:0000000000000c36
80000: 8: [/soc/host/insn                ] _start:12                        M 0000000000000c28 auipc               t0, 0x0                   t0=0000000000000c28
90000: 9: [/soc/host/insn                ] _start:12                        M 0000000000000c2c addi                t0, t0, ffffffffffffffc4  t0=0000000000000bec  t0:0000000000000c28
100000: 10: [/soc/host/insn                ] _start:13                        M 0000000000000c30 c.li                a0, 0, 0                  a0=0000000000000000
```

## Error logs

Available at ERROR_LOG.md