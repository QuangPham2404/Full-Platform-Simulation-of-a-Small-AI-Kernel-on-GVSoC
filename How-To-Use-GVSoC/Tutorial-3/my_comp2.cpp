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
};

// Define the constructor
MyComp2::MyComp2(vp::ComponentConf &config)
    : vp::Component(config)
{
    // Set callback function to the input port
    this->notif_itf.set_sync_meth(&MyComp2::handle_notif);

    // Register the ports
    this->new_slave_port("notif", &(this->notif_itf));
    this->new_master_port("result", &(this->result_itf));
}

// Define the handle_notif callback function
void MyComp2::handle_notif(vp::Block *__this, bool value)
{
    // Turn __this to a usable pointer that points to a MyComp2 object
    MyComp2 *_this = (MyComp2 *)__this;

    // Response behaviour: send out resuls whenever it recieves a notif from MyComp
    printf("Received value %d\n", value);

    MyResult result = {0x11111111, 0x22222222};

    _this->result_itf.sync(&result);
}

// Define function to instantiate the class
extern "C" vp::Component *gv_new(vp::ComponentConf &config)
{
    return new MyComp2(config);
}
