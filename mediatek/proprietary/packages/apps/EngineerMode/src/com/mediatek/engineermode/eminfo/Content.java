/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.engineermode.eminfo;

import com.mediatek.engineermode.Elog;

import java.lang.reflect.Field;
import java.util.Locale;

public class Content {
    private static final String TAG = "EmInfo";

    public static final int RR_EM_CELL_SELECT_PARA_INFO = 0;
    public static final int RR_EM_CHANNEL_DESCR_INFO = 1;
    public static final int RR_EM_CTRL_CHANNEL_DESCR_INFO = 2;
    public static final int RR_EM_RACH_CTRL_PARA_INFO = 3;
    public static final int RR_EM_LAI_INFO = 4;
    public static final int RR_EM_RADIO_LINK_COUNTER_INFO = 5;
    public static final int RR_EM_MEASUREMENT_REPORT_INFO = 6;
    public static final int RR_EM_CA_LIST_INFO = 7;
    public static final int RR_EM_CONTROL_MSG_INFO = 8;
    public static final int RR_EM_SI2Q_INFO_STRUCT_INFO = 9;
    public static final int RR_EM_MI_INFO_STRUCT_INFO = 10;
    public static final int RR_EM_BLK_INFO = 11;
    public static final int RR_EM_TBF_INFO = 12;
    public static final int RR_EM_GPRS_GENERAL_INFO = 13;
    public static final int RRM_EM_FDD_IR_PARAMETER_STATUS_IND_STRUCT_INFO = 14;
    public static final int RRM_EM_IR_RESELECT_STATUS_IND_STRUCT_INFO = 15;
    public static final int RRM_EM_IR_3G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO = 16;
    public static final int RRM_EM_IR_3G_NEIGHBOR_MEAS_INFO_IND_STRUCT_INFO = 17;
    public static final int RRM_EM_IR_4G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO = 18;
    public static final int RRM_EM_IR_4G_NEIGHBOR_MEAS_INFO_IND_STRUCT_INFO = 19;
    public static final int RRM_EM_SERV_CELL_POWER_STATUS_IND_STRUCT_INFO = 20;
    public static final int RRM_EM_IR_3G_NEIGHBOR_CELL_STATUS_IND_STRUCT_INFO = 21;
    public static final int RRM_EM_IR_4G_NEIGHBOR_CELL_STATUS_IND_STRUCT_INFO = 22;
    public static final int RRM_EM_TDD_IR_PARAMETER_STATUS_IND_STRUCT_INFO = 23;
    public static final int RRM_EM_SUCCESS_RATE_KPI_INFO = 24;
    public static final int CC_EM_CHANNEL_INFO = 50;
    public static final int CC_EM_CALL_INFO = 51;
    public static final int SS_EM_INFO = 52;
    public static final int MM_EM_INFO = 53;
    public static final int MMRR_EM_PLMN_INFO_STRUCT_INFO = 54;
    public static final int UEM_EM_BATTERY_INFO = 55;
    public static final int GMM_EM_INFO = 56;
    public static final int LLC_EM_INFO = 57;
    public static final int SM_EM_INFO = 58;
    public static final int EM_TCM_INFO_IND = 59;
    public static final int MMRR_EM_PLMN_LIST_REQ_STRUCT_INFO = 60;
    public static final int MMRR_EM_PLMN_LIST_CNF_STRUCT_INFO = 61;
    public static final int MMRR_EM_PLMN_SEARCH_REQ_STRUCT_INFO = 62;
    public static final int MMRR_EM_PLMN_SEARCH_CNF_STRUCT_INFO = 63;
    public static final int MMRR_EM_HPLMN_LIST_INFO = 64;
    public static final int MMRR_EM_OPLMN_LIST_INFO = 65;
    public static final int MMRR_EM_STATIC_APPLMN_LIST_INFO = 66;
    public static final int MMRR_EM_DYNAMIC_APPLMN_LIST_INFO = 67;
    public static final int MMRR_EM_PLMN_LOSS_INFO_STRUCT_INFO = 68;
    public static final int MMRR_EM_PLMN_SEARCH_CNF_INFO_STRUCT_INFO = 69;
    public static final int FDD_EM_URR_3G_GENERAL_STATUS_IND = 70;
    public static final int EM_SIBE_3G_SIB_IND_STRUCT_INFO = 71;
    public static final int TDD_EM_URR_3G_GENERAL_STATUS_IND = 72;
    public static final int FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO = 75;
    public static final int FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO = 76;
    public static final int FDD_EM_CSCE_R_STATUS_IND_STRUCT_INFO = 77;
    public static final int FDD_EM_CSCE_H_STATUS_IND_STRUCT_INFO = 78;
    public static final int FDD_EM_CSCE_APBCR_STATUS_IND_STRUCT_INFO = 79;
    public static final int FDD_EM_CSCE_MEAS_RULE_STATUS_IND_STRUCT_INFO = 80;
    public static final int FDD_EM_CSCE_MULTIPLE_PLMN_IND_STRUCT_INFO = 81;
    public static final int TDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO = 82;
    public static final int TDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO = 83;
    public static final int TDD_EM_CSCE_R_STATUS_IND_STRUCT_INFO = 84;
    public static final int TDD_EM_CSCE_H_STATUS_IND_STRUCT_INFO = 85;
    public static final int TDD_EM_CSCE_APBCR_STATUS_IND_STRUCT_INFO = 86;
    public static final int TDD_EM_CSCE_MEAS_RULE_STATUS_IND_STRUCT_INFO = 87;
    public static final int TDD_EM_CSCE_MULTIPLE_PLMN_IND_STRUCT_INFO = 88;
    public static final int EM_TDD_MEME_INFO_DCH_UMTS_CELL_INFO = 90;
    public static final int EM_TDD_MEME_INFO_DCH_GSM_CELL_INFO = 91;
    public static final int EM_TDD_MEME_INFO_DCH_LTE_CELL_INFO = 92;
    public static final int EM_TDD_MEME_INFO_EVENT_TYPE_1_PARAMETER_STRUCT_INFO = 93;
    public static final int EM_TDD_MEME_INFO_EVENT_TYPE_2_PARAMETER_STRUCT_INFO = 94;
    public static final int EM_TDD_MEME_INFO_EVENT_TYPE_3_PARAMETER_STRUCT_INFO = 95;
    public static final int EM_TDD_MEME_INFO_DCH_3G_BLER_INFO = 96;
    public static final int EM_TDD_MEME_INFO_GSM_CELL_INFO = 97;
    public static final int EM_TDD_MEME_INFO_LTE_CELL_INFO = 98;
    public static final int EM_TDD_MEME_INFO_REPORT_INFO = 99;
    public static final int EM_RRCE_TGPS_STATUS_IND = 110;
    public static final int EM_SLCE_SRNCID_STATUS_IND = 111;
    public static final int EM_UAS_3G_TDD128_HANDOVER_SEQUENCE_IND = 130;
    public static final int EM_RRCE_3G4_REDIR_EVENT = 131;
    public static final int EM_RRCE_KPI_STATUS_IND = 132;
    public static final int EM_SLCE_PS_DATA_RATE_STATUS_IND = 140;
    public static final int EM_RRCE_DCH_STATE_CONFIGURATION_STATUS_IND = 155;
    public static final int EM_RRCE_FACH_STATE_CONFIGURATION_STATUS_IND = 156;
    public static final int EM_RRCE_CS_OVER_HSPA_STATUS_IND = 157;
    public static final int EM_RRCE_3G_SECURITY_CONFIGURATION_INFO_IND = 158;
    public static final int EM_RRCE_FD_CONFIGURATION_STATUS_IND = 159;
    public static final int EM_RRCE_HSPA_CONFIG_IND = 160;
    public static final int UL1_EM_HS_DSCH_CONFIGURATION_INFO = 170;
    public static final int UL1_EM_EDCH_CONFIGURATION_INFO = 171;
    public static final int UL1_EM_CPC_CONFIGURATION_INFO = 172;
    public static final int UL1_EM_SECONDARY_HS_CONFIGURATION_STATUS_INFO = 173;
    public static final int UL1_EM_PRIMARY_HS_DSCH_BLER_INFO = 174;
    public static final int UL1_EM_SECONDARY_HS_DSCH_BLER_INFO = 175;
    public static final int UL1_EM_EDCH_ACK_RATE_INFO = 176;
    public static final int UL1_EM_PRX_DRX_MEASUREMENT_INFO = 177;
    public static final int UL1_EM_HSPA_INFO_GROUP = 178;
    public static final int EM_UL1_TAS_INFO_IND = 179;
    public static final int UL2_EM_ADM_POOL_STATUS_IND_STRUCT_INFO = 185;
    public static final int UL2_EM_PS_DATA_RATE_STATUS_IND_STRUCT_INFO = 186;
    public static final int UL2_EM_HSDSCH_RECONFIG_STATUS_IND_STRUCT_INFO = 187;
    public static final int UL2_EM_URLC_EVENT_STATUS_IND_STRUCT_INFO = 188;
    public static final int UL2_EM_3G_BLER_IND_STRUCT_INFO = 189;
    public static final int UL2_EM_WCDMA_RLC_STATS_STRUCT_INFO = 190;
    public static final int UL2_EM_URLC_LAYER_TPUT_INFO = 191;
    public static final int UL2_EM_HSUPA_SI_IND_STRUCT_INFO = 192;
    public static final int UL2_EM_UMAC_DCH_INFO = 193;
    public static final int UL2_EM_UMAC_EDCH_INFO = 194;
    public static final int UL2_EM_UMAC_HSDSCH_INFO = 195;
    public static final int ERRC_EM_MOB_MEAS_INTRARAT_INFO = 210;
    public static final int ERRC_EM_MOB_MEAS_INTERRAT_UTRAN_INFO = 211;
    public static final int ERRC_EM_MOB_MEAS_INTERRAT_GERAN_INFO = 212;
    public static final int ERRC_EM_AUTOS_CSG_INFO = 213;
    public static final int ERRC_EM_CARRS_EVENT_IND = 214;
    public static final int ERRC_EM_SIB_EVENT_IND = 215;
    public static final int ERRC_EM_MOB_EVENT_IND = 216;
    public static final int ERRC_EM_SEC_PARAM = 217;
    public static final int ERRC_EM_REEST_INFO = 218;
    public static final int ERRC_EM_RECONF_INFO = 219;
    public static final int ERRC_EM_RCM_SIM_STS_INFO = 220;
    public static final int ERRC_EM_SYS_SIB_RX_STS_INFO = 221;
    public static final int ERRC_EM_ERRC_STATE_IND = 222;
    public static final int ERRC_EM_OVER_PROC_DELAY_WARNING = 223;
    public static final int ERRC_EM_LTE_SUPPORTED_BAND_INFO = 224;
    public static final int ERRC_EM_ERRC_KPI_INFO = 225;
    public static final int ERRC_EM_ERRC_CONFIG_INFO = 226;
    public static final int ERRC_EM_CONN_INFO = 227;
    public static final int ESM_ESM_INFO = 228;
    public static final int ESM_L4C_ESM_INFO = 229;
    public static final int EMM_EM_SEC_INFO = 230;
    public static final int EMM_EM_PLMNSEL_INFO = 231;
    public static final int EMM_EM_CONN_INFO = 232;
    public static final int EMM_EM_NASMSG_INFO = 233;
    public static final int EMM_EM_CALL_INFO = 234;
    public static final int EMM_EM_REG_ATTACH_INFO = 235;
    public static final int EMM_EM_REG_DETACH_INFO = 236;
    public static final int EMM_EM_REG_TAU_INFO = 237;
    public static final int EMM_EM_REG_COMMON_INFO = 238;
    public static final int EMM_EM_SV_INFO = 239;
    public static final int EMM_EM_RATBAND_INFO = 240;
    public static final int EMM_EM_TIMERSRV_INFO = 241;
    public static final int EMM_EM_USIMSRV_INFO = 242;
    public static final int EMM_EM_NVMSRV_INFO = 243;
    public static final int EMM_L4C_EMM_INFO = 244;
    public static final int EM_EL2_OV_STATUS = 245;
    public static final int EM_EL1_OV_STATUS = 246;
    public static final int EM_QBM_STATUS = 247;
    public static final int EM_UPCM_STATUS = 248;
    public static final int EM_EL1_INFO = 249;
    public static final int EM_CSR_STATUS_IND = 250;
    public static final int RAC_EM_INFO = 251;
    public static final int EM_EL2_PUB_STATUS = 252;
    public static final int EMM_L4C_LAI_CHANGE_INFO = 253;
    public static final int RATCM_EM_23G_RAT_CHANGE_IND = 254;
    public static final int EM_EL1_B3B39_INFO = 255;
    public static final int EM_L4C_RAT_CHANGE_IND = 275;
    public static final int IPC_EM_UL_THROTTLE_STATUS = 326;
    public static final int ERRC_EM_SERV_IR_NEIGHBOR_INFO = 327;
    public static final int ERRC_EM_IR_REDIR_EVENT = 328;
    public static final int ERRC_EM_IRAT_MEAS_CFG = 329;
    public static final int ERRC_EM_MOB_MEAS_CONFIG_INFO_IND = 330;
    public static final int ERRC_EM_MOB_MEAS_REPORT_INFO_IND = 331;
    public static final int EM_UPCM_PS_TPUT_INFO = 358;
    public static final int MM_EM_MTC_TIMER_INFO = 359;
    public static final int MM_EM_LU_INFO = 360;
    public static final int MM_EM_RAU_INFO = 361;
    public static final int USIME_EM_INFO_CAPABILITY = 362;
    public static final int EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO = 500;
    public static final int EM_FDD_MEME_INFO_DCH_GSM_CELL_INFO = 501;
    public static final int EM_FDD_MEME_INFO_DCH_LTE_CELL_INFO = 502;
    public static final int EM_FDD_MEME_INFO_DCH_H_SERVING_CELL_INFO = 503;
    public static final int EM_FDD_MEME_INFO_DCH_3G_BLER_INFO = 504;
    public static final int EM_FDD_MEME_INFO_FACH_LTE_CELL_INFO = 505;
    public static final int EM_FDD_MEME_INFO_EVENT_TYPE_3_PARAMETER_INFO = 506;
    public static final int EM_FDD_MEME_INFO_REPORT_INFO = 507;
    public static final int EM_VDM_CALL_INFO_IND = 601;
    public static final int NWSEL_EM_TIMER_INFO = 650;
    public static final int TDD_URR_EM_INFO_BEGIN_R2 = 700;
    public static final int TDD_URR_EM_INFO_END_R2 = 899;
    public static final int EM_GSM_TAS_INFO_IND = 950;

