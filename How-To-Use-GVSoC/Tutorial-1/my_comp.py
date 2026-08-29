import gvsoc.systree

# Create the class for our components, following the same syntax as what we did for components in Tutorial 0.
# The `parent`, and `name` option is a must. The rest can add based on our design.
class MyComp(gvsoc.systree.Component):
    def __init__(self, parent: gvsoc.systree.Component, name: str, value: int):
        super().__init__(parent, name)

        # Add my_comp.cpp file source for this component.
        # Recall that .py file describes the "shell" (sub-components, ports, etc) of the components while .cpp/.c files describe the actual behaviour of the component.
        # OOP knowledge: add_source() is inherited from parent class gvsoc.systree.Component
        self.add_sources(["my_comp.cpp"])

        # Give the component configurable properties by creating its JSON file
        # Again, this function is inherited and will create the JSON property file for the component
        """
        The JSON file has 2 jobs:
            - It is created by the python script, and it acts as a "bridge" to allow the .cpp to describe how the components get the data 
            (i.e. coding that another component will access this file to get the value from mycomp)
            - It is a configurable file that allows users to change the properties of a component without needing to recompile.
        """
        self.add_properties({
            "value": value # This value is passed from the constructor
        })

        # Define input port for component by defining a function
        # the name follows the convention to show that this is an input interface
        # This function return a SlaveIft (slave interface) object, which recieves request from other components (the masters)
        # Later, we can use this to connect MyComp to our SoC/ico
        def i_INPUT(self) -> gvsoc.systree.SlaveItf:
            return gvsoc.systree.SlaveItf(self, "input", signature="io") # Name of port is "input", and port is i/o port.
        

        
            

