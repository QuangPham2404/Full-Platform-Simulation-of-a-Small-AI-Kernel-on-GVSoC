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
    vp::Signal<uint32_t> state_signal; // 0=IDLE, 1=BUSY, 2=DONE
    vp::Signal<uint32_t> accumulator_signal;
    vp::PowerSource background_power;
    vp::PowerSource mac_power;
    void start() override;
    vp::ClockEvent compute_event;
    uint32_t a[16], b[16]; // Snapshot: MMIO writes affect only the next job.
    uint32_t accumulator = 0;
    unsigned index = 0;
    bool busy = false;

    static void handle_compute_event(vp::Block *__this, vp::ClockEvent *event);

    static vp::IoReqStatus handle_req(vp::Block *__this, vp::IoReq *req);
    void handle_control_access(uint64_t reg_offset, int size,
                               uint8_t *value, bool is_write);
};

extern "C" vp::Component *gv_new(vp::ComponentConf &config)
{
    return new dcp(config);
}

dcp::dcp(vp::ComponentConf &config)
    : vp::Component(config), regmap(*this, "regmap"),
      state_signal(*this, "state", 2),
      accumulator_signal(*this, "accumulator", 32),
      compute_event(this, dcp::handle_compute_event)
{
    this->input_itf.set_req_meth(&dcp::handle_req);
    this->new_slave_port("input", &this->input_itf);

    this->traces.new_trace("trace", &this->trace);
    this->regmap.build(this, &this->trace);
    this->state_signal.set(0);
    this->accumulator_signal.set(0);

    this->power.new_power_source("background", &this->background_power,
        this->get_js_config()->get("background_power"));
    this->power.new_power_source("mac", &this->mac_power,
        this->get_js_config()->get("mac_power"));

    // Generic accesses are handled by the generated register map.
    // CONTROL alone needs a side effect when software writes a start command.
    this->regmap.control.register_callback(
        std::bind(&dcp::handle_control_access, this,
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, std::placeholders::_4),
        true);
}

void dcp::start()
{
    // Fixed, always-on background model; no gating or voltage controller.
    this->background_power.leakage_power_start();
    this->background_power.dynamic_power_start();
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

    // Latency is consumed by the requester: 5 CPU cycles at 100 MHz.
    // Register updates happen now; this does not delay the compute event.
    const uint64_t latency_before = req->get_latency();
    req->inc_latency(5);
    _this->trace.msg(vp::TraceLevel::DEBUG,
                     "MMIO latency: offset=0x%llx, added=%llu requester cycles\n",
                     static_cast<unsigned long long>(offset),
                     static_cast<unsigned long long>(req->get_latency() - latency_before));

    if (!req->get_is_write() && offset == 0x88)
        printf("[DPC] Result sent back to CPU.\n");

    return vp::IO_REQ_OK;
}


void dcp::handle_control_access(uint64_t reg_offset, int size,
                                uint8_t *value, bool is_write)
{
    this->regmap.control.update(reg_offset, size, value, is_write);

    // Only CONTROL == 1 starts a new one-shot operation.
    if (is_write && this->regmap.control.get() == 1
        && !this->busy)
    {
        this->busy = true;
        this->index = 0;
        this->accumulator = 0;
        const uint32_t inputs_a[16] = {
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
        const uint32_t inputs_b[16] = {
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
        for (unsigned i = 0; i < 16; ++i)
        {
            this->a[i] = inputs_a[i];
            this->b[i] = inputs_b[i];
        }
        this->regmap.status.set(0);
        this->regmap.result.set(0);
        this->state_signal.set(1); // BUSY
        this->accumulator_signal.set(0);
        this->trace.msg(vp::TraceLevel::DEBUG,
                        "DPC BUSY: computation scheduled for one MAC per DPC cycle\n");
        printf("[DPC] Values received!\n");
        this->compute_event.enqueue(1);
    }
}

// Each event performs exactly one 32-bit wrapping MAC.
void dcp::handle_compute_event(vp::Block *__this, vp::ClockEvent *event)
{
    dcp *_this = static_cast<dcp *>(__this);
    _this->mac_power.account_energy_quantum();
    _this->accumulator += _this->a[_this->index] * _this->b[_this->index];
    ++_this->index;
    _this->accumulator_signal.set(_this->accumulator);
    _this->trace.msg(vp::TraceLevel::DEBUG,
                     "DPC MAC: step=%u accumulator=%u\n",
                     _this->index, _this->accumulator);
    if (_this->index < 16)
    {
        event->enqueue(1);
        return;
    }
    _this->busy = false;
    const uint32_t result = _this->accumulator;
    _this->regmap.result.set(result);
    _this->regmap.status.set(1);
    _this->state_signal.set(2); // DONE
    _this->trace.msg(vp::TraceLevel::DEBUG,
                     "DPC DONE: accumulator=%u\n", result);
    printf("[DPC] Computation success.\n");
}
