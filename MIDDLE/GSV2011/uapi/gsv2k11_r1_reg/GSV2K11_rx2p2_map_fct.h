#ifndef GSV2K11_RX2P2_MAP_FCT_H
#define GSV2K11_RX2P2_MAP_FCT_H
#define GSV2K11_RX2P2_get_RX_DISABLE_ECC_CHECK(gsv_config,port, pval)               AvHalI2cReadField8(gsv_config,GSV2K11_RX2P2_MAP_ADDR(port), 0x07, 0x1, 0x0, pval)
#define GSV2K11_RX2P2_set_RX_DISABLE_ECC_CHECK(gsv_config,port, val)                AvHalI2cWriteField8(gsv_config,GSV2K11_RX2P2_MAP_ADDR(port), 0x07, 0x1, 0x0, val)
#define GSV2K11_RX2P2_set_RX_HDCP2P2_REAUTH_REQUEST(gsv_config,port, val)           AvHalI2cWriteField8(gsv_config,GSV2K11_RX2P2_MAP_ADDR(port), 0x15, 0x1, 0x0, val)
#define GSV2K11_RX2P2_get_RX_STREAMID_TYPE_CLEAR(gsv_config,port, pval)             AvHalI2cReadField8(gsv_config,GSV2K11_RX2P2_MAP_ADDR(port), 0x24, 0x1, 0x0, pval)
#define GSV2K11_RX2P2_get_RX_STREAMID_TYPE(gsv_config,port, pval)                   AvHalI2cReadField32BE(gsv_config,GSV2K11_RX2P2_MAP_ADDR(port), 0x4E, 0xFF, 0xFF, 0, 2, pval)
#endif
