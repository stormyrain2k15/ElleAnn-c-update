/* Auto-generated from 5ZoneServer2-Port80-NoService.exe + .udd */
namespace Op {

    /* ── NC_ACT ──────────────────────────────────────────── */
    constexpr uint16_t NC_ACT_CHAT_REQ                                         = 0x0801;
    constexpr uint16_t NC_ACT_WALK_REQ                                         = 0x0803;
    constexpr uint16_t NC_ACT_RUN_REQ                                          = 0x0805;
    constexpr uint16_t NC_ACT_CHANGEMODE_REQ                                   = 0x0808;
    constexpr uint16_t NC_ACT_NPCCLICK_CMD                                     = 0x080A;
    constexpr uint16_t NC_ACT_ENDOFTRADE_CMD                                   = 0x080B;
    constexpr uint16_t NC_ACT_STOP_REQ                                         = 0x0812;
    constexpr uint16_t NC_ACT_MOVEWALK_CMD                                     = 0x0817;
    constexpr uint16_t NC_ACT_MOVERUN_CMD                                      = 0x0819;
    constexpr uint16_t NC_ACT_NPCMENUOPEN_ACK                                  = 0x081D;
    constexpr uint16_t NC_ACT_SHOUT_CMD                                        = 0x081E;
    constexpr uint16_t NC_ACT_EMOTICON_CMD                                     = 0x0820;
    constexpr uint16_t NC_ACT_EMOTICONSTOP_CMD                                 = 0x0822;
    constexpr uint16_t NC_ACT_JUMP_CMD                                         = 0x0824;
    constexpr uint16_t NC_ACT_PITCHTENT_REQ                                    = 0x0827;
    constexpr uint16_t NC_ACT_FOLDTENT_REQ                                     = 0x082A;
    constexpr uint16_t NC_ACT_GATHERSTART_REQ                                  = 0x082D;
    constexpr uint16_t NC_ACT_GATHERCANCEL_CMD                                 = 0x0830;
    constexpr uint16_t NC_ACT_GATHERCOMPLETE_REQ                               = 0x0832;
    constexpr uint16_t NC_ACT_PRODUCE_CAST_REQ                                 = 0x0835;
    constexpr uint16_t NC_ACT_PRODUCE_CASTABORT_CMD                            = 0x0839;
    constexpr uint16_t NC_ACT_RIDE_FEEDING_REQ                                 = 0x0844;
    constexpr uint16_t NC_ACT_ROAR_REQ                                         = 0x084B;
    constexpr uint16_t NC_ACT_WEDDING_PROPOSEREQ_REQ                           = 0x084F;
    constexpr uint16_t NC_ACT_WEDDING_PROPOSEACK_ACK                           = 0x0851;
    constexpr uint16_t NC_ACT_WEDDING_HALL_RESERV_REQ                          = 0x0853;
    constexpr uint16_t NC_ACT_WEDDING_HALL_GUEST_ENTER_READY_REQ               = 0x0856;
    constexpr uint16_t NC_ACT_WEDDING_HALL_GUEST_ENTER_REQ                     = 0x0858;
    constexpr uint16_t NC_ACT_WEDDING_AGREEMENT_DIVORCE_REQ                    = 0x085B;
    constexpr uint16_t NC_ACT_WEDDING_COMPULSORY_DIVORCE_REQ                   = 0x085C;
    constexpr uint16_t NC_ACT_WEDDING_DIVORCE_REFUSE_CMD                       = 0x085E;
    constexpr uint16_t NC_ACT_COOKING_CAST_REQ                                 = 0x085F;
    constexpr uint16_t NC_ACT_COOKING_CASTABORT_CMD                            = 0x0863;
    constexpr uint16_t NC_ACT_ACTIONBYITEM_REQ                                 = 0x0867;
    constexpr uint16_t NC_ACT_RIDE_OFF_REQ                                     = 0x086D;
    constexpr uint16_t NC_ACT_AUTO_WAY_FINDING_USE_GATE_REQ                    = 0x0870;

