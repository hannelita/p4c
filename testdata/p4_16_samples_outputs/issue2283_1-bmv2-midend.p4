#include <core.p4>
#define V1MODEL_VERSION 20180101
#include <v1model.p4>

header ports_t {
    bit<16> port1;
    bit<16> port2;
    bit<16> port3;
}

struct Headers {
    ports_t ports;
}

struct Meta {
}

parser p(packet_in pkt, out Headers hdr, inout Meta m, inout standard_metadata_t sm) {
    state start {
        pkt.extract<ports_t>(hdr.ports);
        transition accept;
    }
}

control vrfy(inout Headers h, inout Meta m) {
    apply {
    }
}

control ingress(inout Headers h, inout Meta m, inout standard_metadata_t sm) {
    @noWarn("unused") @name(".NoAction") action NoAction_1() {
    }
    @name("ingress.t") table t_0 {
        key = {
            h.ports.port1: exact @name("h.ports.port1") ;
            h.ports.port2: exact @name("h.ports.port2") ;
            h.ports.port3: exact @name("h.ports.port3") ;
        }
        actions = {
            NoAction_1();
        }
        default_action = NoAction_1();
    }
    apply {
        t_0.apply();
    }
}

control egress(inout Headers h, inout Meta m, inout standard_metadata_t sm) {
    apply {
    }
}

control update(inout Headers h, inout Meta m) {
    apply {
    }
}

control deparser(packet_out b, in Headers h) {
    apply {
        b.emit<ports_t>(h.ports);
    }
}

V1Switch<Headers, Meta>(p(), vrfy(), ingress(), egress(), update(), deparser()) main;

