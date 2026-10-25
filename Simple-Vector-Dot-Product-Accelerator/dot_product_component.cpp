#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <cstdint>
#include <cstdio>
#include <functional>

#include "headers/dpc_regfields.h"
#include "headers/dpc_gvsoc.h"

// Dot Product Component: register storage and address decoding are generated
// from regmap.md. The CONTROL callback implements the accelerator behavior.
class dcp : public vp::Component
{
public:
    dcp(vp::ComponentConf &config);

private:
    vp::IoSlave input_itf;
    vp::Trace trace;
    vp_regmap_regmap regmap;

    static vp::IoReqStatus handle_req(vp::Block *__this, vp::IoReq *req);
    void handle_control_access(uint64_t reg_offset, int size,
                               uint8_t *value, bool is_write);
};

extern "C" vp::Component *gv_new(vp::ComponentConf &config)
{
    return new dcp(config);
}

dcp::dcp(vp::ComponentConf &config)
    : vp::Component(config), regmap(*this, "regmap")
{
    this->input_itf.set_req_meth(&dcp::handle_req);
    this->new_slave_port("input", &this->input_itf);

    this->traces.new_trace("trace", &this->trace);
    this->regmap.build(this, &this->trace);

    // Generic accesses are handled by the generated register map.
    // CONTROL alone needs a side effect when software writes a start command.
    this->regmap.control.register_callback(
        std::bind(&dcp::handle_control_access, this,
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, std::placeholders::_4),
        true);
}

vp::IoReqStatus dcp::handle_req(vp::Block *__this, vp::IoReq *req)
{
    dcp *_this = static_cast<dcp *>(__this);
    const uint64_t offset = req->get_addr();

    // V2.2 uses 16 operands per vector, with 32-bit aligned accesses.
    if (req->get_size() != 4 || (offset & 3) != 0 || offset > 0x88)
        return vp::IO_REQ_INVALID;

    if (!req->get_is_write() && offset != 0x84 && offset != 0x88)
        return vp::IO_REQ_INVALID;

    if (req->get_is_write() && (offset == 0x84 || offset == 0x88))
        return vp::IO_REQ_INVALID;

    _this->regmap.access(offset, req->get_size(), req->get_data(),
                         req->get_is_write());

    if (!req->get_is_write() && offset == 0x88)
        printf("[DPC] Result sent back to CPU.\n");

    return vp::IO_REQ_OK;
}

void dcp::handle_control_access(uint64_t reg_offset, int size,
                                uint8_t *value, bool is_write)
{
    // The custom access callback must still perform the register update.
    this->regmap.control.update(reg_offset, size, value, is_write);

    // Preserve V1 semantics: the whole 32-bit register must equal 1.
    if (is_write && this->regmap.control.get() == 1)
    {
        printf("[DPC] Values received!\n");

        // Gather generated named registers into local arrays for a simple MAC loop.
        const uint32_t a[16] = {
            this->regmap.a0.get(),
            this->regmap.a1.get(),
            this->regmap.a2.get(),
            this->regmap.a3.get(),
            this->regmap.a4.get(),
            this->regmap.a5.get(),
            this->regmap.a6.get(),
            this->regmap.a7.get(),
            this->regmap.a8.get(),
            this->regmap.a9.get(),
            this->regmap.a10.get(),
            this->regmap.a11.get(),
            this->regmap.a12.get(),
            this->regmap.a13.get(),
            this->regmap.a14.get(),
            this->regmap.a15.get()
        };
        const uint32_t b[16] = {
            this->regmap.b0.get(),
            this->regmap.b1.get(),
            this->regmap.b2.get(),
            this->regmap.b3.get(),
            this->regmap.b4.get(),
            this->regmap.b5.get(),
            this->regmap.b6.get(),
            this->regmap.b7.get(),
            this->regmap.b8.get(),
            this->regmap.b9.get(),
            this->regmap.b10.get(),
            this->regmap.b11.get(),
            this->regmap.b12.get(),
            this->regmap.b13.get(),
            this->regmap.b14.get(),
            this->regmap.b15.get()
        };

        uint32_t result = 0;
        for (int i = 0; i < 16; i++)
            result += a[i] * b[i];

        this->regmap.result.set(result);
        this->regmap.status.set(1);

        printf("[DPC] Computation success.\n");
    }
}