    // RR_EM_MEASUREMENT_REPORT_INFO
    public static final String[] RR_EM_MEASUREMENT_REPORT_INFO_STRUCT = new String[] {
        "uint8 rr_state",
        "uint8 meas_mode",
        "uint16 serving_arfcn",
        "uint8 serving_bsic",
        "uint8 serving_current_band",
        "uint8 serv_gprs_supported",
        "int16 serv_rla_in_quarter_dbm",
        "uint8 serv_rla_reported_value",
        "bool is_serv_BCCH_rla_valid",
        "int16 serv_BCCH_rla_in_dedi_state",
        "uint8 quality",
        "bool gprs_pbcch_present",
        "bool gprs_c31_c32_enable",
        "int16 c1_serv_cell",
        "int16 c2_serv_cell",
        "int16 c31_serv_cell",
        "uint8 num_of_carriers",
        "uint16 nc_arfcn[32]",
        "int16 rla_in_quarter_dbm[32]",
        "uint8 rla_in_reported_value[32]",
        "uint8 nc_info_status[32]",
        "uint8 nc_bsic[32]",
        "int32 frame_offset[32]",
        "int32 ebit_offset[32]",
        "int16 c1[32]",
        "int16 c2[32]",
        "int16 c31[32]",
        "uint8 multiband_report",
        "uint8 timing_advance",
        "int16 tx_power_level",
        "int16 serv_rla_full_value_in_quater_dbm",
        "uint8 nco",
        "uint8 rxqual_sub",
        "uint8 rxqual_full",
        "int16 using_tx_power_in_dbm",
        "bool amr_info_valid",
        "uint8 cmr_cmc_cmiu_cmid",
        "uint8 c_i",
        "uint16 icm",
        "uint16 acs",
        "bool dl_dtx_used",
        "uint8 cmr",
        "uint8 cmc",
        "uint8 amr_ul_mode",
        "uint8 amr_dl_mode",
        "uint8 num_of_nc_lai",
        "rr_em_lai_info_struct nc_lai[6]",
        "rr_em_cs_meas_param_struct cs_meas_param",
        "rr_em_ps_meas_param_struct ps_meas_param",
        "uint8 nc_band[32]",
        "uint16 nc_cellid[32]",
        "bool nc_barred[32]",
    };
    public static final String[] RR_EM_LAI_INFO_STRUCT = new String[] {
        "uint8 mcc[3]",
        "uint8 mnc[3]",
        "uint8 lac[2]",
        "uint16 cell_id",
        "uint8 nc_info_index",
        "uint8 rac",
        "uint8 nmo",
        "uint8 supported_Band",
    };
    public static final String[] RR_EM_CS_MEAS_PARAM_STRUCT = new String[] {
        "bool is_valid",
        "uint8 cs_report_type",
        "bool is_invalid_bsic_rp",
        "uint8 cs_serv_band_rp",
        "uint8 cs_multi_band_rp",
        "uint8 cs_rp_threshold_850",
        "uint8 cs_rp_offset_850",
        "uint8 cs_rp_threshold_900",
        "uint8 cs_rp_offset_900",
        "uint8 cs_rp_threshold_1800",
        "uint8 cs_rp_offset_1800",
        "uint8 cs_rp_threshold_1900",
        "uint8 cs_rp_offset_1900",
    };
    public static final String[] RR_EM_PS_MEAS_PARAM_STRUCT = new String[] {
        "bool is_valid",
        "uint8 ps_report_type",
        "bool is_invalid_bsic_rp",
        "uint8 ps_serv_band_rp",
        "uint8 ps_multi_band_rp",
        "uint8 ps_rp_threshold_850",
        "uint8 ps_rp_offset_850",
        "uint8 ps_rp_threshold_900",
        "uint8 ps_rp_offset_900",
        "uint8 ps_rp_threshold_1800",
        "uint8 ps_rp_offset_1800",
        "uint8 ps_rp_threshold_1900",
        "uint8 ps_rp_offset_1900",
    };