    /* ── NC_AUCTION ──────────────────────────────────────────── */
    constexpr uint16_t NC_AUCTION_SEARCH_REQ                                   = 0x2E07;
    constexpr uint16_t NC_AUCTION_BUY_REQ                                      = 0x2E0B;
    constexpr uint16_t NC_AUCTION_MY_REGISTER_GOOD_LIST_REQ                    = 0x2E0F;
    constexpr uint16_t NC_AUCTION_REGISTER_GOOD_REQ                            = 0x2E13;
    constexpr uint16_t NC_AUCTION_REGISTER_CANCEL_GOOD_REQ                     = 0x2E17;
    constexpr uint16_t NC_AUCTION_MY_STORAGE_BOX_REQ                           = 0x2E1B;
    constexpr uint16_t NC_AUCTION_MY_STORAGE_ITEM_DETAIL_INFO_REQ              = 0x2E1F;
    constexpr uint16_t NC_AUCTION_STORAGE_BOX_WITHDRAW_REQ                     = 0x2E25;

    /* ── NC_BAT ──────────────────────────────────────────── */
    constexpr uint16_t NC_BAT_TARGETTING_REQ                                   = 0x0901;
    constexpr uint16_t NC_BAT_HIT_REQ                                          = 0x0903;
    constexpr uint16_t NC_BAT_UNTARGET_REQ                                     = 0x0908;
    constexpr uint16_t NC_BAT_SMASH_REQ                                        = 0x0912;
    constexpr uint16_t NC_BAT_SKILLCAST_REQ                                    = 0x0918;
    constexpr uint16_t NC_BAT_SKILLCASTABORT_CMD                               = 0x091C;
    constexpr uint16_t NC_BAT_BASHSTART_CMD                                    = 0x092B;
    constexpr uint16_t NC_BAT_BASHSTOP_CMD                                     = 0x0932;
    constexpr uint16_t NC_BAT_ASSIST_REQ                                       = 0x093E;
    constexpr uint16_t NC_BAT_SKILLBASH_OBJ_CAST_REQ                           = 0x0940;
    constexpr uint16_t NC_BAT_SKILLBASH_FLD_CAST_REQ                           = 0x0941;
    constexpr uint16_t NC_BAT_SKILLBASH_CASTABORT_REQ                          = 0x0944;
    constexpr uint16_t NC_BAT_ABSTATE_ERASE_REQ                                = 0x0954;

    /* ── NC_BOOTH ──────────────────────────────────────────── */
    constexpr uint16_t NC_BOOTH_OPEN_REQ                                       = 0x1A01;
    constexpr uint16_t NC_BOOTH_CLOSE_REQ                                      = 0x1A04;
    constexpr uint16_t NC_BOOTH_ENTRY_REQ                                      = 0x1A07;
    constexpr uint16_t NC_BOOTH_REFRESH_REQ                                    = 0x1A0A;
    constexpr uint16_t NC_BOOTH_ITEMTRADE_REQ                                  = 0x1A0D;
    constexpr uint16_t NC_BOOTH_INTERIORSTART_REQ                              = 0x1A10;

    /* ── NC_BRIEFINFO ──────────────────────────────────────────── */
    constexpr uint16_t NC_BRIEFINFO_INFORM_CMD                                 = 0x0701;

    /* ── NC_CHAR ──────────────────────────────────────────── */
    constexpr uint16_t NC_CHAR_REVIVE_REQ                                      = 0x044E;
    constexpr uint16_t NC_CHAR_STAT_INCPOINT_REQ                               = 0x045C;
    constexpr uint16_t NC_CHAR_STAT_DECPOINT_REQ                               = 0x0462;
    constexpr uint16_t NC_CHAR_LOGOUTREADY_CMD                                 = 0x0471;
    constexpr uint16_t NC_CHAR_LOGOUTCANCEL_CMD                                = 0x0472;
    constexpr uint16_t NC_CHAR_WEDDING_PARTNER_INFO_REQ                        = 0x0493;
    constexpr uint16_t NC_CHAR_DEPOLYMORPH_CMD                                 = 0x04B0;
    constexpr uint16_t NC_CHAR_SAVE_LINK_REQ                                   = 0x04B9;
    constexpr uint16_t NC_CHAR_CLIENT_AUTO_PICK_REQ                            = 0x04BC;
    constexpr uint16_t NC_CHAR_CLIENT_FREESTAT_APPLICATION_REQ                 = 0x04C2;

    /* ── NC_CHARSAVE ──────────────────────────────────────────── */
    constexpr uint16_t NC_CHARSAVE_UI_STATE_SAVE_REQ                           = 0x1025;

    /* ── NC_CT ──────────────────────────────────────────── */
    constexpr uint16_t NC_CT_SET_CURRENT_REQ                                   = 0x1801;

