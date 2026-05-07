// protoitem.h
// NC_ITEM_* packet structures -- item / inventory protocol.
// Source: E:\ProjectF2\CSCode\protocol\protoitem.h
//
// All opcode values and payload layouts:
// TODO -- packet capture pending (inventory session required)
//
// Known subsystems from 5ZoneServer2.pdb:
//   shineitem.cpp, shineitembuy.cpp, shineitemdrop.cpp,
//   shineitemequip.cpp, shineitempick.cpp, shineitemsell.cpp,
//   shineitemsplit.cpp, shineitemunequip.cpp, shineitemuse.cpp,
//   shineitemupgrade.cpp, shineitemoperate.cpp, inventory.cpp

#pragma once
#include "protocommon.h"

#pragma pack( push, 1 )

/*------------------------------------------------------------------
  NC_ITEM_PICKUP_REQ              TODO
  NC_ITEM_PICKUP_ACK              TODO
  NC_ITEM_PICKUP_CMD              TODO
  NC_ITEM_DROP_REQ                TODO
  NC_ITEM_DROP_ACK                TODO
  NC_ITEM_DROP_CMD                TODO
  NC_ITEM_EQUIP_REQ               TODO
  NC_ITEM_EQUIP_ACK               TODO
  NC_ITEM_EQUIP_CMD               TODO
  NC_ITEM_UNEQUIP_REQ             TODO
  NC_ITEM_UNEQUIP_ACK             TODO
  NC_ITEM_UNEQUIP_CMD             TODO
  NC_ITEM_USE_REQ                 TODO
  NC_ITEM_USE_ACK                 TODO
  NC_ITEM_MOVE_REQ                TODO
  NC_ITEM_MOVE_ACK                TODO
  NC_ITEM_UPGRADE_REQ             TODO
  NC_ITEM_UPGRADE_ACK             TODO
  NC_ITEM_UPGRADE_CMD             TODO
  NC_ITEM_DISMANTLE_REQ           TODO
  NC_ITEM_DISMANTLE_ACK           TODO
  NC_ITEM_MIX_REQ                 TODO
  NC_ITEM_MIX_ACK                 TODO
  NC_ITEM_REPURCHASE_REQ          TODO
  NC_ITEM_REPURCHASE_ACK          TODO
  NC_ITEM_SPLIT_REQ               TODO
  NC_ITEM_SPLIT_ACK               TODO
------------------------------------------------------------------*/

#pragma pack( pop )