    // RRM_EM_IR_3G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO
    public static final String[] EM_RRM_IR_3G_NEIGHBOR_MEAS_STATUS_IND_STRUCT = new String[] {
        "rrm_em_ir_3g_neighbor_meas_status_struct ir_3g_neighbor_meas_status[6]",
    };
    public static final String[] RRM_EM_IR_3G_NEIGHBOR_MEAS_STATUS_STRUCT = new String[] {
        "bool is_valid",
        "uint8 cell_type",
        "uint16 uarfcn",
        "uint16 phy_id",
        "int16 strength",
        "int16 quailty",
        "uint8 rep_value",
        "uint8 non_rep_value",
        "uint8 fail_type",
    };

    // RRM_EM_IR_4G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO
    public static final String[] EM_RRM_IR_4G_NEIGHBOR_MEAS_STATUS_IND_STRUCT = new String[] {
        "rrm_em_ir_4g_neighbor_meas_status_struct ir_4g_neighbor_meas_status[6]",
    };
    public static final String[] RRM_EM_IR_4G_NEIGHBOR_MEAS_STATUS_STRUCT = new String[] {
        "bool is_valid",
        "uint8 cell_type",
        "uint32 earfcn",
        "uint16 pci",
        "uint8 band_type",
        "int16 rsrp",
        "int16 rsrq",
        "uint8 rep_value",
        "uint8 non_rep_value",
        "uint8 fail_type",
    };

    // RRM_EM_SUCCESS_RATE_KPI_INFO
    public static final String[] EM_RRM_SUCCESS_RATE_KPI_IND_STRUCT = new String[] {
        "uint8 proc_id",
        "uint8 status",
        "uint32 attempt",
        "uint32 success",
    };

    // MM_EM_INFO
    public static final String[] EM_MM_INFO_IND_STRUCT = new String[] {
        "uint8 t3212_val",
        "uint8 att_flag",
        "uint8 mm_cause",
        "uint8 mm_state",
        "uint8 mcc[3]",
        "uint8 mnc[3]",
        "uint8 loc[2]",
        "uint8 rac",
        "uint8 tmsi[4]",
        "bool is_t3212_running",
        "uint16 t3212_timer_value",
        "uint16 t3212_passed_time",
        "uint8 common_access_class",
        "uint8 cs_access_class",
        "uint8 ps_access_class",
        "uint8 paging_for_ppac",
        "uint8 lr_for_ppac",
        "uint8 lr_access_class",
        "uint8 lu_req_mobile_identity",
        "uint8 imsi[9]",
        "uint8 gsm_update_status",
        "uint8 current_rat",
        "bool cell_support_cs",
        "uint8 mm_lu_reject_cause",
        "uint8 lu_reject_mccmnc[3]",
        "uint8 mm_lu_status",
        "uint8 cell_service_capab",
        "uint8 lu_rej_count",
        "uint32 num_of_lu_in_current_plmn",
    };

    // MMRR_EM_PLMN_INFO_STRUCT_INFO
    public static final String[] EM_NWSEL_PLMN_INFO_IND_STRUCT = new String[] {
        "uint8 is_higher_pri_plmn_srch",
        "uint8 hp_timeout_timer",
        "uint8 hp_count_down_timer",
        "uint8 rplmn[6]",
        "uint8 eq_plmn[5][6]",
        "uint8 forbid_plmn[4][6]",
        "uint8 hplmn[6]",
        "uint8 previous_rplmn[6]",
        "uint8 requested_plmn[6]",
        "uint8 plmn_sel_mode",
        "uint8 mnc_length",
        "uint8 multi_plmn_count",
        "em_multi_plmn_struct multi_plmn_id[6]",
    };
    public static final String[] EM_MULTI_PLMN_STRUCT = new String[] {
        "uint8 mcc1",
        "uint8 mcc2",
        "uint8 mcc3",
        "uint8 mnc1",
        "uint8 mnc2",
        "uint8 mnc3",
        "uint8 cell_type",
    };

