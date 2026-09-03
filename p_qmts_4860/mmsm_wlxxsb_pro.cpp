/************************/
/*** 2024-9-5 ********/
/****   xmy **************/
/**** 每班推送  ***********/
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
BM2F_ENTERACE(mmsm_wlxxsb_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_wlxxsb_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstrnq = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString date_r = CDateTime::Now().ToString("yyyyMMdd");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqnq(conn);
	CDbCommand cmd_inqtest(conn);
	CString prod_shift = " ";
	CString prod_shift_no = " ";
	CString tap_date = " ";
	CString datetime1 = CDateTime::Now().AddDays(-1).ToString("yyyyMMddHHmmss");
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
	CString	s_jk_sum = " ";
	CString	s_jk_count = " ";
	CString	c_jk_sum = " ";
	CString	c_jk_count = " ";
	CString	s_xm_sum = " ";
	CString	s_xm_count = " ";
	CString	c_xm_sum = " ";
	CString	c_xm_count = " ";
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
	EIClass iplat_Tab;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "TAP_DATE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "PROD_SHIFT_NO");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "BOF_NUMBER");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "DEP_NUMBER_BOF");
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
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
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

	try
	{
		tap_date = datetime.SubstringNE(0, 8);
		prod_shift = datetime.SubstringNE(8, 2);
		Log::Trace("", "", "prod_shift{0}", prod_shift);
		if (prod_shift == "00" || prod_shift == "11"){
			prod_shift_no = "二";
			tap_date = datetime1.SubstringNE(0, 8);
		}
		if (prod_shift == "16" || prod_shift == "14"){
			prod_shift_no = "白";
		}
		if (prod_shift == "08"){
			prod_shift_no = "夜";
		}
		Log::Trace("", "", "tap_date{0}", tap_date);
		//每班推送 北区
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select tap_date, "
				" prod_shift_no, "
				" bof_number, "
				" dep_number_bof, "
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
				" c_xm_count "
				" from ztc_v_north_pro_info  where TAP_DATE='"+tap_date+"' and prod_shift_no='"+prod_shift_no+"' ";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tap_date = cmd_inq.GetString(1);
			prod_shift_no = cmd_inq.GetString(2);
			bof_number = cmd_inq.GetString(3);
			dep_number_bof = cmd_inq.GetString(4);
			aod_number = cmd_inq.GetString(5);
			number_des_aod = cmd_inq.GetString(6);
			number_bof_aod = cmd_inq.GetString(7);
			number_eaf_aod = cmd_inq.GetString(8);
			number_high_si_aod = cmd_inq.GetString(9);
			eaf_number = cmd_inq.GetString(10);
			if_number = cmd_inq.GetString(11);
			des_number = cmd_inq.GetString(12);
			s_jk_sum = cmd_inq.GetString(13);
			s_jk_count = cmd_inq.GetString(14);
			c_jk_sum = cmd_inq.GetString(15);
			c_jk_count = cmd_inq.GetString(16);
			s_xm_sum = cmd_inq.GetString(17);
			s_xm_count = cmd_inq.GetString(18);
			c_xm_sum = cmd_inq.GetString(19);
			c_xm_count = cmd_inq.GetString(20);
		}
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
				" c_jk_sum "
				" from TMMSMNQSJ  where prod_date = '" + tap_date + "' and CLASSGROUP = '" + prod_shift_no + "' ";
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
		}
		cmd_inqnq.Close();
		//发送宝武聊天
		bcls_rec_s.Tables[0].Rows.Clear();
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["TAP_DATE"] = tap_date;
		bcls_rec_s.Tables[0].Rows[0]["PROD_SHIFT_NO"] = prod_shift_no;
		bcls_rec_s.Tables[0].Rows[0]["BOF_NUMBER"] = bof_number;
		bcls_rec_s.Tables[0].Rows[0]["DEP_NUMBER_BOF"] = dep_number_bof;
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
		bcls_rec_s.Tables[0].Rows[0]["CODE"] = "14";
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
		cmd_inq.Close();
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
