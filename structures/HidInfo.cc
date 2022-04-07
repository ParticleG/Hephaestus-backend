//
// Created by ParticleG on 2022/3/28.
//

#include <structures/HidInfo.h>

using namespace hephaestus::structures;
using namespace std;

HidInfo::HidInfo(struct hid_device_info *info) :
        path(info->path),
        manufacturer_string(info->manufacturer_string),
        product_string(info->product_string),
        serial_number(info->serial_number),
        product_id(info->product_id),
        release_number(info->release_number),
        usage_page(info->usage_page),
        usage(info->usage),
        vendor_id(info->vendor_id),
        collection_number(getCollectionNumber(string(info->path))),
        interface_number(info->interface_number) {}
