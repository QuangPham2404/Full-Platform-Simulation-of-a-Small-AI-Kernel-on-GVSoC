#include <vp/vp.hpp>
#include <vp/itf/wire.hpp>
#include <cstdint>
#include "my_result.hpp"

// Create class for my_comp2
class MyComp2 : public vp::Component
{
    // Define constructor
    public:
        MyComp2(vp::ComponentConf &config);

    private:
        // Declare ports: Input port to recieve notif and output port to send result
        vp::WireSlave<bool> notif_itf;
        vp::WireMaster<MyResult *> result_itf;

        // Define callback function to handle notif
        static void handle_notif(vp::Block *__this, bool value);

        // Define callback function for the delayed response event
        static void handle_event(vp::Block *__this, vp::ClockEvent *event);

        // Declare a clock event object to allow "schduling of response" behaviour. In this case its waiting some cycles before sending output to my_comp
        vp::ClockEvent event;

        // Trace channel used when the delayed response is sent
        vp::Trace trace;
};

// Define the constructor
MyComp2::MyComp2(vp::ComponentConf &config)
    : vp::Component(config), event(this, MyComp2::handle_event)
{
    // Set callback function to the input port
    this->notif_itf.set_sync_meth(&MyComp2::handle_notif);

    // Register the ports
    this->new_slave_port("notif", &(this->notif_itf));
    this->new_master_port("result", &(this->result_itf));

    // Register the trace channel
    this->traces.new_trace("trace", &this->trace);
}

// Define the handle_notif callback function
void MyComp2::handle_notif(vp::Block *__this, bool value)
{
    // Turn __this to a usable pointer that points to a MyComp2 object
    MyComp2 *_this = (MyComp2 *)__this;

    // Response behaviour: First print boolean notfication value when it recieves a notif from MyComp
    printf("Received value %d\n", value);

    // Using this conditional prevent this case: multiple notif from MyComp will cause multiple event to be enqueue, and this may broke our "timer"
    if (!_this->event.is_enqueued())
    {
        _this->event.enqueue(10);
    }

    // With the event object, we defer the behaviour of sending the result back to the function handle_event, and in handle_notif, we only "handle" the handle_event function.
    // Think of this as a "two-step" handling proceed.
}

// Define function to instantiate the class
extern "C" vp::Component *gv_new(vp::ComponentConf &config)
{
    return new MyComp2(config);
}

// Define handle_event function to use the object "event" for scheduling a response to the notif received from my_comp
void MyComp2::handle_event(vp::Block *__this, vp::ClockEvent *event)
{    
    // Turn __this to a usable pointer that points to a MyComp2 object
    MyComp2 *_this = (MyComp2 *)__this;

    // Adding tracing
    _this->trace.msg(vp::TraceLevel::DEBUG, "Sending result\n");

    // Send output to my_comp
    MyResult result = {0x11111111, 0x22222222};

    _this->result_itf.sync(&result);
}
