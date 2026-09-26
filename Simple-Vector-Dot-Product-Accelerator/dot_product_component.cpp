#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <cstdint>
#include <cstdio>


// Create class dcp for our component, which inherits from vp::Component
class dcp : public vp::Component
{
public:
    dcp(vp::ComponentConf &config);

private:
    // Input I/O port
    vp::IoSlave input_itf;

    // Internal registers
    uint32_t a0, a1, a2, a3;
    uint32_t b0, b1, b2, b3;
    uint32_t result, control, status;

    // Callback for I/O requests
    static vp::IoReqStatus handle_req(vp::Block *__this, vp::IoReq *req);
};


// Instantiation function
extern "C" vp::Component *gv_new(vp::ComponentConf &config)
{
    return new dcp(config);
}


// Constructor
dcp::dcp(vp::ComponentConf &config)
    : vp::Component(config)
{
    // Define callback function for input port
    this->input_itf.set_req_meth(&dcp::handle_req);

    // Initial values
    this->a0 = 0;
    this->a1 = 0;
    this->a2 = 0;
    this->a3 = 0;

    this->b0 = 0;
    this->b1 = 0;
    this->b2 = 0;
    this->b3 = 0;

    this->control = 0;
    this->status = 0;
    this->result = 0;

    // Register input port
    this->new_slave_port("input", &(this->input_itf));
}


// Callback function
vp::IoReqStatus dcp::handle_req(vp::Block *__this, vp::IoReq *req)
{
    // Get pointer to current DPC instance
    dcp *_this = (dcp *)__this;


    // -------------------------
    // WRITE REQUEST
    // -------------------------
    if (req->get_is_write())
    {
        uint32_t value = *(uint32_t *)req->get_data();

        if (req->get_addr() == 0x00)
        {
            _this->a0 = value;
        }
        else if (req->get_addr() == 0x04)
        {
            _this->a1 = value;
        }
        else if (req->get_addr() == 0x08)
        {
            _this->a2 = value;
        }
        else if (req->get_addr() == 0x0C)
        {
            _this->a3 = value;
        }
        else if (req->get_addr() == 0x10)
        {
            _this->b0 = value;
        }
        else if (req->get_addr() == 0x14)
        {
            _this->b1 = value;
        }
        else if (req->get_addr() == 0x18)
        {
            _this->b2 = value;
        }
        else if (req->get_addr() == 0x1C)
        {
            _this->b3 = value;
        }
        else if (req->get_addr() == 0x20)
        {
            _this->control = value;

            // Start computation when CONTROL = 1
            if (_this->control == 1)
            {
                printf("[DPC] Values received!\n");

                _this->result =
                      _this->a0 * _this->b0
                    + _this->a1 * _this->b1
                    + _this->a2 * _this->b2
                    + _this->a3 * _this->b3;

                // Computation completed
                _this->status = 1;

                printf("[DPC] Computation success.\n");
            }
        }
        else
        {
            return vp::IO_REQ_INVALID;
        }

        return vp::IO_REQ_OK;
    }


    // -------------------------
    // READ REQUEST
    // -------------------------
    else
    {
        // STATUS register
        if (req->get_addr() == 0x24)
        {
            *(uint32_t *)req->get_data() = _this->status;
        }

        // RESULT register
        else if (req->get_addr() == 0x28)
        {
            *(uint32_t *)req->get_data() = _this->result;

            printf("[DPC] Result sent back to CPU.\n");
        }

        // Unsupported address
        else
        {
            return vp::IO_REQ_INVALID;
        }

        return vp::IO_REQ_OK;
    }
}