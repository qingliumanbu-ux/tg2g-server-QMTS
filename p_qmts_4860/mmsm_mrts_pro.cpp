/************************/
/*** 2024-9-5 ********/
/****   xmy **************/
/**** 每天推送  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天
int f_tran_json_func(EIClass* blks_in, EIClass * blks_out, CDbConnection* conn);//调用
int f_push_baowu_chat_dd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天（群聊）
// service入口
BM2F_ENTERACE(mmsm_mrts_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_mrts_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstrnq = "";
	CString sqlstrn = "";
	//查询前一天的时间
	CString datetime = CDateTime::Now().AddDays(-1).ToString("yyyyMMddHHmmss");
	CString date_r = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");
	CDecimal c_receive_weight = 0;
	CDecimal s_receive_weight = 0;
	CDecimal c_receive_weight_lj = 0;
	CDecimal s_receive_weight_lj = 0;
	CString date = "";
	CString mat_no = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqnq(conn);
	CDbCommand cmd_inq1(conn); 
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CDbCommand cmd_inqz(conn);
	CDbCommand cmd_inqn(conn);
	CString tap_date = " ";
	CString bof_number = " ";
	CString dep_number_bof = " ";
	CString si_aver_bof = " ";
	CString hmtemp_aver_bof = " ";
	CString aod_number = " ";
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
	CString c_xm_count = " ";
	CString act_amount = " ";
	CString yg_accout_wt = " ";
	CString south_accout_wt = " ";
	CString north_accout_wt = " ";
	CString steel_amount = " ";
	CString xm_wait_weight_north = " ";
	CString xm_wait_count_north = " ";
	CString l2n_fg_wt = " ";
	//南区字段
	CString tapDate = " ";
	CString bofcnumber = " ";
	CString sinumber = " ";
	CString rhnumber = " ";
	CString fenumber = " ";
	CString bofsnumber = " ";
	CString moldnumber = " ";
	CString ccmcnumber = " ";
	CString ccmsnumber = " ";
	CString sumcou = " ";
	CString sumwt = " ";
	CString jkcount = " ";
	CString jksum = " ";
	CString cjkcount = " ";
	CString sjksum = " ";
	CString sjkcount = " ";
	CString cjksum = " ";
	CString sjkcar = " ";
	CString cjkcar = " ";
	CDecimal sum_h = 0;
	CDecimal clcnumber = 0;
	CDecimal clsnumber = 0;
	EIClass iplat_Tab;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_RECEIVE_WEIGHT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_RECEIVE_WEIGHT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "RECV_TIME");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ZO_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_RECEIVE_WEIGHT_LJ");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_RECEIVE_WEIGHT_LJ");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ZO_COUNT_LJ");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TAP_DATE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "BOF_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEP_NUMBER_BOF");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SI_AVER_BOF");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "HMTEMP_AVER_BOF");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "AOD_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "NUMBER_DES_AOD");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "NUMBER_BOF_AOD");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "NUMBER_EAF_AOD");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "NUMBER_HIGH_SI_AOD");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "EAF_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "IF_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DES_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_JK_SUM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_JK_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_JK_SUM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_JK_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_XM_SUM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "S_XM_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_XM_SUM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_XM_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ACT_AMOUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "YG_ACCOUT_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SOUTH_ACCOUT_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "NORTH_ACCOUT_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "STEEL_AMOUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "XM_WAIT_WEIGHT_NORTH");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "XM_WAIT_COUNT_NORTH");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "L2N_FG_WT");
	//南区数据
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_BOF_CNUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_SI_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_FE_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_BOF_SNUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_MOLD_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_CCM_CNUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_CCM_SNUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_SUM_COU");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_SUM_WT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_JK_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_JK_SUM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_RH_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_S_JK_CAR");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_S_JK_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_S_JK_SUM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_C_JK_CAR");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_C_JK_COUNT");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_C_JK_SUM");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_CL_CNUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "N_CL_SNUMBER");

	try
	{
		//南区生产量 cmd_inqn
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstrn = " select SUM_H from VMMSM_NQ_LJ ";
		}
		cmd_inqn.SetCommandText(sqlstrn);
		cmd_inqn.ExecuteReader();
		if (cmd_inqn.Read())
		{
			sum_h = cmd_inqn.GetDecimal(1);
		}
		cmd_inqn.Close();
		//碳钢当日产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)) MAT_ACT_WT, substr(to_date(sysdate - 1), 0, 2) RECV_TIME FROM VMMSMTS_CGCL";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			c_receive_weight = cmd_inq.GetDecimal(1);
			date = cmd_inq.GetString(2);
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "c_receive_weight[{0}]  ", c_receive_weight);
		Log::Trace("", __FUNCTION__, "date[{0}]  ", date);

		//碳钢累计产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)/10000,2) MAT_ACT_WT  FROM VMMSMTS_CGCL_LJ";
		}
		cmd_inq2.SetCommandText(sqlstr);
		cmd_inq2.ExecuteReader();
		if (cmd_inq2.Read())
		{
			c_receive_weight_lj = cmd_inq2.GetDecimal(1);
		}
		cmd_inq2.Close();


		//不锈钢当日产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)) MAT_ACT_WT FROM VMMSMTS_BXGCL";
		}
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteReader();
		if (cmd_inq1.Read())
		{
			s_receive_weight = cmd_inq1.GetDecimal(1);
		}
		cmd_inq1.Close();

		Log::Trace("", __FUNCTION__, "s_receive_weight[{0}]  ", s_receive_weight);

		//不锈钢累计产量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT  ROUND(SUM(MAT_ACT_WT)/10000,2) MAT_ACT_WT  FROM VMMSMTS_BXGCL_LJ";
		}
		cmd_inq3.SetCommandText(sqlstr);
		cmd_inq3.ExecuteReader();
		if (cmd_inq3.Read())
		{
			s_receive_weight_lj = cmd_inq3.GetDecimal(1);
		}
		cmd_inq3.Close();

		//炉次,铁水,物流
		tap_date = date_r;
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT tap_date, "
				" bof_number, "
				" dep_number_bof, "
				" si_aver_bof, "
				" hmtemp_aver_bof, "
				" aod_number, "
				" number_des_aod, "
				" number_bof_aod, "
				" number_eaf_aod, "
				" number_high_si_aod, "
				" eaf_number, "
				" if_number, "
				" des_number, "
				" s_jk_sum, "
				" s_jk_count, "
				" c_jk_sum, "
				" c_jk_count, "
				" s_xm_sum, "
				" s_xm_count, "
				" c_xm_sum, "
				" c_xm_count, "
				" act_amount, "
				" yg_accout_wt, "
				" south_accout_wt, "
				" north_accout_wt, "
				" steel_amount, "
				" xm_wait_weight_north, "
				" xm_wait_count_north, "
				" L2N_FG_WT "
				" FROM ZTC_V_NORTH_PRO_INFO_DAY where tap_date='" + tap_date + "' ";
		}
		cmd_inqz.SetCommandText(sqlstr);
		cmd_inqz.ExecuteReader();
		if (cmd_inqz.Read())
		{
			tap_date = cmd_inqz.GetString(1);
			bof_number = cmd_inqz.GetString(2);
			dep_number_bof = cmd_inqz.GetString(3);
			si_aver_bof = cmd_inqz.GetString(4);
			hmtemp_aver_bof = cmd_inqz.GetString(5);
			aod_number = cmd_inqz.GetString(6);
			number_des_aod = cmd_inqz.GetString(7);
			number_bof_aod = cmd_inqz.GetString(8);
			number_eaf_aod = cmd_inqz.GetString(9);
			number_high_si_aod = cmd_inqz.GetString(10);
			eaf_number = cmd_inqz.GetString(11);
			if_number = cmd_inqz.GetString(12);
			des_number = cmd_inqz.GetString(13);
			s_jk_sum = cmd_inqz.GetString(14);
			s_jk_count = cmd_inqz.GetString(15);
			c_jk_sum = cmd_inqz.GetString(16);
			c_jk_count = cmd_inqz.GetString(17);
			s_xm_sum = cmd_inqz.GetString(18);
			s_xm_count = cmd_inqz.GetString(19);
			c_xm_sum = cmd_inqz.GetString(20);
			c_xm_count = cmd_inqz.GetString(21);
			act_amount = cmd_inqz.GetString(22);
			yg_accout_wt = cmd_inqz.GetString(23);
			south_accout_wt = cmd_inqz.GetString(24);
			north_accout_wt = cmd_inqz.GetString(25);
			steel_amount = cmd_inqz.GetString(26);
			xm_wait_weight_north = cmd_inqz.GetString(27);
			xm_wait_count_north = cmd_inqz.GetString(28);
			l2n_fg_wt = cmd_inqz.GetString(29);

		}
		cmd_inqz.Close();
		//南区数据
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select  "
				" bof_cnumber, "
				" si_number, "
				" fe_number, "
				" bof_snumber, "
				" mold_number, "
				" ccm_cnumber, "
				" ccm_snumber, "
				" sum_cou, "
				" sum_wt, "
				" jk_count, "
				" jk_sum, "
				" rh_number, "
				" s_jk_car, "
				" s_jk_count, "
				" s_jk_sum, "
				" c_jk_car, "
				" c_jk_count, "
				" c_jk_sum,CL_CNUMBER,CL_SNUMBER "
				" from TMMSMNQDAY  where PROD_DATE = '" + tap_date + "' ";
		}
		cmd_inqnq.SetCommandText(sqlstr);
		cmd_inqnq.ExecuteReader();
		if (cmd_inqnq.Read())
		{
			bofcnumber = cmd_inqnq.GetString(1);
			sinumber = cmd_inqnq.GetString(2);
			fenumber = cmd_inqnq.GetString(3);
			bofsnumber = cmd_inqnq.GetString(4);
			moldnumber = cmd_inqnq.GetString(5);
			ccmcnumber = cmd_inqnq.GetString(6);
			ccmsnumber = cmd_inqnq.GetString(7);
			sumcou = cmd_inqnq.GetString(8);
			sumwt = cmd_inqnq.GetString(9);
			jkcount = cmd_inqnq.GetString(10);
			jksum = cmd_inqnq.GetString(11);
			rhnumber = cmd_inqnq.GetString(12);
			sjkcar = cmd_inqnq.GetString(13);
			sjkcount = cmd_inqnq.GetString(14);
			sjksum = cmd_inqnq.GetString(15);
			cjkcar = cmd_inqnq.GetString(16);
			cjkcount = cmd_inqnq.GetString(17);
			cjksum = cmd_inqnq.GetString(18);
			clcnumber = cmd_inqnq.GetDecimal(19);
			clsnumber = cmd_inqnq.GetDecimal(20);
		}
		cmd_inqnq.Close();
		//发送宝武聊天
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["C_RECEIVE_WEIGHT"] = c_receive_weight;//碳钢当日产量
		bcls_rec_s.Tables[0].Rows[0]["S_RECEIVE_WEIGHT"] = s_receive_weight;//不锈钢当日产量
		bcls_rec_s.Tables[0].Rows[0]["RECV_TIME"] = date.SubstringNE(6, 2);
		bcls_rec_s.Tables[0].Rows[0]["ZO_COUNT"] = c_receive_weight + s_receive_weight + clsnumber + clcnumber;//总产量
		bcls_rec_s.Tables[0].Rows[0]["C_RECEIVE_WEIGHT_LJ"] = c_receive_weight_lj;//碳钢累计产量
		bcls_rec_s.Tables[0].Rows[0]["S_RECEIVE_WEIGHT_LJ"] = s_receive_weight_lj;//不锈钢累计产量
		bcls_rec_s.Tables[0].Rows[0]["ZO_COUNT_LJ"] = c_receive_weight_lj + s_receive_weight_lj + sum_h;//累计总产量
		bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
		bcls_rec_s.Tables[0].Rows[0]["BOF_NUMBER"] = bof_number;
		bcls_rec_s.Tables[0].Rows[0]["DEP_NUMBER_BOF"] = dep_number_bof;
		bcls_rec_s.Tables[0].Rows[0]["SI_AVER_BOF"] = si_aver_bof;
		bcls_rec_s.Tables[0].Rows[0]["HMTEMP_AVER_BOF"] = hmtemp_aver_bof;
		bcls_rec_s.Tables[0].Rows[0]["AOD_NUMBER"] = aod_number;
		bcls_rec_s.Tables[0].Rows[0]["NUMBER_DES_AOD"] = number_des_aod;
		bcls_rec_s.Tables[0].Rows[0]["NUMBER_BOF_AOD"] = number_bof_aod;
		bcls_rec_s.Tables[0].Rows[0]["NUMBER_EAF_AOD"] = number_eaf_aod;
		bcls_rec_s.Tables[0].Rows[0]["NUMBER_HIGH_SI_AOD"] = number_high_si_aod;
		bcls_rec_s.Tables[0].Rows[0]["EAF_NUMBER"] = eaf_number;
		bcls_rec_s.Tables[0].Rows[0]["IF_NUMBER"] = if_number;
		bcls_rec_s.Tables[0].Rows[0]["DES_NUMBER"] = des_number;
		bcls_rec_s.Tables[0].Rows[0]["S_JK_SUM"] = s_jk_sum;
		bcls_rec_s.Tables[0].Rows[0]["S_JK_COUNT"] = s_jk_count;
		bcls_rec_s.Tables[0].Rows[0]["C_JK_SUM"] = c_jk_sum;
		bcls_rec_s.Tables[0].Rows[0]["C_JK_COUNT"] = c_jk_count;
		bcls_rec_s.Tables[0].Rows[0]["S_XM_SUM"] = s_xm_sum;
		bcls_rec_s.Tables[0].Rows[0]["S_XM_COUNT"] = s_xm_count;
		bcls_rec_s.Tables[0].Rows[0]["C_XM_SUM"] = c_xm_sum;
		bcls_rec_s.Tables[0].Rows[0]["C_XM_COUNT"] = c_xm_count;
		bcls_rec_s.Tables[0].Rows[0]["ACT_AMOUNT"] = act_amount;
		bcls_rec_s.Tables[0].Rows[0]["YG_ACCOUT_WT"] = yg_accout_wt;
		bcls_rec_s.Tables[0].Rows[0]["SOUTH_ACCOUT_WT"] = south_accout_wt;
		bcls_rec_s.Tables[0].Rows[0]["NORTH_ACCOUT_WT"] = north_accout_wt;
		bcls_rec_s.Tables[0].Rows[0]["STEEL_AMOUNT"] = steel_amount;
		bcls_rec_s.Tables[0].Rows[0]["XM_WAIT_WEIGHT_NORTH"] = xm_wait_weight_north;
		bcls_rec_s.Tables[0].Rows[0]["XM_WAIT_COUNT_NORTH"] = xm_wait_count_north;
		bcls_rec_s.Tables[0].Rows[0]["L2N_FG_WT"] = l2n_fg_wt;
		//南区
		bcls_rec_s.Tables[0].Rows[0]["N_BOF_CNUMBER"] = bofcnumber;
		bcls_rec_s.Tables[0].Rows[0]["N_SI_NUMBER"] = sinumber;
		bcls_rec_s.Tables[0].Rows[0]["N_FE_NUMBER"] = fenumber;
		bcls_rec_s.Tables[0].Rows[0]["N_BOF_SNUMBER"] = bofsnumber;
		bcls_rec_s.Tables[0].Rows[0]["N_MOLD_NUMBER"] = moldnumber;
		bcls_rec_s.Tables[0].Rows[0]["N_CCM_CNUMBER"] = ccmcnumber;
		bcls_rec_s.Tables[0].Rows[0]["N_CCM_SNUMBER"] = ccmsnumber;
		bcls_rec_s.Tables[0].Rows[0]["N_SUM_COU"] = sumcou;
		bcls_rec_s.Tables[0].Rows[0]["N_SUM_WT"] = sumwt;
		bcls_rec_s.Tables[0].Rows[0]["N_JK_COUNT"] = jkcount;
		bcls_rec_s.Tables[0].Rows[0]["N_JK_SUM"] = jksum;
		bcls_rec_s.Tables[0].Rows[0]["N_RH_NUMBER"] = rhnumber;
		bcls_rec_s.Tables[0].Rows[0]["N_S_JK_CAR"] = sjkcar;
		bcls_rec_s.Tables[0].Rows[0]["N_S_JK_COUNT"] = sjkcount;
		bcls_rec_s.Tables[0].Rows[0]["N_S_JK_SUM"] = sjksum;
		bcls_rec_s.Tables[0].Rows[0]["N_C_JK_CAR"] = cjkcar;
		bcls_rec_s.Tables[0].Rows[0]["N_C_JK_COUNT"] = cjkcount;
		bcls_rec_s.Tables[0].Rows[0]["N_C_JK_SUM"] = cjksum;
		bcls_rec_s.Tables[0].Rows[0]["N_CL_CNUMBER"] = clcnumber;
		bcls_rec_s.Tables[0].Rows[0]["N_CL_SNUMBER"] = clsnumber;
		bcls_rec_s.Tables[0].Rows[0]["CODE"] = "15";
		bcls_rec_s.Tables[0].Rows[0]["REMARK"] = date_r;
		Log::Trace("", "", "sjkcar[{0}]", sjkcar);
		Log::Trace("", "", "N_S_JK_CAR[{0}]", bcls_rec_s.Tables[0].Rows[0]["N_S_JK_CAR"].ToString());
		/*doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
		if (doFlag < 0)
		{
		strcpy(s.msg, "调用函数报错!");
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		doFlag = f_push_baowu_chat_dd(&bcls_rec_s, bcls_ret, conn);
		if (doFlag < 0)
		{
			strcpy(s.msg, "调用函数报错!");
			throw CApplicationException(-1, s.msg, log.Location);
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