    // 56
    public static final String[] EM_GMM_INFO_IND_STRUCT = new String[] {
        "uint16 ready_timer",
        "uint16 rau_timer",
        "uint8 ms_state",
        "bool is_rau_timer_running",
        "uint16 rau_timer_passed_time",
        "uint8 attach_req_mobile_identity",
        "uint8 ptmsi[4]",
        "uint8 attach_rej_cause",
        "uint8 rau_rej_cause",
        "uint8 gprs_update_status",
        "uint8 cipher_algo",
        "uint8 attach_type",
        "uint8 gmm_state",
        "uint8 gprs_attach_status",
        "bool tlli_valid",
        "uint32 tlli",
        "bool cell_support_ps",
        "uint8 gmm_attach_status",
        "uint8 attach_rej_mccmnc[3]",
        "uint8 gmm_rau_status",
        "uint8 rau_reject_mccmnc[3]",
        "uint8 serv_rej_detach_status",
        "uint8 serv_rej_detach_cause",
        "uint8 serv_rej_detach_mccmnc[3]",
        "uint8 network_op_mode",
        "uint32 attach_rej_count",
        "uint32 rau_rej_count",
        "uint32 ps_no_service_count",
    };

    // FDD_EM_URR_3G_GENERAL_STATUS_IND
    public static final String[] FDD_EM_URR_3G_GENERAL_STATUS_IND_STRUCT = new String[] {
        "uint8 service_status",
        "uint8 umts_rrc_state",
        "uint16 uarfcn_DL",
        "uint16 psc",
        "uint8 plmn_search_status",
    };

    // FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO
    public static final String[] FDD_EM_CSCE_SERV_CELL_S_STATUS_IND_STRUCT = new String[] {
        "uint8 cellidx",
        "uint16 uarfcn_DL",
        "uint16 psc",
        "bool is_s_criteria_satisfied",
        "int8 qQualmin",
        "int8 qRxlevmin",
        "int32 srxlev",
        "int32 squal",
        "int32 rscp",
        "int32 ec_no",
        "uint16 cycleLength",
        "uint8 quality_measure",
        "uint8 band",
        "int32 rssi",
        "uint32 cell_identity",
        "uint32 csg_id",
        "int8 apbcr_priority",
        "uint8 sprio_search1",
        "uint8 sprio_search2",
        "uint8 threshserv_low",
        "uint8 threshserv_low2",
        "uint8 multi_plmn_count",
        "plmn_struct multi_plmn_id[6]",
        "bool lac_valid",
        "uint16 lac",
        "bool rac_valid",
        "uint8 rac",
        "bool ura_valid",
        "uint8 num_ura_id",
        "meme_umts_ura_identity uraIdentity[8]",
        "uint8 hspa_status",
        "uint8 sib19CellFachMeasInd",
        "uint8 eutraRachReportNumBits",
        "uint8 eutraRachReportStringData",
    };
    public static final String[] PLMN_STRUCT = new String[] {
        "uint8 mcc1",
        "uint8 mcc2",
        "uint8 mcc3",
        "uint8 mnc1",
        "uint8 mnc2",
        "uint8 mnc3",
    };

    // FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO
    public static final String[] FDD_EM_CSCE_NEIGH_CELL_S_STATUS_IND_STRUCT = new String[] {
        "uint8 neigh_cell_count",
        "uint8 operation",
        "uint8 RAT_type",
        "union_fdd_em_csce_neigh_cell choice",
    };
    public static final String[] UNION_FDD_EM_CSCE_NEIGH_CELL = new String[] {
        "FDD_csce_em_neigh_cell_s_status neigh_cells[16]",
        "FDD_csce_em_2G_neigh_cell_s_status GSM_neigh_cells[16]",
        "FDD_csce_em_LTE_neigh_cell_s_status LTE_neigh_cells[16]",
    };
    public static final String[] FDD_CSCE_EM_NEIGH_CELL_S_STATUS = new String[] {
        "uint8 cellidx",
        "uint16 uarfcn_DL",
        "uint16 psc",
        "bool is_s_criteria_satisfied",
        "int8 qQualmin",
        "int8 qRxlevmin",
        "int32 srxlev",
        "int32 squal",
        "int32 rscp",
        "int32 ec_no",
        "int8 apbcr_priority",
        "uint8 threshx_high",
        "uint8 threshx_low",
        "uint8 threshx_high2",
        "uint8 threshx_low2",
        "uint32 cell_identity",
        "bool multi_plmn_valid",
        "uint8 multi_plmn_count",
        "plmn_struct multi_plmn_id[6]",
        "bool lac_valid",
        "uint16 lac",
        "bool rac_valid",
        "uint8 rac",
        "bool ura_valid",
        "uint8 num_ura_id",
        "meme_umts_ura_identity uraIdentity[8]",
    };
    public static final String[] FDD_CSCE_EM_2G_NEIGH_CELL_S_STATUS = new String[] {
        "uint8 cellidx",
        "uint16 arfcn",
        "uint8 bsic",
        "bool is_bsic_verified",
        "bool is_s_criteria_satisfied",
        "uint8 freq_band",
        "int8 qRxlevmin",
        "int32 srxlev",
        "int32 rssi",
        "int8 apbcr_priority",
        "uint8 threshx_high",
        "uint8 threshx_low",
        "uint8 threshx_high2",
        "uint8 threshx_low2",
    };
    public static final String[] FDD_CSCE_EM_LTE_NEIGH_CELL_S_STATUS = new String[] {
        "uint32 earfcn",
        "uint16 pci",
        "int32 rsrp",
        "int32 rsrq",
        "int32 Treselection",
        "int8 apbcr_priority",
        "int16 qRxlevmin",
        "int8 qQualMinEUTRA",
        "uint8 threshx_high",
        "uint8 threshx_low",
        "uint8 threshx_high2",
        "uint8 threshx_low2",
    };

    // TDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO
    public static final String[] TDD_EM_CSCE_SERV_CELL_S_STATUS_IND_STRUCT = new String[] {
        "uint8 cellidx",
        "uint16 uarfcn_DL",
        "uint16 psc",
        "bool is_s_criteria_satisfied",
        "int8 qQualmin",
        "int8 qRxlevmin",
        "int32 srxlev",
        "int32 squal",
        "int32 rscp",
        "uint16 cycleLength",
        "uint8 band",
        "int32 rssi",
        "uint32 cell_identity",
        "uint32 csg_id",
        "int8 apbcr_priority",
        "uint8 sprio_search1",
        "uint8 sprio_search2",
        "uint8 threshserv_low",
        "uint8 threshserv_low2",
    };

    // TDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO
    public static final String[] TDD_EM_CSCE_NEIGH_CELL_S_STATUS_IND_STRUCT = new String[] {
        "uint8 neigh_cell_count",
        "uint8 operation",
        "uint8 RAT_type",
        "union_tdd_em_csce_neigh_cell choice",
    };
    public static final String[] UNION_TDD_EM_CSCE_NEIGH_CELL = new String[] {
        "tdd_csce_em_neigh_cell_s_status neigh_cells[16]",
        "tdd_csce_em_2G_neigh_cell_s_status GSM_neigh_cells[16]",
        "tdd_csce_em_LTE_neigh_cell_s_status LTE_neigh_cells[16]",
    };
    public static final String[] TDD_CSCE_EM_NEIGH_CELL_S_STATUS = new String[] {
        "uint8 cellidx",
        "uint16 uarfcn_DL",
        "uint16 psc",
        "bool is_s_criteria_satisfied",
        "int8 qQualmin",
        "int8 qRxlevmin",
        "int32 srxlev",
        "int32 squal",
        "int32 rscp",
        "int32 ec_no",
        "int8 apbcr_priority",
        "uint8 threshx_high",
        "uint8 threshx_low",
        "uint8 threshx_high2",
        "uint8 threshx_low2",
    };
    public static final String[] TDD_CSCE_EM_2G_NEIGH_CELL_S_STATUS = new String[] {
        "uint8 cellidx",
        "uint16 arfcn",
        "uint8 bsic",
        "bool is_bsic_verified",
        "bool is_s_criteria_satisfied",
        "uint8 freq_band",
        "int8 qRxlevmin",
        "int32 srxlev",
        "int32 rssi",
        "int8 apbcr_priority",
        "uint8 threshx_high",
        "uint8 threshx_low",
        "uint8 threshx_high2",
        "uint8 threshx_low2",
    };
    public static final String[] TDD_CSCE_EM_LTE_NEIGH_CELL_S_STATUS = new String[] {
        "uint16 earfcn",
        "uint16 pci",
        "int32 rsrp",
        "int32 rsrq",
        "int32 Treselection",
        "int8 apbcr_priority",
        "int16 qRxlevmin",
        "int8 qQualMinEUTRA",
        "uint8 threshx_high",
        "uint8 threshx_low",
        "uint8 threshx_high2",
        "uint8 threshx_low2",
    };

    // EM_TDD_MEME_INFO_DCH_UMTS_CELL_INFO
    public static final String[] TDD_EM_MEME_DCH_UMTS_CELL_INFO_IND_STRUCT = new String[] {
        "uint16 workingFreq",
        "uint8 num_cells",
        "uint8 supportBand",
        "uint8 sinr",
        "TDD_meme_umts_cell_struct umts_cell_list[64]",
    };
    public static final String[] TDD_MEME_UMTS_CELL_STRUCT = new String[] {
        "uint16 UARFCN",
        "uint16 CELLPARAID",
        "int32 RSCP",
        "int32 ISCP[6]",
        "bool isServingCell",
    };

    // EM_TDD_MEME_INFO_DCH_GSM_CELL_INFO
    public static final String[] TDD_EM_MEME_DCH_GSM_CELL_INFO_IND_STRUCT = new String[] {
        "uint8 num_cells",
        "meme_gsm_cell_struct gsm_cell_list[6]",
    };
    public static final String[] MEME_GSM_CELL_STRUCT = new String[] {
        "uint8 frequency_band",
        "uint16 arfcn",
        "uint8 bsic",
        "int16 rssi",
        "bool verified",
    };

    // EM_TDD_MEME_INFO_DCH_LTE_CELL_INFO
    public static final String[] TDD_EM_MEME_DCH_LTE_CELL_INFO_IND_STRUCT = new String[] {
        "uint8 num_cells",
        "meme_lte_cell_struct lte_cell_list[32]",
    };
    public static final String[] MEME_LTE_CELL_STRUCT = new String[] {
        "uint32 EARFCN",
        "uint16 PCI",
        "int16 RSRP",
        "int16 RSRQ",
    };

    // EM_TDD_MEME_INFO_EVENT_TYPE_3_PARAMETER_STRUCT_INFO
    public static final String[] TDD_EM_MEME_EVENT_TYPE_3_PARAMETER_INFO_IND_STRUCT = new String[] {
        "uint8 num_event_info",
        "event_3_struct em_event_info[4]",
    };
    public static final String[] EVENT_3_STRUCT = new String[] {
        "uint8 MeasId",
        "uint8 EventType",
        "uint8 MeasQty",
        "uint8 MeasQtyOtherRAT",
        "uint8 fc_OtherSys",
        "uint8 fc_OwnSys",
        "int16 ThresholdOwnSystem",
        "int16 ThresholdOtherSystem",
        "uint8 W",
        "uint8 Hystersis",
        "uint16 TimeToTrigger",
    };

    // EM_TDD_MEME_INFO_REPORT_INFO
    public static final String[] TDD_EM_MEME_REPORT_INFO_IND_STRUCT = new String[] {
        "uint8 MeasId",
        "uint8 EventType",
    };

    // EM_RRCE_KPI_STATUS_IND
    public static final String[] EM_RRCE_KPI_STATUS_IND_STRUCT = new String[] {
        "uint8 proc_id",
        "uint8 status",
        "uint32 attempt",
        "uint32 success",
    };

    // EM_RRCE_CS_OVER_HSPA_STATUS_IND
    public static final String[] EM_RRCE_CS_OVER_HSPA_STATUS_IND_STRUCT = new String[] {
        "bool cs_over_hspa_status",
    };

    // EM_RRCE_FD_CONFIGURATION_STATUS_IND
    public static final String[] EM_RRCE_FD_CONFIGURATION_STATUS_IND_STRUCT = new String[] {
        "bool FDcfgStatus",
    };

    // EM_RRCE_HSPA_CONFIG_IND
    public static final String[] EM_RRCE_HSPA_CONFIG_IND_STRUCT = new String[] {
        "uint8 e_dch_tti",
        "uint8 hsdpa_configured",
        "uint8 hsupa_configured",
        "uint8 dc_hsdpa_configured",
        "uint8 dtx_configured",
        "uint8 drx_configured",
    };

    // UL1_EM_HSPA_INFO_GROUP
    public static final String[] FDD_EM_UL1_HSPA_INFO_GROUP_IND_STRUCT = new String[] {
    "ul1_em_hs_dsch_configuration_struct hs_dsch_configuration",
    "ul1_em_edch_configuration_struct edch_configuration",
    "ul1_em_cpc_configuration_struct cpc_configuration",
    "ul1_em_secondary_hs_dsch_configuration_status_struct secondary_hs_dsch_configuration_status",
    "ul1_em_primary_hs_dsch_bler_struct primary_hs_dsch_bler",
    "ul1_em_secondary_hs_dsch_bler_struct secondary_hs_dsch_bler",
    "ul1_em_edch_ack_rate_struct edch_ack_rate",
    };
    public static final String[] UL1_EM_HS_DSCH_CONFIGURATION_STRUCT = new String[] {
        "bool dl_64QAM_on",
    };
    public static final String[] UL1_EM_EDCH_CONFIGURATION_STRUCT = new String[] {
        "bool ul_16QAM_on",
        "uint8 edch_tti",
    };
    public static final String[] UL1_EM_CPC_CONFIGURATION_STRUCT = new String[] {
        "bool dtx_on",
        "bool drx_on",
    };
    public static final String[] UL1_EM_SECONDARY_HS_DSCH_CONFIGURATION_STATUS_STRUCT=new String[]{
        "bool dc_on",
        "uint16 dl_freq",
        "uint16 psc",
        "bool dl_64QAM_on",
    };
    public static final String[] UL1_EM_PRIMARY_HS_DSCH_BLER_STRUCT = new String[] {
        "uint16 dsch_error_all",
        "uint16 dsch_error_new_data",
        "uint16 dsch_error_after_retx",
        "uint8 dsch_curr_mod",
        "uint16 dsch_code_num",
        "uint16 dsch_cqi_val",
        "uint16 dsch_gap_ratio",
    };
    public static final String[] UL1_EM_SECONDARY_HS_DSCH_BLER_STRUCT = new String[] {
        "uint16 dsch_error_all",
        "uint16 dsch_error_new_data",
        "uint16 dsch_error_after_retx",
        "uint8 dsch_curr_mod",
        "uint16 dsch_code_num",
        "uint16 dsch_cqi_val",
        "uint16 dsch_gap_ratio",
    };
    public static final String[] UL1_EM_EDCH_ACK_RATE_STRUCT = new String[] {
        "uint16 edch_ack_cnt",
        "uint16 edch_gap_ratio",
    };