    /* ── NC_DICE ──────────────────────────────────────────── */
    constexpr uint16_t NC_DICE_TAISAI_GAME_JOIN_REQ                            = 0x2B01;
    constexpr uint16_t NC_DICE_TAISAI_GAME_LEAVE_REQ                           = 0x2B05;
    constexpr uint16_t NC_DICE_TAISAI_EXPECT_INCOME_MONEY_REQ                  = 0x2B0B;
    constexpr uint16_t NC_DICE_TAISAI_RECEIPT_INCOME_MONEY_REQ                 = 0x2B0F;
    constexpr uint16_t NC_DICE_TAISAI_GAME_MODE_CHANGE_REQ                     = 0x2B13;
    constexpr uint16_t NC_DICE_TAISAI_RANK_REQ                                 = 0x2B16;
    constexpr uint16_t NC_DICE_TAISAI_BETTING_REQ                              = 0x2B1A;
    constexpr uint16_t NC_DICE_TAISAI_BETTING_CANCEL_REQ                       = 0x2B1D;
    constexpr uint16_t NC_DICE_TAISAI_LARGE_AMOUNT_REQ                         = 0x2B28;

    /* ── NC_GAMBLE ──────────────────────────────────────────── */
    constexpr uint16_t NC_GAMBLE_GAMBLEHOUSE_EXIT_REQ                          = 0x2F02;
    constexpr uint16_t NC_GAMBLE_COIN_VIPCARD_UI_OPEN_REQ                      = 0x2F0D;
    constexpr uint16_t NC_GAMBLE_COIN_BUY_REQ                                  = 0x2F0F;
    constexpr uint16_t NC_GAMBLE_COIN_SELL_REQ                                 = 0x2F13;
    constexpr uint16_t NC_GAMBLE_TYPE_AND_WHERE_STAND_REQ                      = 0x2F17;
    constexpr uint16_t NC_GAMBLE_DICE_TAISAI_GAME_JOIN_REQ                     = 0x2F64;
    constexpr uint16_t NC_GAMBLE_DICE_TAISAI_GAME_LEAVE_REQ                    = 0x2F68;
    constexpr uint16_t NC_GAMBLE_DICE_TAISAI_ALL_RANK_REQ                      = 0x2F6B;
    constexpr uint16_t NC_GAMBLE_DICE_TAISAI_CURR_RANK_REQ                     = 0x2F6F;
    constexpr uint16_t NC_GAMBLE_DICE_TAISAI_BETTING_REQ                       = 0x2F71;
    constexpr uint16_t NC_GAMBLE_DICE_TAISAI_BETTING_CANCEL_REQ                = 0x2F74;
    constexpr uint16_t NC_GAMBLE_SLOTMACHINE_GAME_JOIN_REQ                     = 0x2FC8;
    constexpr uint16_t NC_GAMBLE_SLOTMACHINE_GAME_LEAVE_REQ                    = 0x2FCA;
    constexpr uint16_t NC_GAMBLE_SLOTMACHINE_START_REQ                         = 0x2FCC;
    constexpr uint16_t NC_GAMBLE_SLOTMACHINE_STOPBUTTON_REQ                    = 0x2FCE;
    constexpr uint16_t NC_GAMBLE_SLOTMACHINE_WHEELSTOP_REQ                     = 0x2FD0;
    constexpr uint16_t NC_GAMBLE_SLOTMACHINE_JACKPOTRANKING_REQ                = 0x2FD9;
    constexpr uint16_t NC_GAMBLE_SLOTMACHINE_WINRANKING_REQ                    = 0x2FDB;

    /* ── NC_GUILD ──────────────────────────────────────────── */
    constexpr uint16_t NC_GUILD_STORAGEOPEN_REQ                                = 0x1D91;
    constexpr uint16_t NC_GUILD_TOURNAMENT_RECALL_ENTER_REQ                    = 0x1D9C;
    constexpr uint16_t NC_GUILD_RENAME_REQ                                     = 0x1DAC;
    constexpr uint16_t NC_GUILD_RETYPE_REQ                                     = 0x1DB4;
    constexpr uint16_t NC_GUILD_TOURNAMENT_FLAGCAPTURE_REQ                     = 0x1DCA;
    constexpr uint16_t NC_GUILD_TOURNAMENT_USESKILL_REQ                        = 0x1DCF;
    constexpr uint16_t NC_GUILD_TOURNAMENT_DICEGAME_THROW_REQ                  = 0x1DD3;
    constexpr uint16_t NC_GUILD_TOURNAMENT_OBSERVER_ENTER_REQ                  = 0x1DD7;
    constexpr uint16_t NC_GUILD_TOURNAMENT_OBSERVER_OUT_REQ                    = 0x1DDB;
    constexpr uint16_t NC_GUILD_ACADEMY_REWARD_STORAGE_OPEN_REQ                = 0x266D;
    constexpr uint16_t NC_GUILD_ACADEMY_REWARD_STORAGE_CLOSE_CMD               = 0x266F;

