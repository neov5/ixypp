#pragma once

namespace ixypp {

class driver_t {
public:
    virtual void init() = 0;
    virtual void rx_batch() = 0;
    virtual void tx_batch() = 0;
};

};