    // ERRC_EM_MOB_MEAS_INTRARAT_INFO
    public static final String[] EM_ERRC_MOB_MEAS_INTRARAT_INFO_IND_STRUCT = new String[] {
        "errc_mob_em_serving_info_struct serving_info",
        "errc_mob_em_intrarat_intra_info_struct intra_info",
        "errc_mob_em_intrarat_inter_info_struct inter_info",
        "errc_mob_em_scell_info_list_struct scell_info_list",
    };
    public static final String[] ERRC_MOB_EM_SERVING_INFO_STRUCT = new String[] {
        "uint32 earfcn",
        "uint16 pci",
        "int32 rsrp",
        "int32 rsrq",
        "int32 rs_snr_in_qdb",
        "uint16 serv_lte_band",
        "uint8 mobility_state",
        "uint8 S_intra_search_p",
        "uint8 S_intra_search_q",
        "uint8 S_nonintra_search_p",
        "uint8 S_nonintra_search_q",
        "uint8 thresh_serving_low_p",
        "uint8 thresh_serving_low_q",
        "uint8 tresel",
        "uint8 DlBandwidth",
        "uint8 UlBandwidth",
    };
    public static final String[] ERRC_MOB_EM_INTRARAT_INTRA_INFO_STRUCT = new String[] {
        "int8 priority",
        "uint8 bandwidth",
        "bool is_blacklist_present",
        "uint16 pcomp",
        "uint8 cell_num",
        "errc_mob_em_intrarat_intra_info_ecell_struct intra_cell[16]",
    };
    public static final String[] ERRC_MOB_EM_INTRARAT_INTRA_INFO_ECELL_STRUCT = new String[] {
        "bool valid",
        "uint16 pci",
        "int32 rsrp",
        "int32 rsrq",
    };
    public static final String[] ERRC_MOB_EM_INTRARAT_INTER_INFO_STRUCT = new String[] {
        "uint8 freq_num",
        "errc_mob_em_intrarat_inter_info_inter_freq_struct inter_freq[4]",
    };
    public static final String[] ERRC_MOB_EM_INTRARAT_INTER_INFO_INTER_FREQ_STRUCT = new String[] {
        "bool valid",
        "uint32 earfcn",
        "uint16 pcomp",
        "int8 priority",
        "uint8 bandwidth",
        "bool is_blacklist_present",
        "uint8 treselection",
        "uint8 thresh_x_high_p",
        "uint8 thresh_x_high_q",
        "uint8 thresh_x_low_p",
        "uint8 thresh_x_low_q",
        "uint8 cell_num",
        "errc_mob_em_intrarat_inter_info_ecell_struct inter_cell[6]",
    };
    public static final String[] ERRC_MOB_EM_INTRARAT_INTER_INFO_ECELL_STRUCT = new String[] {
        "bool valid",
        "uint16 pci",
        "int32 rsrp",
        "int32 rsrq",
    };
    public static final String[] ERRC_MOB_EM_SCELL_INFO_LIST_STRUCT = new String[] {
        "uint8 num_scell",
        "errc_mob_em_scell_info_struct scell_info[7]",
    };
    public static final String[] ERRC_MOB_EM_SCELL_INFO_STRUCT = new String[] {
        "uint32 earfcn",
        "uint16 pci",
        "int32 rsrp",
        "int32 rsrq",
        "int32 rs_snr_in_qdb",
        "uint16 serv_lte_band",
    };

    // ERRC_EM_MOB_MEAS_INTERRAT_UTRAN_INFO
    public static final String[] EM_ERRC_MOB_MEAS_INTERRAT_UTRAN_INFO_IND_STRUCT = new String[] {
        "uint8 freq_num",
        "errc_mob_em_ir_utran_freq_struct inter_freq[16]",
    };
    public static final String[] ERRC_MOB_EM_IR_UTRAN_FREQ_STRUCT = new String[] {
        "bool valid",
        "uint16 uarfcn",
        "int8 priority",
        "uint8 threshx_high_p",
        "uint8 threshx_high_q",
        "uint8 threshx_low_p",
        "uint8 threshx_low_q",
        "uint8 ucell_num",
        "errc_mob_em_ir_utran_ucell_struct ucell[6]",
    };
    public static final String[] ERRC_MOB_EM_IR_UTRAN_UCELL_STRUCT = new String[] {
        "bool valid",
        "uint16 psc",
        "int32 rscp",
        "int32 ec_n0",
    };

    // ERRC_EM_MOB_MEAS_INTERRAT_GERAN_INFO
    public static final String[] EM_ERRC_MOB_MEAS_INTERRAT_GERAN_INFO_IND_STRUCT = new String[] {
        "uint8 total_gcell_num",
        "errc_mob_em_ir_geran_gcell_struct gcell[6]",
    };
    public static final String[] ERRC_MOB_EM_IR_GERAN_GCELL_STRUCT = new String[] {
        "bool valid",
        "int8 priority",
        "bool band_ind",
        "uint16 arfcn",
        "uint8 bsic",
        "int32 rssi",
        "uint8 thresh_x_high",
        "uint8 thresh_x_low",
    };

    // ERRC_EM_ERRC_STATE_IND
    public static final String[] EM_ERRC_STATE_IND_STRUCT = new String[] {
        "uint8 errc_sts",
    };

    // ERRC_EM_ERRC_KPI_INFO
    public static final String[] EM_ERRC_SUCCESS_RATE_KPI_IND_STRUCT = new String[] {
        "uint8 proc_id",
        "uint8 status",
        "uint32 attempt",
        "uint32 success",
    };

    // ERRC_EM_CONN_INFO
    public static final String[] EM_ERRC_CONN_INFO_IND_STRUCT = new String[] {
        "uint32 pcell_earfcn",
        "uint16 pcell_band",
        "uint16 pcell_pci",
        "uint8 pcell_bw",
        "uint8 unused[3]",
        "bool is_scell_configured[4]",
        "uint32 scell_earfcn[4]",
        "uint16 scell_band[4]",
        "uint16 scell_pci[4]",
        "uint8 scell_bw[4]",
    };