    /* ── NC_HOLY ──────────────────────────────────────────── */
    constexpr uint16_t NC_HOLY_PROMISE_GET_CEN_REWARD_REQ                      = 0x253C;
    constexpr uint16_t NC_HOLY_PROMISE_WITHDRAW_CEN_REWARD_REQ                 = 0x2540;

    /* ── NC_INSTANCE ──────────────────────────────────────────── */
    constexpr uint16_t NC_INSTANCE_DUNGEON_DELETE_DUNGEON_REQ                  = 0x2905;

    /* ── NC_ITEM ──────────────────────────────────────────── */
    constexpr uint16_t NC_ITEM_BUY_REQ                                         = 0x0C03;
    constexpr uint16_t NC_ITEM_SELL_REQ                                        = 0x0C06;
    constexpr uint16_t NC_ITEM_DROP_REQ                                        = 0x0C07;
    constexpr uint16_t NC_ITEM_PICK_REQ                                        = 0x0C09;
    constexpr uint16_t NC_ITEM_RELOC_REQ                                       = 0x0C0B;
    constexpr uint16_t NC_ITEM_SPLIT_REQ                                       = 0x0C0D;
    constexpr uint16_t NC_ITEM_EQUIP_REQ                                       = 0x0C0F;
    constexpr uint16_t NC_ITEM_RINGEQUIP_REQ                                   = 0x0C10;
    constexpr uint16_t NC_ITEM_UNEQUIP_REQ                                     = 0x0C12;
    constexpr uint16_t NC_ITEM_USE_REQ                                         = 0x0C15;
    constexpr uint16_t NC_ITEM_UPGRADE_REQ                                     = 0x0C17;
    constexpr uint16_t NC_ITEM_USEABORT_CMD                                    = 0x0C19;
    constexpr uint16_t NC_ITEM_DEPOSIT_REQ                                     = 0x0C1C;
    constexpr uint16_t NC_ITEM_WITHDRAW_REQ                                    = 0x0C1E;
    constexpr uint16_t NC_ITEM_CHARGEDINVENOPEN_REQ                            = 0x0C20;
    constexpr uint16_t NC_ITEM_CHARGED_WITHDRAW_REQ                            = 0x0C22;
    constexpr uint16_t NC_ITEM_GUILD_STORAGE_WITHDRAW_REQ                      = 0x0C26;
    constexpr uint16_t NC_ITEM_OPENSTORAGEPAGE_REQ                             = 0x0C28;
    constexpr uint16_t NC_ITEM_DISMANTLE_REQ                                   = 0x0C2A;
    constexpr uint16_t NC_ITEM_REWARDINVENOPEN_REQ                             = 0x0C2C;
    constexpr uint16_t NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_REQ       = 0x0C2F;
    constexpr uint16_t NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_REQ        = 0x0C31;
    constexpr uint16_t NC_ITEM_MH_FURNITURE_ENDURE_KIT_USE_REQ                 = 0x0C33;
    constexpr uint16_t NC_ITEM_WEAPONENDURE_CHARGE_REQ                         = 0x0C35;
    constexpr uint16_t NC_ITEM_REVIVEITEMUSE_CMD                               = 0x0C39;
    constexpr uint16_t NC_ITEM_DICE_GAME_START_REQ                             = 0x0C3C;
    constexpr uint16_t NC_ITEM_ENCHANT_ADD_GEM_REQ                             = 0x0C40;
    constexpr uint16_t NC_ITEM_ENCHANT_REMOVE_GEM_REQ                          = 0x0C42;
    constexpr uint16_t NC_ITEM_ENCHANT_ADD_NEW_SOCKET_REQ                      = 0x0C44;
    constexpr uint16_t NC_ITEM_EQUIP_BELONGED_CANCEL_USE_REQ                   = 0x0C48;

    /* ── NC_ITEMDB ──────────────────────────────────────────── */
    constexpr uint16_t NC_ITEMDB_CLOSE_GUILD_STORAGE_CMD                       = 0x1DA0;

