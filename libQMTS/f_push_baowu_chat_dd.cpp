/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      wsl
Version:     1.0
Date:        2023-10-31
Description: 推送文字宝武聊天
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


#include "epex.h"
#include <list>

void f_call_ijudge_svc(CDbConnection* conn, const CString& system_code, const CString& envType, const CString& apiCode, const CString& svc_name, EIClass* blks_in, EIClass* blks_out, bool traceJson);
int f_qmts_call_judge(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息规则引擎
BM2_FUNCTION_EXPORT


int f_push_baowu_chat_dd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	// 创建电文处理对象
	EPEX epex;

	/* 实体类定义 */


	CString sqlstr("");

	/*宝物聊天相关字段*/
	CString message = " ";//邮件消息体
	CString appId = "sids"; //应用ID
	CString msgType = "text";//推送企业微信消息类型
	CString showType = "text";//消息类型
	CString sendMode = "1";//模式
	CString secretCode = "cewe";//密钥
	CString msgCategory = "03";//消息类型-待办
	CString isSend = "1";//填1
	CString configNum = "";//填空
	CString title = "msgTitle";

	CString st_no = "";


	CString c_receive_weight = "";//碳钢当日产量
	CString s_receive_weight = "";//不锈钢当日产量
	CString date = "";//日期
	CString zo_count = "";//总量
	CString c_receive_weight_lj = "";//碳钢累计产量
	CString s_receive_weight_lj = "";//不锈钢累计产量
	CString zo_count_lj = "";//总量累计
	int id_count = 0;

	CString username = " ";
	CString code = " ";//代码
	CString MODULE_NAME = " ";//模块名
	CString LOGINNAME = " ";//责任人
	CString LOGINNAME_LIST = " ";//消息通知人
	CString MESSAGE_TITLE = " ";//消息标题
	CString MESSAGE_LIST = " ";//消息内容
	CString KZABSCHL = " ";//推送开关
	CDbCommand cmd_inq(conn);
	CDbCommand cmdws_inq(conn);



	CString remark = "";
	//库存
	CString mat_wt = "";
	CString mat_wt_wt = "";
	CString count_huizo = "";


	CString SEND_FLAG = "";
	CString Messagew_list = "";

	CString raq_mater_inve = "";


	//成品水爆坯检查结果 
	CString batch = "";
	CString sg_sign = "";
	CString unit_code = "";
	CString mat_width = "";
	CString deal_notion = "";
	CString grind_main_reason = "";
	CString casting_pre_judgment = " ";


	//成分不合封锁信息
	CString judge_code = "";
	CString haet_no = "";
	CString st_sample_no = "";
	CString remak_cf = "";

	//表面质量封锁
	CString surf_quality = "";

	//修磨量
	CString mend_afer_wt = "";
	CString count_ks = "";

	//工艺卡违规
	CString heat_no = "";
	CString	gpoup = "";
	CString	desc = "";
	CString	flag_mc = "";
	CString un_flag = "";
	CString	dlag_id = "";
	CString	wt_desc = "";
	CString	err_desc = "";
	CString	examiner = "";
	CString jc_rq_time = "";

	//镍板库405 
	CString remark_NICKEL = "";
	//火车库402
	CString remark_HC = "";
	//铬库403
	CString remark_LK = "";
	//废钢料场
	CString remark_FG = "";

	//每班推送
	CString prod_shift_no = " ";
	CString tap_date = " ";
	CString bof_number = " ";
	CString	dep_number_bof = " ";
	CString aod_number = " ";
	CString	number_des_aod = " ";
	CString	number_bof_aod = " ";
	CString	number_eaf_aod = " ";
	CString	number_high_si_aod = " ";
	CString	eaf_number = " ";
	CString	if_number = " ";
	CString	des_number = " ";
	CDecimal	s_jk_sum = 0;
	CDecimal	s_jk_count = 0;
	CDecimal	c_jk_sum = 0;
	CDecimal	c_jk_count = 0;
	CDecimal	s_xm_sum = 0;
	CDecimal s_xm_count = 0;
	CDecimal	c_xm_sum = 0;
	CDecimal	c_xm_count = 0;
	CString bofcnumber = " ";
	CString sinumber = " ";
	CString rhnumber = " ";
	CString fenumber = " ";
	CString bofsnumber = " ";
	CString moldnumber = " ";
	CDecimal ccmcnumber = 0;
	CDecimal ccmsnumber = 0;
	CDecimal sumcou = 0;
	CDecimal sumwt = 0;
	CDecimal jkcount = 0;
	CDecimal jksum = 0;
	CDecimal cjkcount = 0;
	CDecimal sjksum = 0;
	CDecimal sjkcount = 0;
	CDecimal cjksum = 0;
	CDecimal sjkcar = 0;
	CDecimal cjkcar = 0;
	CDecimal xiumk = 0;
	CDecimal xiumdun = 0;
	CDecimal zplyk = 0;
	CDecimal zplydun = 0;
	CDecimal clcnumber = 0;
	CDecimal clsnumber = 0;
	//每日推送
	/*CString bof_number = " ";
	CString dep_number_bof = " ";*/
	CString si_aver_bof = " ";
	CString hmtemp_aver_bof = " ";
	/*CString aod_number = " ";
	CString number_des_aod = " ";
	CString number_bof_aod = " ";
	CString number_eaf_aod = " ";
	CString number_high_si_aod = " ";
	CString eaf_number = " ";
	CString if_number = " ";
	CString des_number = " ";
	CString s_jk_sum = " ";
	CString s_jk_count = " ";
	CString c_jk_sum = " ";
	CString c_jk_count = " ";
	CString s_xm_sum = " ";
	CString s_xm_count = " ";
	CString c_xm_sum = " ";
	CString c_xm_count = " ";*/
	CString act_amount = " ";
	CString yg_accout_wt = " ";
	CString south_accout_wt = " ";
	CString north_accout_wt = " ";
	CString steel_amount = " ";
	CString xm_wait_weight_north = " ";
	CString xm_wait_count_north = " ";
	CString l2n_fg_wt = " ";
	CDecimal zxm = 0;

	//党员推送
	CString content = " ";
	CString topic = " ";




	CModel tqmtsbwu_h("TQMTSBWU_H");
	CModel tqmtsbwu("TQMTSBWU");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//月日
	CString date_ry = CDateTime::Now().ToString("MM-dd");
	//分钟
	CString date_fz = CDateTime::Now().ToString("HH:mm:ss");

	try
	{
		//工艺卡下发
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		Log::Trace("", "", "st_no={0}", st_no);


		//产量
		if (bcls_rec->Tables[0].Columns.Contains("C_RECEIVE_WEIGHT"))
			c_receive_weight = bcls_rec->Tables[0].Rows[0]["C_RECEIVE_WEIGHT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("S_RECEIVE_WEIGHT"))
			s_receive_weight = bcls_rec->Tables[0].Rows[0]["S_RECEIVE_WEIGHT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("RECV_TIME"))
			date = bcls_rec->Tables[0].Rows[0]["RECV_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ZO_COUNT"))
			zo_count = bcls_rec->Tables[0].Rows[0]["ZO_COUNT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("C_RECEIVE_WEIGHT_LJ"))
			c_receive_weight_lj = bcls_rec->Tables[0].Rows[0]["C_RECEIVE_WEIGHT_LJ"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("S_RECEIVE_WEIGHT_LJ"))
			s_receive_weight_lj = bcls_rec->Tables[0].Rows[0]["S_RECEIVE_WEIGHT_LJ"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ZO_COUNT_LJ"))
			zo_count_lj = bcls_rec->Tables[0].Rows[0]["ZO_COUNT_LJ"].ToString().Trim();


		Log::Trace("", "", "c_receive_weight={0}", c_receive_weight);


		//跟各个业务唯一对应字段
		if (bcls_rec->Tables[0].Columns.Contains("REMARK"))
			remark = bcls_rec->Tables[0].Rows[0]["REMARK"].ToString().Trim();


		//铸坯库存
		if (bcls_rec->Tables[0].Columns.Contains("MAT_WT"))
			mat_wt = bcls_rec->Tables[0].Rows[0]["MAT_WT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_WT_WT"))
			mat_wt_wt = bcls_rec->Tables[0].Rows[0]["MAT_WT_WT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DATE_TIME"))
			date = bcls_rec->Tables[0].Rows[0]["DATE_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("COUNT_HUIZO"))
			count_huizo = bcls_rec->Tables[0].Rows[0]["COUNT_HUIZO"].ToString().Trim();


		//原料库存报警 
		if (bcls_rec->Tables[0].Columns.Contains("RAQ_MATER_INVE"))
			raq_mater_inve = bcls_rec->Tables[0].Rows[0]["RAQ_MATER_INVE"].ToString().Trim();

		//成品水爆坯检查结果
		if (bcls_rec->Tables[0].Columns.Contains("BATCH"))
			batch = bcls_rec->Tables[0].Rows[0]["BATCH"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SG_SIGN"))
			sg_sign = bcls_rec->Tables[0].Rows[0]["SG_SIGN"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("UNIT_CODE"))
			unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_WIDTH"))
			mat_width = bcls_rec->Tables[0].Rows[0]["MAT_WIDTH"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DEAL_NOTION"))
			deal_notion = bcls_rec->Tables[0].Rows[0]["DEAL_NOTION"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("GRIND_MAIN_REASON"))
			grind_main_reason = bcls_rec->Tables[0].Rows[0]["GRIND_MAIN_REASON"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("CASTING_PRE_JUDGMENT"))
			casting_pre_judgment = bcls_rec->Tables[0].Rows[0]["CASTING_PRE_JUDGMENT"].ToString().Trim();


		//修磨量
		if (bcls_rec->Tables[0].Columns.Contains("MEND_AFTER_WEIGHT"))
			mend_afer_wt = bcls_rec->Tables[0].Rows[0]["MEND_AFTER_WEIGHT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("COUNT_KS"))
			count_ks = bcls_rec->Tables[0].Rows[0]["COUNT_KS"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DATE_TIME_TO"))
			date = bcls_rec->Tables[0].Rows[0]["DATE_TIME_TO"].ToString().Trim();


		//表面质量封锁
		if (bcls_rec->Tables[0].Columns.Contains("SURF_QUALITY"))
			surf_quality = bcls_rec->Tables[0].Rows[0]["SURF_QUALITY"].ToString().Trim();


		//成分不合封锁信息
		if (bcls_rec->Tables[0].Columns.Contains("REMARK_CF"))
			remak_cf = bcls_rec->Tables[0].Rows[0]["REMARK_CF"].ToString().Trim();


		//工艺卡违规
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SHIFT_GROUP_BZ"))
			gpoup = bcls_rec->Tables[0].Rows[0]["SHIFT_GROUP_BZ"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SAP_ZRDW_DESC"))
			desc = bcls_rec->Tables[0].Rows[0]["SAP_ZRDW_DESC"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("UNRULL_FLAG_XM"))
			flag_mc = bcls_rec->Tables[0].Rows[0]["UNRULL_FLAG_XM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("UNRULL_FLAG_LX"))
			un_flag = bcls_rec->Tables[0].Rows[0]["UNRULL_FLAG_LX"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("UNRULL_FLAG_ID"))
			dlag_id = bcls_rec->Tables[0].Rows[0]["UNRULL_FLAG_ID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TRACE_WT_1_DESC"))
			wt_desc = bcls_rec->Tables[0].Rows[0]["TRACE_WT_1_DESC"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ERR_DESC"))
			err_desc = bcls_rec->Tables[0].Rows[0]["ERR_DESC"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("EXAMINER"))
			examiner = bcls_rec->Tables[0].Rows[0]["EXAMINER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("JC_RQ_TIME"))
			jc_rq_time = bcls_rec->Tables[0].Rows[0]["JC_RQ_TIME"].ToString().Trim();


		//镍板库405
		if (bcls_rec->Tables[0].Columns.Contains("REMARK_NICKEL"))
			remark_NICKEL = bcls_rec->Tables[0].Rows[0]["REMARK_NICKEL"].ToString().Trim();

		//火车库402
		if (bcls_rec->Tables[0].Columns.Contains("REMARK_HC"))
			remark_HC = bcls_rec->Tables[0].Rows[0]["REMARK_HC"].ToString().Trim();

		//铬库403
		if (bcls_rec->Tables[0].Columns.Contains("REMARK_LK"))
			remark_LK = bcls_rec->Tables[0].Rows[0]["REMARK_LK"].ToString().Trim();

		//废钢料场
		if (bcls_rec->Tables[0].Columns.Contains("REMARK_FG"))
			remark_FG = bcls_rec->Tables[0].Rows[0]["REMARK_FG"].ToString().Trim();

		//每班推送
		if (bcls_rec->Tables[0].Columns.Contains("TAP_DATE"))
			tap_date = bcls_rec->Tables[0].Rows[0]["TAP_DATE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_NO"))
			prod_shift_no = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("BOF_NUMBER"))
			bof_number = bcls_rec->Tables[0].Rows[0]["BOF_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DEP_NUMBER_BOF"))
			dep_number_bof = bcls_rec->Tables[0].Rows[0]["DEP_NUMBER_BOF"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("AOD_NUMBER"))
			aod_number = bcls_rec->Tables[0].Rows[0]["AOD_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_DES_AOD"))
			number_des_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_DES_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_BOF_AOD"))
			number_bof_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_BOF_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_EAF_AOD"))
			number_eaf_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_EAF_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_HIGH_SI_AOD"))
			number_high_si_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_HIGH_SI_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("EAF_NUMBER"))
			eaf_number = bcls_rec->Tables[0].Rows[0]["EAF_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("IF_NUMBER"))
			if_number = bcls_rec->Tables[0].Rows[0]["IF_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DES_NUMBER"))
			des_number = bcls_rec->Tables[0].Rows[0]["DES_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("S_JK_SUM"))
			s_jk_sum = bcls_rec->Tables[0].Rows[0]["S_JK_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("S_JK_COUNT"))
			s_jk_count = bcls_rec->Tables[0].Rows[0]["S_JK_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_JK_SUM"))
			c_jk_sum = bcls_rec->Tables[0].Rows[0]["C_JK_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_JK_COUNT"))
			c_jk_count = bcls_rec->Tables[0].Rows[0]["C_JK_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("S_XM_SUM"))
			s_xm_sum = bcls_rec->Tables[0].Rows[0]["S_XM_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("S_XM_COUNT"))
			s_xm_count = bcls_rec->Tables[0].Rows[0]["S_XM_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_XM_SUM"))
			c_xm_sum = bcls_rec->Tables[0].Rows[0]["C_XM_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_XM_COUNT"))
			c_xm_count = bcls_rec->Tables[0].Rows[0]["C_XM_COUNT"].ToDecimal();
		//南区
		if (bcls_rec->Tables[0].Columns.Contains("N_BOF_CNUMBER"))
			bofcnumber = bcls_rec->Tables[0].Rows[0]["N_BOF_CNUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("N_SI_NUMBER"))
			sinumber = bcls_rec->Tables[0].Rows[0]["N_SI_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("N_FE_NUMBER"))
			fenumber = bcls_rec->Tables[0].Rows[0]["N_FE_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("N_BOF_SNUMBER"))
			bofsnumber = bcls_rec->Tables[0].Rows[0]["N_BOF_SNUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("N_MOLD_NUMBER"))
			moldnumber = bcls_rec->Tables[0].Rows[0]["N_MOLD_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("N_CCM_CNUMBER"))
			ccmcnumber = bcls_rec->Tables[0].Rows[0]["N_CCM_CNUMBER"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_CCM_SNUMBER"))
			ccmsnumber = bcls_rec->Tables[0].Rows[0]["N_CCM_SNUMBER"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_SUM_COU"))
			sumcou = bcls_rec->Tables[0].Rows[0]["N_SUM_COU"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_SUM_WT"))
			sumwt = bcls_rec->Tables[0].Rows[0]["N_SUM_WT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_JK_COUNT"))
			jkcount = bcls_rec->Tables[0].Rows[0]["N_JK_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_JK_SUM"))
			jksum = bcls_rec->Tables[0].Rows[0]["N_JK_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_RH_NUMBER"))
			rhnumber = bcls_rec->Tables[0].Rows[0]["N_RH_NUMBER"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("N_S_JK_CAR"))
			sjkcar = bcls_rec->Tables[0].Rows[0]["N_S_JK_CAR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_S_JK_COUNT"))
			sjkcount = bcls_rec->Tables[0].Rows[0]["N_S_JK_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_S_JK_SUM"))
			sjksum = bcls_rec->Tables[0].Rows[0]["N_S_JK_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_C_JK_CAR"))
			cjkcar = bcls_rec->Tables[0].Rows[0]["N_C_JK_CAR"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_C_JK_COUNT"))
			cjkcount = bcls_rec->Tables[0].Rows[0]["N_C_JK_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_C_JK_SUM"))
			cjksum = bcls_rec->Tables[0].Rows[0]["N_C_JK_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_CL_CNUMBER"))
			clcnumber = bcls_rec->Tables[0].Rows[0]["N_CL_CNUMBER"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("N_CL_SNUMBER"))
			clsnumber = bcls_rec->Tables[0].Rows[0]["N_CL_SNUMBER"].ToDecimal();
		//每日推送宝武
		if (bcls_rec->Tables[0].Columns.Contains("C_RECEIVE_WEIGHT"))
			c_receive_weight = bcls_rec->Tables[0].Rows[0]["C_RECEIVE_WEIGHT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("S_RECEIVE_WEIGHT"))
			s_receive_weight = bcls_rec->Tables[0].Rows[0]["S_RECEIVE_WEIGHT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("RECV_TIME"))
			date = bcls_rec->Tables[0].Rows[0]["RECV_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ZO_COUNT"))
			zo_count = bcls_rec->Tables[0].Rows[0]["ZO_COUNT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("C_RECEIVE_WEIGHT_LJ"))
			c_receive_weight_lj = bcls_rec->Tables[0].Rows[0]["C_RECEIVE_WEIGHT_LJ"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("S_RECEIVE_WEIGHT_LJ"))
			s_receive_weight_lj = bcls_rec->Tables[0].Rows[0]["S_RECEIVE_WEIGHT_LJ"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ZO_COUNT_LJ"))
			zo_count_lj = bcls_rec->Tables[0].Rows[0]["ZO_COUNT_LJ"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TAP_DATE"))
			tap_date = bcls_rec->Tables[0].Rows[0]["TAP_DATE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("BOF_NUMBER"))
			bof_number = bcls_rec->Tables[0].Rows[0]["BOF_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DEP_NUMBER_BOF"))
			dep_number_bof = bcls_rec->Tables[0].Rows[0]["DEP_NUMBER_BOF"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SI_AVER_BOF"))
			si_aver_bof = bcls_rec->Tables[0].Rows[0]["SI_AVER_BOF"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HMTEMP_AVER_BOF"))
			hmtemp_aver_bof = bcls_rec->Tables[0].Rows[0]["HMTEMP_AVER_BOF"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("AOD_NUMBER"))
			aod_number = bcls_rec->Tables[0].Rows[0]["AOD_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_DES_AOD"))
			number_des_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_DES_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_BOF_AOD"))
			number_bof_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_BOF_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_EAF_AOD"))
			number_eaf_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_EAF_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NUMBER_HIGH_SI_AOD"))
			number_high_si_aod = bcls_rec->Tables[0].Rows[0]["NUMBER_HIGH_SI_AOD"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("EAF_NUMBER"))
			eaf_number = bcls_rec->Tables[0].Rows[0]["EAF_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("IF_NUMBER"))
			if_number = bcls_rec->Tables[0].Rows[0]["IF_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DES_NUMBER"))
			des_number = bcls_rec->Tables[0].Rows[0]["DES_NUMBER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("S_JK_SUM"))
			s_jk_sum = bcls_rec->Tables[0].Rows[0]["S_JK_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_JK_SUM"))
			c_jk_sum = bcls_rec->Tables[0].Rows[0]["C_JK_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_JK_COUNT"))
			c_jk_count = bcls_rec->Tables[0].Rows[0]["C_JK_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("S_XM_SUM"))
			s_xm_sum = bcls_rec->Tables[0].Rows[0]["S_XM_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("S_XM_COUNT"))
			s_xm_count = bcls_rec->Tables[0].Rows[0]["S_XM_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_XM_SUM"))
			c_xm_sum = bcls_rec->Tables[0].Rows[0]["C_XM_SUM"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("C_XM_COUNT"))
			c_xm_count = bcls_rec->Tables[0].Rows[0]["C_XM_COUNT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("ACT_AMOUNT"))
			act_amount = bcls_rec->Tables[0].Rows[0]["ACT_AMOUNT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("YG_ACCOUT_WT"))
			yg_accout_wt = bcls_rec->Tables[0].Rows[0]["YG_ACCOUT_WT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SOUTH_ACCOUT_WT"))
			south_accout_wt = bcls_rec->Tables[0].Rows[0]["SOUTH_ACCOUT_WT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("NORTH_ACCOUT_WT"))
			north_accout_wt = bcls_rec->Tables[0].Rows[0]["NORTH_ACCOUT_WT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STEEL_AMOUNT"))
			steel_amount = bcls_rec->Tables[0].Rows[0]["STEEL_AMOUNT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("XM_WAIT_WEIGHT_NORTH"))
			xm_wait_weight_north = bcls_rec->Tables[0].Rows[0]["XM_WAIT_WEIGHT_NORTH"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("XM_WAIT_COUNT_NORTH"))
			xm_wait_count_north = bcls_rec->Tables[0].Rows[0]["XM_WAIT_COUNT_NORTH"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("L2N_FG_WT"))
			l2n_fg_wt = bcls_rec->Tables[0].Rows[0]["L2N_FG_WT"].ToString().Trim();
		//sumcou+c_xm_count+s_xm_count块sumwt+c_xm_sum+s_xm_sum 总修模量 zxm
		//修磨块
		xiumk = sumcou + c_xm_count + s_xm_count;
		//修磨吨
		xiumdun = sumwt + c_xm_sum + s_xm_sum;
		zplyk = s_jk_count + c_jk_count + sjkcount + cjkcount;
		zplydun = s_jk_sum + c_jk_sum + sjksum + cjksum;

		//党员推送
		if (bcls_rec->Tables[0].Columns.Contains("TAP_DATE"))
			tap_date = bcls_rec->Tables[0].Rows[0]["TAP_DATE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("CONTENT"))
			content = bcls_rec->Tables[0].Rows[0]["CONTENT"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TOPIC"))
			topic = bcls_rec->Tables[0].Rows[0]["TOPIC"].ToString().Trim();

		//专业模块代码 -- 防止之后不同程序接入发不同的推送信息的一个判断代码
		if (bcls_rec->Tables[0].Columns.Contains("CODE"))
			code = bcls_rec->Tables[0].Rows[0]["CODE"].ToString();
		Log::Trace("", "", "st_nowsl={0}", st_no);
		Log::Trace("", "", "st_code{0}", code);
		Log::Trace("", "", "remark{0}", remark);

		//获取该条件相关信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT  MODULE_NAME,LOGINNAME,LOGINNAME_LIST,MESSAGE_TITLE,MESSAGE_LIST,KZABSCHL "
				"   FROM TQMTSBWU "
				"  WHERE CODE = @code  ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("code", code);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			MODULE_NAME = cmd_inq.GetString(1);//模块名
			LOGINNAME = cmd_inq.GetString(2);//推送人
			LOGINNAME_LIST = cmd_inq.GetString(3);//消息通知人
			MESSAGE_TITLE = cmd_inq.GetString(4); //消息标题
			MESSAGE_LIST = cmd_inq.GetString(5);//消息内容
			KZABSCHL = cmd_inq.GetString(6);//推送开关
		}
		cmd_inq.Close();
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT  SEND_FLAG "
				"   FROM TQMTSBWU_H "
				"  WHERE CODE = @code and REMARK = @remark ";
			break;
		}
		cmdws_inq.SetCommandText(sqlstr);
		cmdws_inq.Parameters.Set("code", code);
		cmdws_inq.Parameters.Set("remark", remark);
		cmdws_inq.ExecuteReader();
		if (cmdws_inq.Read())
		{
			SEND_FLAG = cmdws_inq.GetString(1);//已发送标志
		}
		cmdws_inq.Close();
		Log::Trace("", "", "SEND_FLAG{0}", SEND_FLAG);
		if ((SEND_FLAG.Trim() == "0" || SEND_FLAG.Trim() == ""))//1已发送，0则发送 KZABSCHL 推送开关按钮
		{
			Log::Trace("", __FUNCTION__, "发送宝武聊天", "");
			EIClass* inblock = new EIClass();
			EIClass* outblock = new EIClass();
			inblock->Tables[0].set_TableName("SYS");
			inblock->Tables[0].Columns.Add(DT_STRING, "messageTitle");
			inblock->Tables[0].Columns.Add(DT_STRING, "message");
			inblock->Tables[0].Columns.Add(DT_STRING, "appId");
			inblock->Tables[0].Columns.Add(DT_STRING, "sendUserId");
			inblock->Tables[0].Columns.Add(DT_STRING, "msgType");
			inblock->Tables[0].Columns.Add(DT_STRING, "showType");
			inblock->Tables[0].Columns.Add(DT_STRING, "sendMode");
			inblock->Tables[0].Columns.Add(DT_STRING, "secretCode");
			inblock->Tables[0].Columns.Add(DT_STRING, "msgCategory");
			inblock->Tables[0].Columns.Add(DT_STRING, "isSend");
			inblock->Tables[0].Columns.Add(DT_STRING, "configNum");

			inblock->Tables[0].Columns.Add(DT_STRING, "title");
			inblock->Tables[0].Columns.Add(DT_STRING, "btntxt");
			inblock->Tables[0].Columns.Add(DT_STRING, "wxurl");
			inblock->Tables[0].Columns.Add(DT_STRING, "description");
			inblock->Tables[0].Columns.Add(DT_STRING, "appUrl");
			CDataRow& row0 = inblock->Tables[0].Rows.Add();
			row0["messageTitle"] = MESSAGE_TITLE;

			//工艺卡下发
			if (code == "1")
			{
				row0["message"] = MESSAGE_LIST + "[" + st_no + "]";
			}
			//当日产量
			else if (code == "2")
			{
				row0["message"] = /*"系统通知 " + date_ry + " " + date_fz + "<br>" +*/ MESSAGE_LIST + date + "日产量：<br>碳钢：" + c_receive_weight + "吨   累计: " + c_receive_weight_lj + "万吨<br>"
					+ "不锈钢：" + s_receive_weight + "吨   累计：" + s_receive_weight_lj + "万吨<br>总产量：" + zo_count + "吨   累计：" + zo_count_lj + "万吨<br>"
					"板坯库存量：<br>碳钢：" + mat_wt + "吨<br>" + "不锈钢：" + mat_wt_wt + "吨<br> 汇总：" + count_huizo + "吨<br>"
					"修磨情况：<br>总修磨量：" + mend_afer_wt + "吨，块数：" + count_ks;

			}
			//板坯提示
			else if (code == "3")
			{
				row0["message"] = /*"系统通知 " + date_ry + " " + date_fz + "<br>" +*/ MESSAGE_LIST + date + "日板坯库存量：<br>碳钢：" + mat_wt + "吨<br>" + "不锈钢：" + mat_wt_wt + "吨<br> 汇总：" + count_huizo + "吨";
			}
			//成品检查
			else if (code == "4")
			{
				if (deal_notion = "1")
				{
					deal_notion = "内弧边修";
				}
				else if (deal_notion = "2")
				{
					deal_notion = "内弧边+中修磨";
				}
				else if (deal_notion = "3")
				{
					deal_notion = "内弧局修";
				}
				else if (deal_notion = "4")
				{
					deal_notion = "内弧全修";
				}
				else if (deal_notion = "5")
				{
					deal_notion = "内、外弧边修";
				}
				else if (deal_notion = "6")
				{
					deal_notion = "内、外弧边+中修磨";
				}
				else if (deal_notion = "7")
				{
					deal_notion = "全修";
				}
				else if (deal_notion = "8")
				{
					deal_notion = "外弧边修";
				}
				else if (deal_notion = "9")
				{
					deal_notion = "外弧边+中修磨";
				}
				else if (deal_notion = "10")
				{
					deal_notion = "外弧局修";
				}
				else if (deal_notion = "11")
				{
					deal_notion = "外弧全修";
				}

				row0["message"] = MESSAGE_LIST + "<br>板坯号[" + batch + "]"
					+ "<br>钢种[" + sg_sign + "]<br>连铸机[" + unit_code + "]<br>宽[" + mat_width + "]<br>检验员处置意见[" + deal_notion +
					+"]<br>铸坯修磨主要原因[" + grind_main_reason + "]<br>判定结果[" + casting_pre_judgment + "]";
			}
			//修磨量
			else if (code == "5")
			{
				row0["message"] = /*"系统通知 " + date_ry + " " + date_fz + "<br>" + */MESSAGE_LIST + date + "日修磨情况：<br>总修磨量：" + mend_afer_wt + "吨，块数：" + count_ks;
			}
			//表面质量封锁
			else if (code == "6")
			{
				row0["message"] = MESSAGE_LIST + +"材料号：" + remak_cf + "描述：" + surf_quality;
			}

			//成分不合
			else if (code == "7")
			{
				row0["message"] = MESSAGE_LIST + remak_cf;
			}
			//工艺卡违规
			else if (code == "8")
			{
				row0["message"] = MESSAGE_LIST + "<br>炉号：" + heat_no + "<br>钢种：" + st_no + "<br>班组：" + gpoup + "<br>责任单位：" + desc +
					"<br>违规项目：" + flag_mc + "<br>违规类型：" + un_flag + "<br>违规级别：" + dlag_id + "<br>结果跟踪：" + wt_desc + "<br>整改措施：" + err_desc + "<br>检查人：" + examiner + "<br>检查日期：" + jc_rq_time;
			}
			//火车库402
			else if (code == "9")
			{
				row0["message"] = MESSAGE_LIST + remark_HC;
			}
			//镍板库405
			else if (code == "10")
			{
				row0["message"] = MESSAGE_LIST + remark_NICKEL;
			}
			//镍板库405
			else if (code == "11")
			{
				row0["message"] = MESSAGE_LIST + remark_LK;
			}
			//废钢料场
			else if (code == "12")
			{
				row0["message"] = MESSAGE_LIST + remark_FG;
			}
			else if (code == "14")
			{
				row0["message"] = MESSAGE_LIST + "<br>炼钢厂" + tap_date.SubstringNE(6, 2) + "日生产情况" + prod_shift_no + "班<br>1.生产炉数:<br>"
					"1.1南区碳钢" + bofcnumber + "炉,其中过RH" + rhnumber + "炉,硅钢" + sinumber + "炉,纯铁" + fenumber + "炉,连铸" + ccmcnumber.ToString() + "炉。<br>"
					"1.2南区不锈钢" + bofsnumber + "炉,其中连铸" + ccmsnumber.ToString() + "炉,模铸" + moldnumber + "炉。<br>"
					"1.3北区碳钢" + bof_number + "炉,转炉脱磷铁水" + dep_number_bof + "炉,三脱铁水" + des_number + "炉。<br>"
					"1.4北区不锈钢" + aod_number + "炉,其中三脱" + number_des_aod + "炉,中脱" + number_bof_aod + "炉，一兑一" + number_eaf_aod + "炉，高硅" + number_high_si_aod + "炉。<br>"
					"1.5电炉" + eaf_number + "炉,合金熔化炉" + if_number + "炉。<br>"
					"2.物流信息:<br>"
					"2.1修磨量" + xiumk.ToString() + "块" + xiumdun.ToString() + "吨，其中南区" + sumcou.ToString() + "块" + sumwt.ToString() + "吨,北区碳钢" + c_xm_count.ToString() + "块" + c_xm_sum.ToString() + "吨,不锈钢" + s_xm_count.ToString() + "块" + s_xm_sum.ToString() + "吨。<br>"
					"2.2铸坯拉运" + zplyk.ToString() + "块" + zplydun.ToString() + "吨，其中南区不锈钢" + sjkcar.ToString() + "车" + sjkcount.ToString() + "块" + sjksum.ToString() + "吨,南区碳钢" + cjkcar.ToString() + "车" + cjkcount.ToString() + "块" + cjksum.ToString() + "吨,"
					"北区不锈钢" + s_jk_count.ToString() + "块" + s_jk_sum.ToString() + "吨, 北区碳钢" + c_jk_count.ToString() + "块" + c_jk_sum.ToString() + "吨。";
			}
			else if (code == "15")
			{
				Log::Trace("", "", "进入每天推送{0}", date);
				row0["message"] = MESSAGE_LIST + "<br>炼钢厂" + tap_date.SubstringNE(6, 2) + "日生产情况<br>1.铁水情况:<br>"
					"1.1高炉出铁" + act_amount + "吨,其中一钢使用" + yg_accout_wt + "吨,二钢南区使用" + south_accout_wt + "吨,北区使用" + north_accout_wt + "吨,铸铁" + steel_amount + "吨。<br>"
					"1.2铁水平均温度" + hmtemp_aver_bof + "度,铁水平均硅" + si_aver_bof + "%,鱼雷罐加废钢" + l2n_fg_wt + "吨。<br>"
					"2.产量完成情况:<br>"
					"2.1全厂生产量:合计" + zo_count + "吨,累计" + zo_count_lj + "万吨。其中北区不锈钢" + s_receive_weight + "吨，累计" + s_receive_weight_lj + "万吨，北区碳钢" + c_receive_weight + "吨，累计" + c_receive_weight_lj + "万吨。南区不锈钢" + clcnumber.ToString() + "吨 南区碳钢" + clsnumber.ToString() + "吨。<br>"
					"3.生产炉数:<br>"
					"3.1南区碳钢" + bofcnumber + "炉,其中过RH" + rhnumber + "炉,硅钢" + sinumber + "炉,纯铁" + fenumber + "炉,连铸" + ccmcnumber.ToString() + "炉。<br>"
					"3.2南区不锈钢" + bofsnumber + "炉,其中连铸" + bofsnumber + "炉,模铸" + moldnumber + "炉。<br>"
					"3.3北区碳钢" + bof_number + "炉,转炉脱磷铁水" + dep_number_bof + "炉,三脱铁水" + des_number + "炉。<br>"
					"3.4北区不锈钢" + aod_number + "炉,其中三脱" + number_des_aod + "炉,中脱" + number_bof_aod + "炉，一兑一" + number_eaf_aod + "炉，高硅" + number_high_si_aod + "炉。<br>"
					"3.5电炉" + eaf_number + "炉,合金熔化炉" + if_number + "炉。<br>"
					"4.物流信息:<br>"
					"4.1总修磨量" + xiumdun.ToString() + "吨，其中南区" + sumcou.ToString() + "块" + sumwt.ToString() + "吨,北区碳钢" + c_xm_count.ToString() + "块" + c_xm_sum.ToString() + "吨,不锈钢" + s_xm_count.ToString() + "块" + s_xm_sum.ToString() + "吨。北区待磨料" + xm_wait_count_north + "块" + xm_wait_weight_north + "吨。<br>"
					"4.2铸坯拉运" + zplyk.ToString() + "块" + zplydun.ToString() + "吨，其中南区不锈钢" + sjkcar.ToString() + "车" + sjkcount.ToString() + "块" + sjksum.ToString() + "吨,南区碳钢" + cjkcar.ToString() + "车" + cjkcount.ToString() + "块" + cjksum.ToString() + "吨,北区不锈钢" + s_jk_count.ToString() + "块" + s_jk_sum.ToString() + "吨, 北区碳钢" + c_jk_count.ToString() + "块" + c_jk_sum.ToString() + "吨。";
			}
			else if (code == "16")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "17")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "18")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "19")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "20")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "21")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "22")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "23")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			else if (code == "24")
			{
				row0["message"] = "炼钢厂安全学习园地<br>" + topic + "<br>" + content + "";
			}
			//双基管理
			else if (code == "25")
			{
				row0["message"] = "【" + MESSAGE_LIST + "】<br>" + remark_LK;
			}
			else
			{
				row0["message"] = MESSAGE_LIST + remark_FG;
			}

			//整体消息内容
			Messagew_list = row0["message"];
			Log::Trace("", "", "Messagew_list{0}", Messagew_list);

			row0["appId"] = appId;
			row0["sendUserId"] = LOGINNAME;
			row0["msgType"] = msgType;
			row0["showType"] = showType;
			row0["sendMode"] = sendMode;
			row0["secretCode"] = secretCode;
			row0["msgCategory"] = msgCategory;
			row0["isSend"] = isSend;
			row0["title"] = title;
			inblock->Tables.Add("LOGIN_NAME");
			inblock->Tables[1].Columns.Add(DT_STRING, "LoginNameList");//发送工号(可以多人)

			EIClass bcls_rec_id;
			bcls_rec_id.Tables[0].Columns.Add(DT_STRING, "LoginNameList");

			//获取该条件相关信息
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT USER_ID FROM TQMTSBWU_YH WHERE CODE = @code  ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("code", code);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				username = cmd_inq.GetString(1);//工号
				inblock->Tables[1].Rows.Add();
				inblock->Tables[1].Rows[i]["LoginNameList"] = username;//截取后的工号添加到LOGIN_NAME块

				bcls_rec_id.Tables[0].Rows.Add();
				bcls_rec_id.Tables[0].Rows[i]["LoginNameList"] = username;//全部工号 缓冲
				i++;
			}
			cmd_inq.Close();

			id_count = inblock->Tables[1].Rows.get_Count();

			if (id_count <= 200)
			{
				LOGINNAME_LIST = "";
				for (int seq = 0; seq < id_count; seq++)
				{
					LOGINNAME_LIST += inblock->Tables[1].Rows[seq]["LoginNameList"].ToString() + ",";
				}
				if (LOGINNAME_LIST.GetLength()>0)
				{
					LOGINNAME_LIST = LOGINNAME_LIST.SubstringNE(0, LOGINNAME_LIST.GetLength() - 1);
				}
				Log::Trace("", __FUNCTION__, "11250 LOGINNAME_LIST[{0}]", LOGINNAME_LIST);
				if (KZABSCHL == "1")
				{
					f_call_ijudge_svc(conn, "TG23M_0003", "1", "MSG", "callWeChat", inblock, outblock, true);
					CString status = " ";
					Log::Trace("", __FUNCTION__, "outblock[{0}]", outblock->Tables["Table0"].Rows[0]["status"].ToString().Trim());
					Log::Trace("", __FUNCTION__, "msg[{0}]", outblock->Tables["Table0"].Rows[0]["msg"].ToString().Trim());
				}
				//发送成功信息写入履历表
				tqmtsbwu_h["REC_CREATOR"] = s.userid;
				tqmtsbwu_h["REC_CREATE_TIME"] = dateNow;
				tqmtsbwu_h["LOGINNAME"] = LOGINNAME; //推送人
				tqmtsbwu_h["LOGINNAME_LIST"] = LOGINNAME_LIST;//消息通知人
				tqmtsbwu_h["MESSAGE_LIST"] = Messagew_list; //消息内容
				tqmtsbwu_h["MESSAGE_TITLE"] = MESSAGE_TITLE; //消息标题
				tqmtsbwu_h["CODE"] = code;//代码
				tqmtsbwu_h["MODULE_NAME"] = MODULE_NAME;//模块名
				tqmtsbwu_h["SEND_FLAG"] = "1";//已发送标志
				tqmtsbwu_h["REMARK"] = remark;//跟各个业务唯一对应字段
				tqmtsbwu_h.TrimOrBlank();
				tqmtsbwu_h.Insert();
			}
			else
			{
				for (int group = 0; group <= id_count / 200; group++)
				{
					inblock->Tables[1].Rows.Clear();
					LOGINNAME_LIST = "";
					for (int seq = 0; seq < 200; seq++)
					{
						if (seq + group * 200 >= id_count) break;
						inblock->Tables[1].Rows.Add();
						inblock->Tables[1].Rows[seq]["LoginNameList"] = bcls_rec_id.Tables[0].Rows[seq + group*200]["LoginNameList"];
						LOGINNAME_LIST += inblock->Tables[1].Rows[seq]["LoginNameList"].ToString() + ",";

					}

					if (LOGINNAME_LIST.GetLength()>0)
					{
						LOGINNAME_LIST = LOGINNAME_LIST.SubstringNE(0, LOGINNAME_LIST.GetLength() - 1);
					}
					Log::Trace("", __FUNCTION__, "11251 LOGINNAME_LIST[{0}]", LOGINNAME_LIST);
					if (KZABSCHL == "1")
					{
						f_call_ijudge_svc(conn, "TG23M_0003", "1", "MSG", "callWeChat", inblock, outblock, true);
						CString status = " ";
						Log::Trace("", __FUNCTION__, "outblock[{0}]", outblock->Tables["Table0"].Rows[0]["status"].ToString().Trim());
						Log::Trace("", __FUNCTION__, "msg[{0}]", outblock->Tables["Table0"].Rows[0]["msg"].ToString().Trim());
					}
					//发送成功信息写入履历表
					tqmtsbwu_h["REC_CREATOR"] = s.userid;
					tqmtsbwu_h["REC_CREATE_TIME"] = dateNow;
					tqmtsbwu_h["LOGINNAME"] = LOGINNAME; //推送人
					tqmtsbwu_h["LOGINNAME_LIST"] = LOGINNAME_LIST;//消息通知人
					tqmtsbwu_h["MESSAGE_LIST"] = Messagew_list; //消息内容
					tqmtsbwu_h["MESSAGE_TITLE"] = MESSAGE_TITLE; //消息标题
					tqmtsbwu_h["CODE"] = code;//代码
					tqmtsbwu_h["MODULE_NAME"] = MODULE_NAME;//模块名
					tqmtsbwu_h["SEND_FLAG"] = "1";//已发送标志
					tqmtsbwu_h["REMARK"] = remark;//跟各个业务唯一对应字段
					tqmtsbwu_h.TrimOrBlank();
					tqmtsbwu_h.Insert();
				}
			}

			tpcommit(0);
			tpbegin(0, 0);

			//推送宝武群聊-消息引擎
			//当日产量-2 每班推送-14 每日推送-15 推群聊
			if (code == "2" || code == "14" || code == "15" )
			{
				EIClass iblk_yq;
				if (iblk_yq.Tables.Contains("RULE_CONFIG") == false)
				{
					iblk_yq.Tables[0].set_TableName("RULE_CONFIG");
					iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "PROJECT_ENAME");
					iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "ENV_TYPE");
					iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "VERSION");
					iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CUSTOM_CONFIG");
					iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CODE_CLASS");
					iblk_yq.Tables["RULE_CONFIG"].Rows.Add();
					iblk_yq.Tables["RULE_CONFIG"].Rows[0]["PROJECT_ENAME"] = "TASK";//固定值，不变
					iblk_yq.Tables["RULE_CONFIG"].Rows[0]["ENV_TYPE"] = "1";//测试--0，正式--1
					iblk_yq.Tables["RULE_CONFIG"].Rows[0]["VERSION"] = "20241201";//固定值，不变
					iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CUSTOM_CONFIG"] = "T";//固定值，不变
					iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CODE_CLASS"] = "EPIJG0";//固定值，不变
				}
				if (iblk_yq.Tables.Contains("PROJECT_CONFIG") == false)
				{
					iblk_yq.Tables.Add();
					iblk_yq.Tables[1].set_TableName("PROJECT_CONFIG");
					iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MESSAGE_CLASS");//三列必须有
					iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "UNIT_CODE");//三列必须有
					iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MAT_NO");//三列必须有
				}
				if (iblk_yq.Tables.Contains("DATA_CUSTOM") == false)
				{
					Log::Trace("", "", "inBlock 初始化表3为 DATA_CUSTOM ");

					iblk_yq.Tables.Add("DATA_CUSTOM");
					iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "UNIT_CODE");//该列必须有
					iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "CODE");//参与计算的列
	
				}

				iblk_yq.Tables["PROJECT_CONFIG"].Rows.Add();
				iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["MESSAGE_CLASS"] = "BWCH";//任务池准入条件的比对值
				iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["UNIT_CODE"] = "H000";//不定机组


				iblk_yq.Tables["DATA_CUSTOM"].Rows.Add();
				iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
				iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["CODE"] = code;

				doFlag = f_qmts_call_judge(&iblk_yq, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", "", "引擎失败 ");
					doFlag = 0;
					s.flag = 0;
				}
				Log::Trace("", "", "引擎成功 ");

				return 0;

			}
			
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