    // EMM_L4C_EMM_INFO
    public static final String[] EM_EMM_L4C_EMM_INFO_IND_STRUCT = new String[] {
        "l4c_em_emm_call_para_struct emm_call_para",
        "l4c_em_emm_conn_para_struct emm_conn_para",
        "l4c_em_emm_reg_para_struct emm_reg_para",
        "l4c_em_emm_plmnsel_para_struct emm_plmnsel_para",
        "l4c_em_emm_usimsrv_para_struct emm_usimsrv_para",
        "l4c_em_emm_nvmsrv_para_struct emm_nvmsrv_para",
        "l4c_em_emm_sec_para_struct emm_sec_para",
        "l4c_em_emm_timersrv_para_struct emm_timersrv_para",
    };
    public static final String[] L4C_EM_EMM_CALL_PARA_STRUCT = new String[] {
        "uint8 service_request_type",
        "uint8 service_request_cause",
        "uint8 mo_csfb_cause",
        "uint8 mt_csfb_paging_id",
    };
    public static final String[] L4C_EM_EMM_CONN_PARA_STRUCT = new String[] {
        "uint8 ecm_status",
    };
    public static final String[] L4C_EM_EMM_REG_PARA_STRUCT = new String[] {
        "uint8 eps_attach_type",
        "uint8 attach_additional_update_type",
        "uint8 eps_attach_result",
        "uint8 attach_additional_update_result",
        "uint8 attach_emm_cause",
        "uint32 attach_attempt_count",
        "uint8 attach_status",
        "uint8 tau_req_update_type",
        "uint8 tau_additional_update_type",
        "uint8 tau_update_result",
        "uint8 tau_additional_update_result",
        "uint8 tau_emm_cause",
        "uint32 tau_attempt_count",
        "uint8 tau_status",
        "uint8 user_detach_type",
        "uint8 nw_detach_type",
        "uint8 nw_detach_emm_cause",
        "uint32 detach_attempt_count",
        "uint8 ims_service_ind",
        "uint8 emergency_service_ind",
        "em_emm_lai_struct lai",
    };
    public static final String[] L4C_EM_EMM_PLMNSEL_PARA_STRUCT = new String[] {
        "plmn_struct selected_plmn",
        "uint16 tac",
        "uint8 csg_access_mode",
        "uint32 csg_id",
        "uint8 duplex_type",
//        "uint32 cell_id",
    };
    public static final String[] L4C_EM_EMM_USIMSRV_PARA_STRUCT = new String[] {
        "uint8 update_status",
        "uint8 cs_sim_status",
        "uint8 ps_sim_status",
        "em_emm_guti_struct guti",
    };
    public static final String[] L4C_EM_EMM_NVMSRV_PARA_STRUCT = new String[] {
        "uint8 tin",
    };
    public static final String[] L4C_EM_EMM_SEC_PARA_STRUCT = new String[] {
        "uint8 integrity_alg",
        "uint8 ciphering_alg",
    };
    public static final String[] L4C_EM_EMM_TIMERSRV_PARA_STRUCT = new String[] {
        "uint16 t3402_remaining_time",
    };
    public static final String[] EM_EMM_LAI_STRUCT = new String[] {
        "plmn_struct plmn_id",
        "uint8 lac[2]",
    };
    public static final String[] EM_EMM_GUTI_STRUCT = new String[] {
        "plmn_struct plmn_id",
        "uint8 mme_gid[2]",
        "uint8 mme_code",
        "uint8 mtmsi[4]",
    };

    // EM_EL1_INFO
    public static final String[] EM_EL1_STATUS_IND_STRUCT = new String[] {
        "em_el1_cell_info_struct cell_info[2]",
        "em_el1_dl_status_struct dl_info[2]",
        "em_el1_ul_status_struct ul_info[1]",
    };
    public static final String[] EM_EL1_CELL_INFO_STRUCT = new String[] {
        "uint8 band",
        "uint8 ant_port",
        "uint8 dl_bw",
        "uint8 ul_bw",
        "uint8 tdd_cfg",
        "uint8 sp_cfg",
        "uint8 tm",
        "int8 ul_cc_idx",
        "int16 pci",
        "uint32 earfcn",
        "uint16 dlFreq",
        "uint16 ulFreq",
    };
    public static final String[] EM_EL1_DL_STATUS_STRUCT = new String[] {
        "int16 dl_rssi[2]",
        "int16 dl_rsrp[2]",
        "int16 dl_rsrq[2]",
        "int16 dl_sinr[2]",
        "int16 rsrp",
        "int16 rsrq",
        "int16 sinr",
        "int16 rsSNR",
        "int16 tm",
        "int16 cqi_cw0",
        "int16 cqi_cw1",
        "int16 ri",
        "uint32 DL_Tput",
        "int16 DL_Imcs",
        "int16 DL_rb",
        "int16 DL_block",
        "int16 DL_bler",
        "uint32 MCH_Tput",
        "int16 MCH_block",
        "int16 MCH_bler",
    };
    public static final String[] EM_EL1_UL_STATUS_STRUCT = new String[] {
        "int16 prach_tx_power_ave",
        "int16 pucch_tx_power_ave",
        "int16 pusch_tx_power_ave",
        "int16 srs_tx_power_ave",
        "int16 tm",
        "int16 phr",
        "int16 ta",
        "uint32 UL_Tput",
        "int16 UL_Imcs",
        "int16 UL_rb",
        "int16 UL_block",
        "int16 UL_bler",
    };

    // EM_CSR_STATUS_IND
    public static final String[] EM_CSR_STATUS_IND_STRUCT = new String[] {
        "uint8 ULAMRType",
        "uint8 DLAMRType",
    };

    // RAC_EM_INFO
    public static final String[] EM_RAC_INFO_IND_STRUCT = new String[] {
        "uint8 ue_mode",
        "bool is_sms_over_ip",
        "uint8 ue_usage_setting",
        "uint8 eutran_vdp",
        "uint8 active_rat_info",
    };

    // ERRC_EM_MOB_MEAS_CONFIG_INFO_IND
    public static final String[] EM_ERRC_MOB_MEAS_CONFIG_INFO_IND_STRUCT = new String[] {
        "uint8 num_event_info",
        "event_B_struct em_event_info[2]",
    };
    public static final String[] EVENT_B_STRUCT = new String[] {
        "uint8 MeasId",
        "uint8 EventType",
        "uint8 MeasQty",
        "uint8 MeasQtyOtherRAT",
        "uint8 fc_OtherSys",
        "uint8 fc_OwnSys",
        "int16 ThresholdOwnSystem",
        "int16 ThresholdOtherSystem",
        "uint8 Hystersis",
        "uint16 TimeToTrigger",
    };

    // ERRC_EM_MOB_MEAS_REPORT_INFO_IND
    public static final String[] EM_ERRC_MOB_MEAS_REPORT_INFO_IND_STRUCT = new String[] {
        "uint8 MeasId",
        "uint8 EventType",
    };

    // EM_UPCM_PS_TPUT_INFO
    public static final String[] EM_UPCM_PS_TPUT_INFO_IND_STRUCT = new String[] {
        "uint32 total_rx_byte_per_second",
        "uint32 total_tx_byte_per_second",
    };

    // USIME_EM_INFO_CAPABILITY
    public static final String[] FDD_EM_USIME_CAPABILITY_INFO_IND_STRUCT = new String[] {
        "uint8 hsdpa_enable",
        "uint8 hsupa_enable",
    };

    // EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO
    public static final String[] FDD_EM_MEME_DCH_UMTS_CELL_INFO_IND_STRUCT = new String[] {
        "int8 tx_power",
        "uint8 num_cells",
        "FDD_meme_umts_cell_struct umts_cell_list[32]",
    };
    public static final String[] FDD_MEME_UMTS_CELL_STRUCT = new String[] {
        "uint16 UARFCN",
        "uint16 PSC",
        "int32 RSCP",
        "int32 ECN0",
        "uint8 cell_type",
        "uint8 Band",
        "int32 RSSI",
        "uint32 Cell_identity",
        "uint8 validity",
        "uint8 num_plmn_id",
        "uas_cell_plmn_id plmn_id_list[6]",
        "uint16 lac",
        "uint8 rac",
        "uint8 num_ura_id",
        "meme_umts_ura_identity uraIdentity[8]",
    };
    public static final String[] UAS_CELL_PLMN_ID = new String[] {
        "uint16 mcc",
        "uint16 mnc",
    };
    public static final String[] MEME_UMTS_URA_IDENTITY = new String[] {
        "uint8 numBits",
        "uint8 stringData[2]",
    };