    /* ── NC_KQ ──────────────────────────────────────────── */
    constexpr uint16_t NC_KQ_ENTRYRESPONCE_ACK                                 = 0x161A;

    /* ── NC_LOG ──────────────────────────────────────────── */
    constexpr uint16_t NC_LOG_MAP_NOBASE_CMD                                   = 0x0384;

    /* ── NC_MAP ──────────────────────────────────────────── */
    constexpr uint16_t NC_MAP_LOGIN_REQ                                        = 0x0601;
    constexpr uint16_t NC_MAP_LOGINCOMPLETE_CMD                                = 0x0603;
    constexpr uint16_t NC_MAP_WING_SAVE_REQ                                    = 0x0614;
    constexpr uint16_t NC_MAP_WING_FLY_REQ                                     = 0x0616;
    constexpr uint16_t NC_MAP_TOWNPORTAL_REQ                                   = 0x061A;
    constexpr uint16_t NC_MAP_TONORMALCOORD_CMD                                = 0x061C;
    constexpr uint16_t NC_MAP_MULTY_LINK_SELECT_REQ                            = 0x061F;

    /* ── NC_MENU ──────────────────────────────────────────── */
    constexpr uint16_t NC_MENU_SERVERMENU_ACK                                  = 0x0F02;

    /* ── NC_MINIHOUSE ──────────────────────────────────────────── */
    constexpr uint16_t NC_MINIHOUSE_ACTIV_REQ                                  = 0x2301;
    constexpr uint16_t NC_MINIHOUSE_BUILDING_REQ                               = 0x2303;
    constexpr uint16_t NC_MINIHOUSE_LOGINCOMPLETE_REQ                          = 0x230A;
    constexpr uint16_t NC_MINIHOUSE_VISITREADY_REQ                             = 0x230E;
    constexpr uint16_t NC_MINIHOUSE_VISIT_REQ                                  = 0x2310;
    constexpr uint16_t NC_MINIHOUSE_KICKOUT_REQ                                = 0x2312;
    constexpr uint16_t NC_MINIHOUSE_EXIT_REQ                                   = 0x2318;
    constexpr uint16_t NC_MINIHOUSE_FUNITUREINFOCOMPLETE_REQ                   = 0x231D;
    constexpr uint16_t NC_MINIHOUSE_REARRANGE_REQ                              = 0x231F;
    constexpr uint16_t NC_MINIHOUSE_CREATE_FURNITURE_REQ                       = 0x2322;
    constexpr uint16_t NC_MINIHOUSE_DELETE_FURNITURE_REQ                       = 0x2325;
    constexpr uint16_t NC_MINIHOUSE_MODIFY_PASSWORD_REQ                        = 0x232B;
    constexpr uint16_t NC_MINIHOUSE_MODIFY_TITLE_REQ                           = 0x232E;
    constexpr uint16_t NC_MINIHOUSE_MODIFY_MAXENTERNUM_REQ                     = 0x2332;
    constexpr uint16_t NC_MINIHOUSE_CHAR_ACTION_REQ                            = 0x233D;
    constexpr uint16_t NC_MINIHOUSE_FURNITURE_EFFECT_REQ                       = 0x2340;
    constexpr uint16_t NC_MINIHOUSE_MODIFY_ITEM_INFO_OPEN_REQ                  = 0x2343;
    constexpr uint16_t NC_MINIHOUSE_MODIFY_NOTIFY_REQ                          = 0x2348;
    constexpr uint16_t NC_MINIHOUSE_PORTAL_OPEN_REQ                            = 0x2350;
    constexpr uint16_t NC_MINIHOUSE_PORTAL_CLOSE_REQ                           = 0x2353;
    constexpr uint16_t NC_MINIHOUSE_PORTAL_EFFECT_REQ                          = 0x2356;
    constexpr uint16_t NC_MINIHOUSE_FURNITURE_EMOTION_REQ                      = 0x2362;
    constexpr uint16_t NC_MINIHOUSE_FURNITURE_EMOTION_CANCEL_REQ               = 0x2365;

