#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/wire.hpp>
#include <cstdint>
#include <my_results.hpp>


// Create class MyComp for our component, which inherits from vp::Component
class MyComp : public vp::Component
{
    // Define constructor for our component and its parameters.
    public:
        MyComp(vp :: ComponentConf &config);

    private:
        // Variable for our input port defined by "gvsoc.systree.SlaveItf(self, "input", signature="io")" in the Python script
        // This allow the component to recieve I/O request from cpu --> ico --> its input port
        vp::IoSlave input_itf;

        // Variable to store the data that the component will return
        uint32_t value;

        // Define the callback function for our input port. This function will be called when the component receives an I/O request from the CPU.
        // Return type is status if I/O request is accepted or rejected.
        // Its parameters are a pointer to an instantiation of the component and a pointer to the I/O request.gvsoc.systree.SlaveItf(self, "input", signature="io")
        static vp::IoReqStatus handle_req(vp::Block *__this, vp::IoReq *req);

        // Create new ports for tutorial 2: 1 ouput port to send noti and 1 input port to recieve result from mycomp2
        vp::WireMaster<bool> notif_itf;
        vp::WireSlave<MyResult *> result_itf;

        // Define callback functinon to handle result, which expects a pointer to the mycomp2 object and another pointer to a MyResult object
        static void handle_result(vp::Block *__this, MyResult *result);

        // Create a trace object as the "trace" chanel for this component.
        vp::Trace trace;

        // Create a vcd trace signal (not a channel like normal trace)
        vp::Signal<uint32_t> vcd_value; // Signal is 32 bits

};


//Define instantiation function for the class. Since this file and the main runtime fules are compiled separately,
// so we need to define a function that will be called by the runtime to instantiate our component.
extern "C" vp::Component *gv_new(vp::ComponentConf &config)
{
    return new MyComp(config); // Recall that "new" returns a pointer to the object
};


// Define the constructor for our component. This function will be called when the component is instantiated.
MyComp::MyComp(vp::ComponentConf &config)
    : vp::Component(config), vcd_value(*this, "status", 32) // The vcd_value signal belongs to my_comp, it has the name status, and it is 32 bits wide
{
    // Define the callback function for the input port. This function will be called when the component receives an I/O request from the CPU.
    this->input_itf.set_req_meth(&MyComp::handle_req);

    // Register an input port for our component. This will allow the component to receive I/O requests from the CPU.
    // Note that the name must match that of the port defined in the Python script. In this case, the name is "input".
    /*
    Here the "&" means takes the address of the input_itf object, which is a pointer to the input_itf object. 
    This is necessary because the new_slave_port function expects a pointer to an IoSlave object.

    The flow of creating an I/O port is as follows:
    1. In the Python script, we define a port with a name and a signature. The name is used to identify the port, and the signature defines the type of the port (in this case, "io" for I/O).
    2. In the C++ code, we create an instance of the IoSlave class, which represents the I/O port. 
    3. We then set a callback function for the port using the set_req_meth function. This function will be called when the component receives an I/O request from the CPU.
    4. Finally, we register the port with the component using the new_slave_port function. This function takes the name of the port and a pointer to the IoSlave object as arguments.
    The step 4 is purely for registering the component with the port defined in the Python script.
    */
    this->new_slave_port("input", &(this->input_itf));

    // Define how to get the value from the config JSON file
    this->value = this->get_js_config()->get_child_int("value");

    /*
    Notice that the functions set_req_meth, new_slave_port, and get_js_config are all member functions of the vp::Component class, which is the base class of MyComp.
    */

    // Define callback function for the input port and register it under the name "result" defined by the Python program
    this->result_itf.set_sync_meth(&MyComp::handle_result);
    this->new_slave_port("result", &this->result_itf);

    // Register output port under the name "notif" defined by the Python script
    this->new_master_port("notif", &this->notif_itf);

    // Register trace channel for the component using "traces" the inherited "trace manager" functionality from the vp::Component class, allowing the trace channel to be created when the componenent is instantiated.
    this->traces.new_trace("trace", &this->trace);
}


// Define the callback function MyComp::handle_req for our input port
vp::IoReqStatus MyComp::handle_req(vp::Block *__this, vp::IoReq *req)
{
    // First, turn the voide pointer into a pointer pointining to an instance of MyComp that is calling the function
    MyComp *_this = (MyComp *)__this;

    /*
    // Printing out the request for logging - recall that req is a pointer to an I/O request
    printf("Received request at offset 0x%lx, size 0x%lx, is_write %d\n",
        req->get_addr(), req->get_size(), req->get_is_write());
    */

    // Use tracing instead of printing
     _this->trace.msg(vp::TraceLevel::DEBUG, "Received request at offset 0x%lx, size 0x%lx, is_write %d\n",
        req->get_addr(), req->get_size(), req->get_is_write());

    // Response behaviour: take the stored value in the component and write that to the memory of the return buffer of the I/O request.
    /*
    Conditions:
    1. The request is a read request (not a write request)
    2. The request is at offset 0x0 (the address of the request is 0), checking if the request is requesting the correct mem space
    3. The size of the request is 4 bytes (the size of the request is 4), matching the size of "value"
    */
    if (!req->get_is_write() && req->get_addr() == 0 && req->get_size() == 4)
    {
        /*
        1. Find the pointer to the return buffer
        2. Cast it to a pointer to a uint32_t to ensure the when the data is written to the buffer it is in the correct data type
        3. Derefence the pointer and assign the stored value to its memory location
        */
        *(uint32_t *)req->get_data() = _this-> value;

        // Send notification to mycomp2 whenever this valid read occurs.
        // The .sync is inherited by an vp::WireMaster<bool> object
        _this->notif_itf.sync(true);

        // Return the status of the I/O request, as it is what the function is defined to return
        // This code doesnt account for the situtation where the request failed - it sends an "OK" message regardless
        return vp::IO_REQ_OK;
    }
    else if (req->get_is_write()) // If request is write rather than read (main.c is modified for Tutorial 4)
    {
        // Set the value to requested-to-be-written data in the buffer
        uint32_t value = *(uint32_t *)req->get_data();

        // Set an example condition to release the vcd_value
        if (value == 5)
        {
            _this->vcd_value.release();
        }
        // Set vcd trace value to value to capture write
        else
        {
            _this->vcd_value.set(value);
        }

        // Return status of the I/O request, as it is waht the function is defined to return
        return vp::IO_REQ_OK;
    }
    else // If request is not valid, retrun failure status
    {
    return vp::IO_REQ_INVALID;
    }
}


// Define the call back function to handle the request from mycomp2
void MyComp::handle_result(vp::Block *__this, MyResult *result)
{
    printf("Received results %x %x\n", result->value0, result->value1);
}
