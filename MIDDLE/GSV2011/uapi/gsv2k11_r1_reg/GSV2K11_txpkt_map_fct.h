#ifndef GSV2K11_TXPKT_MAP_FCT_H
#define GSV2K11_TXPKT_MAP_FCT_H
#define GSV2K11_TXPKT_set_AVI_IF_TYPE(gsv_config,port, val)                         AvHalI2cWriteField8(gsv_config,GSV2K11_TXPKT_MAP_ADDR(port), 0x1F, 0xFF, 0, val)
#define GSV2K11_TXPKT_set_AVI_IF_VER(gsv_config,port, val)                          AvHalI2cWriteField8(gsv_config,GSV2K11_TXPKT_MAP_ADDR(port), 0x20, 0xFF, 0, val)
#define GSV2K11_TXPKT_set_AVI_IF_LEN(gsv_config,port, val)                          AvHalI2cWriteField8(gsv_config,GSV2K11_TXPKT_MAP_ADDR(port), 0x21, 0xFF, 0, val)
#define GSV2K11_TXPKT_get_AUD_IF_CA7_0(gsv_config,port, pval)                       AvHalI2cReadField8(gsv_config,GSV2K11_TXPKT_MAP_ADDR(port), 0x64, 0xFF, 0, pval)
#define GSV2K11_TXPKT_set_AUD_IF_CA7_0(gsv_config,port, val)                        AvHalI2cWriteField8(gsv_config,GSV2K11_TXPKT_MAP_ADDR(port), 0x64, 0xFF, 0, val)
#define GSV2K11_TXPKT_get_TX_PKT_UPDATE(gsv_config,port, pval)                      AvHalI2cReadField32LE(gsv_config,GSV2K11_TXPKT_MAP_ADDR(port), 0xFB, 0x7F, 0xFF, 0, 2, pval)
#define GSV2K11_TXPKT_set_TX_PKT_UPDATE(gsv_config,port, val)                       AvHalI2cWriteField32LE(gsv_config,GSV2K11_TXPKT_MAP_ADDR(port), 0xFB, 0x7F, 0xFF, 0, 2, val)
#endif