    // EM_FDD_MEME_INFO_DCH_H_SERVING_CELL_INFO
    public static final String[] FDD_EM_MEME_DCH_H_SERVING_CELL_INFO_IND_STRUCT = new String[] {
        "uint16 HSDSCH_Serving_UARFCN",
        "uint16 HSDSCH_Serving_PSC",
        "uint16 EDCH_Serving_UARFCN",
        "uint16 EDCH_Serving_PSC",
    };

    public static final String[] UL1_EM_TAS_INFO_STRUCT = new String[] {
        "uint16 band",
        "uint16 uarfcn",
        "uint16 serving_psc",
        "int16 rscp_max",
        "int16 rscp_0",
        "int16 rscp_1",
        "int16 rscp_diff",
        "int16 ecno_max",
        "int16 ecno_0",
        "int16 ecno_1",
        "int16 rssi_max",
        "int16 rssi_0",
        "int16 rssi_1",
        "int8 tx_pwr",
        "int8 dpcch_tx_pwr",
        "uint8 main_ant_idx",
    };

    // WCDMA TAS INFO
    public static final String[] EM_UL1_TAS_INFO_IND_STRUCT = new String[] {
        "ul1_em_tas_info_struct EmUl1Tas",
    };

    // GSM TAS INFO
    public static final String[] EM_GSM_TAS_INFO_IND_STRUCT = new String[] {
        "uint8 gsm_antenna",
        "uint16 gsm_current_antenna_rxLevel",
        "uint16 gsm_other_antenna_rxLevel",
    };

    public static final Object[][] MAPPING = new Object[][]  {
        {RR_EM_MEASUREMENT_REPORT_INFO, "rr_em_measurement_report_info_struct"},
        {RRM_EM_IR_3G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO,
         "em_rrm_ir_3g_neighbor_meas_status_ind_struct"},
        {RRM_EM_IR_4G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO,
         "em_rrm_ir_4g_neighbor_meas_status_ind_struct"},
//        {19, "EM_RRM_IR_4G_NEIGHBOR_MEAS_STATUS_IND_STRUCT"},
        {RRM_EM_SUCCESS_RATE_KPI_INFO, "em_rrm_success_rate_kpi_ind_struct"},
        {MM_EM_INFO, "em_mm_info_ind_struct"},
        {MMRR_EM_PLMN_INFO_STRUCT_INFO, "em_nwsel_plmn_info_ind_struct"},
        {GMM_EM_INFO, "em_gmm_info_ind_struct"},
        {FDD_EM_URR_3G_GENERAL_STATUS_IND, "fdd_em_urr_3g_general_status_ind_struct"},
        {FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO, "fdd_em_csce_serv_cell_s_status_ind_struct"},
        {FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO, "fdd_em_csce_neigh_cell_s_status_ind_struct"},
        {TDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO, "tdd_em_csce_serv_cell_s_status_ind_struct"},
        {TDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO, "tdd_em_csce_neigh_cell_s_status_ind_struct"},
        {EM_TDD_MEME_INFO_DCH_UMTS_CELL_INFO, "tdd_em_meme_dch_umts_cell_info_ind_struct"},
        {EM_TDD_MEME_INFO_DCH_GSM_CELL_INFO, "tdd_em_meme_dch_gsm_cell_info_ind_struct"},
        {EM_TDD_MEME_INFO_DCH_LTE_CELL_INFO, "tdd_em_meme_dch_lte_cell_info_ind_struct"},
        {EM_TDD_MEME_INFO_EVENT_TYPE_3_PARAMETER_STRUCT_INFO,
         "tdd_em_meme_event_type_3_parameter_info_ind_struct"},
        {EM_TDD_MEME_INFO_REPORT_INFO, "tdd_em_meme_report_info_ind_struct"},
        {EM_RRCE_KPI_STATUS_IND, "em_rrce_kpi_status_ind_struct"},
        {EM_RRCE_CS_OVER_HSPA_STATUS_IND, "em_rrce_cs_over_hspa_status_ind_struct"},
        {EM_RRCE_FD_CONFIGURATION_STATUS_IND, "em_rrce_fd_configuration_status_ind_struct"},
        {EM_RRCE_HSPA_CONFIG_IND, "em_rrce_hspa_config_ind_struct"},
        {UL1_EM_HSPA_INFO_GROUP, "fdd_em_ul1_hspa_info_group_ind_struct"},

        {ERRC_EM_MOB_MEAS_INTRARAT_INFO, "em_errc_mob_meas_intrarat_info_ind_struct"},
        {ERRC_EM_MOB_MEAS_INTERRAT_UTRAN_INFO, "em_errc_mob_meas_interrat_utran_info_ind_struct"},
        {ERRC_EM_MOB_MEAS_INTERRAT_GERAN_INFO, "em_errc_mob_meas_interrat_geran_info_ind_struct"},

        {ERRC_EM_ERRC_STATE_IND, "em_errc_state_ind_struct"},
        {ERRC_EM_ERRC_KPI_INFO, "em_errc_success_rate_kpi_ind_struct"},
        {ERRC_EM_CONN_INFO, "em_errc_conn_info_ind_struct"},
        {EMM_L4C_EMM_INFO, "em_emm_l4c_emm_info_ind_struct"},
        {EM_EL1_INFO, "em_el1_status_ind_struct"},
        {EM_CSR_STATUS_IND, "em_csr_status_ind_struct"},
        {RAC_EM_INFO, "em_rac_info_ind_struct"},
        {ERRC_EM_MOB_MEAS_CONFIG_INFO_IND, "em_errc_mob_meas_config_info_ind_struct"},
        {ERRC_EM_MOB_MEAS_REPORT_INFO_IND, "em_errc_mob_meas_report_info_ind_struct"},
        {EM_UPCM_PS_TPUT_INFO, "em_upcm_ps_tput_info_ind_struct"},
        {USIME_EM_INFO_CAPABILITY, "fdd_em_usime_capability_info_ind_struct"},
        {EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO, "fdd_em_meme_dch_umts_cell_info_ind_struct"},
        {EM_FDD_MEME_INFO_DCH_H_SERVING_CELL_INFO,
         "fdd_em_meme_dch_h_serving_cell_info_ind_struct"},
        {EM_UL1_TAS_INFO_IND, "em_ul1_tas_info_ind_struct"},
        {EM_GSM_TAS_INFO_IND, "em_gsm_tas_info_ind_struct"},
    };

    public static String[] findStructDefinition(String structName) {
        try {
            Field f = Content.class.getField(structName.toUpperCase(Locale.ENGLISH));
            return (String[]) f.get(null);
        } catch (NoSuchFieldException e) {
            Elog.e(TAG, "NoSuchFieldException " + structName);
            Elog.e(TAG, e.toString());
            throw new RuntimeException("NoSuchFieldException");
        } catch (IllegalArgumentException e) {
            Elog.e(TAG, "IllegalArgumentException " + structName);
            Elog.e(TAG, e.toString());
            throw new RuntimeException("IllegalArgumentException");
        } catch (IllegalAccessException e) {
            Elog.e(TAG, "IllegalAccessException " + structName);
            Elog.e(TAG, e.toString());
            throw new RuntimeException("IllegalAccessException");
        }
    }
}
//[ ]+kal_\(.*\)[ ]+\(.*\);.*
//        {"\1", "\2"},
