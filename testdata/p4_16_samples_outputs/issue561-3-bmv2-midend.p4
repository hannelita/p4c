#include <core.p4>
#define V1MODEL_VERSION 20180101
#include <v1model.p4>

header S {
    bit<8> t;
}

header O1 {
    bit<8> data;
}

header O2 {
    bit<16> data;
}

struct headers {
    S  base;
    O1 u_byte;
    O2 u_short;
}

struct metadata {
}

parser ParserImpl(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    state start {
        packet.extract<S>(hdr.base);
        transition select(hdr.base.t) {
            8w0: parseO1;
            8w1: parseO2;
            default: skip;
        }
    }
    state parseO1 {
        packet.extract<O1>(hdr.u_byte);
        transition accept;
    }
    state parseO2 {
        packet.extract<O2>(hdr.u_short);
        transition accept;
    }
    state skip {
        transition accept;
    }
}

control ingress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @noWarn("unused") @name(".NoAction") action NoAction_1() {
    }
    @name("ingress.debug_hdr") table debug_hdr_0 {
        key = {
            hdr.base.t           : exact @name("hdr.base.t") ;
            hdr.u_short.isValid(): exact @name("hdr.u.short.$valid$") ;
            hdr.u_byte.isValid() : exact @name("hdr.u.byte.$valid$") ;
        }
        actions = {
            NoAction_1();
        }
        const default_action = NoAction_1();
    }
    @hidden action issue5613bmv2l69() {
        hdr.u_short.setValid();
        hdr.u_short.data = 16w0xaaaa;
        hdr.u_byte.setInvalid();
        hdr.u_byte.setValid();
        hdr.u_short.setInvalid();
        hdr.u_byte.setValid();
        hdr.u_byte.data = 8w0xff;
        hdr.u_short.setInvalid();
    }
    @hidden action issue5613bmv2l74() {
        hdr.u_byte.setValid();
        hdr.u_byte.data = 8w0xaa;
        hdr.u_short.setInvalid();
        hdr.u_short.setValid();
        hdr.u_byte.setInvalid();
        hdr.u_short.setValid();
        hdr.u_short.data = 16w0xffff;
        hdr.u_byte.setInvalid();
    }
    @hidden table tbl_issue5613bmv2l69 {
        actions = {
            issue5613bmv2l69();
        }
        const default_action = issue5613bmv2l69();
    }
    @hidden table tbl_issue5613bmv2l74 {
        actions = {
            issue5613bmv2l74();
        }
        const default_action = issue5613bmv2l74();
    }
    apply {
        debug_hdr_0.apply();
        if (hdr.u_short.isValid()) {
            tbl_issue5613bmv2l69.apply();
        } else if (hdr.u_byte.isValid()) {
            tbl_issue5613bmv2l74.apply();
        }
    }
}

control egress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    apply {
    }
}

control DeparserImpl(packet_out packet, in headers hdr) {
    apply {
        packet.emit<S>(hdr.base);
        packet.emit<O1>(hdr.u_byte);
        packet.emit<O2>(hdr.u_short);
    }
}

control verifyChecksum(inout headers hdr, inout metadata meta) {
    apply {
    }
}

control computeChecksum(inout headers hdr, inout metadata meta) {
    apply {
    }
}

V1Switch<headers, metadata>(ParserImpl(), verifyChecksum(), ingress(), egress(), computeChecksum(), DeparserImpl()) main;

