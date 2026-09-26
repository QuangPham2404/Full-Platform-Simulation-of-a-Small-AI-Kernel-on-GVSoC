import gvsoc.systree

# Create the class for our components, following the same syntax as what we did for components in Tutorial 0.
# The `parent`, and `name` option is a must. The rest can add based on our design.
class DotProductComponent(gvsoc.systree.Component):
    def __init__(self, parent: gvsoc.systree.Component, name: str):
        super().__init__(parent, name)

        # Add my_comp.cpp file source for this component.
        # Recall that .py file describes the "shell" (sub-components, ports, etc) of the components while .cpp/.c files describe the actual behaviour of the component.
        # OOP knowledge: add_source() is inherited from parent class gvsoc.systree.Component
        self.add_sources(["dot_product_component.cpp"])

    # Define input port for component by defining a function
    # the name follows the convention to show that this is an input interface
    # This function return a SlaveIft (slave interface) object, which recieves request from other components (the masters)
    # Later, we can use this to connect dcp to our SoC/ico in my_system.py
    def i_INPUT(self) -> gvsoc.systree.SlaveItf:
        return gvsoc.systree.SlaveItf(self, "input", signature="io") # Name of port is "input", and port is i/o port


    
    
    
        

        
            