    /* ── NC_MISC ──────────────────────────────────────────── */
    constexpr uint16_t NC_MISC_SEED_REQ                                        = 0x0206;
    constexpr uint16_t NC_MISC_PINGTEST_CLIENT_ZONE_DB                         = 0x0212;
    constexpr uint16_t NC_MISC_PINGTEST_TOOL_WM_CLIENT_ZONE_DB                 = 0x0213;
    constexpr uint16_t NC_MISC_HIDE_EXCEPT_ME_ON_CMD                           = 0x0217;
    constexpr uint16_t NC_MISC_HIDE_EXCEPT_ME_OFF_CMD                          = 0x0218;
    constexpr uint16_t NC_MISC_SERVERPARAMETER_REQ                             = 0x022F;
    constexpr uint16_t NC_MISC_TIMERECORD_REQ                                  = 0x023E;

    /* ── NC_PARTY ──────────────────────────────────────────── */
    constexpr uint16_t NC_PARTY_MEMBERINFOREQ_CMD                              = 0x0E48;

    /* ── NC_QUEST ──────────────────────────────────────────── */
    constexpr uint16_t NC_QUEST_SCRIPT_CMD_ACK                                 = 0x1102;
    constexpr uint16_t NC_QUEST_GIVE_UP_REQ                                    = 0x1107;
    constexpr uint16_t NC_QUEST_CLIENT_SCENARIO_DONE_REQ                       = 0x110B;
    constexpr uint16_t NC_QUEST_SELECT_START_REQ                               = 0x110F;
    constexpr uint16_t NC_QUEST_REWARD_SELECT_ITEM_INDEX_CMD                   = 0x1111;
    constexpr uint16_t NC_QUEST_START_REQ                                      = 0x1114;
    constexpr uint16_t NC_QUEST_READ_REQ                                       = 0x1116;

    /* ── NC_SCENARIO ──────────────────────────────────────────── */
    constexpr uint16_t NC_SCENARIO_AREAENTRY_ACK                               = 0x1B06;
    constexpr uint16_t NC_SCENARIO_AREALEAVE_ACK                               = 0x1B08;

    /* ── NC_SKILL ──────────────────────────────────────────── */
    constexpr uint16_t NC_SKILL_EMPOWALLOC_REQ                                 = 0x1211;
    constexpr uint16_t NC_SKILL_PRODUCTFIELD_REQ                               = 0x1222;
    constexpr uint16_t NC_SKILL_UNLEARN_REQ                                    = 0x1224;
    constexpr uint16_t NC_SKILL_REPLYREVIVE_CMD                                = 0x122A;

    /* ── NC_SOULSTONE ──────────────────────────────────────────── */
    constexpr uint16_t NC_SOULSTONE_HP_BUY_REQ                                 = 0x1401;
    constexpr uint16_t NC_SOULSTONE_SP_BUY_REQ                                 = 0x1402;
    constexpr uint16_t NC_SOULSTONE_HP_USE_REQ                                 = 0x1407;
    constexpr uint16_t NC_SOULSTONE_SP_USE_REQ                                 = 0x1409;

    /* ── NC_TRADE ──────────────────────────────────────────── */
    constexpr uint16_t NC_TRADE_PROPOSE_REQ                                    = 0x1301;
    constexpr uint16_t NC_TRADE_PROPOSE_ASKNO_ACK                              = 0x1303;
    constexpr uint16_t NC_TRADE_PROPOSEYES_ACK                                 = 0x1306;
    constexpr uint16_t NC_TRADE_PROPOSE_CANCEL_CMD                             = 0x1307;
    constexpr uint16_t NC_TRADE_CANCEL_REQ                                     = 0x130A;
    constexpr uint16_t NC_TRADE_UPBOARD_REQ                                    = 0x130D;
    constexpr uint16_t NC_TRADE_DOWNBOARD_REQ                                  = 0x1311;
    constexpr uint16_t NC_TRADE_CENBOARDING_REQ                                = 0x1315;
    constexpr uint16_t NC_TRADE_BOARDLOCK_REQ                                  = 0x1319;
    constexpr uint16_t NC_TRADE_DECIDE_REQ                                     = 0x131F;

    /* ── NC_USER ──────────────────────────────────────────── */
    constexpr uint16_t NC_USER_NORMALLOGOUT_CMD                                = 0x0318;
    constexpr uint16_t NC_USER_USE_BEAUTY_SHOP_CMD                             = 0x0332;
    constexpr uint16_t NC_USER_CONNECTION_CHANGE_CHAR_ID_ITEM_USE_REQ          = 0x2D10;

    /* ── NC_WT ──────────────────────────────────────────── */
    constexpr uint16_t NC_WT_LICENSE_REQ                                       = 0x1701;

}  /* namespace Op */