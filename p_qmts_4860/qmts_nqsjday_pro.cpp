/************************/
/*** 2024-9-19 ********/
/****   xmy **************/
/**** 南区每班数据  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_tran_json_func(EIClass* blks_in, EIClass * blks_out, CDbConnection* conn);//调用

// service入口
BM2F_ENTERACE(qmts_nqsjday_pro)
BM2_FUNCTION_EXPORT
int f_qmts_nqsjday_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstrnq = "";
	//查询前一天的时间
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetime1 = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");
	CDecimal c_receive_weight = 0;
	CDecimal s_receive_weight = 0;
	CDecimal c_receive_weight_lj = 0;
	CDecimal s_receive_weight_lj = 0;
	CString date = "";
	CString mat_no = "";
	CString tap_date = " ";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn); 
	CDbCommand cmd_inq3(conn);
	CDbCommand cmd_inqz(conn);
	CModel tmmsmnqday("TMMSMNQDAY");
	CString prod_shift = " ";
	CString prod_shift_no = " ";

	EIClass iplat_Tab;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_RECEIVE_WEIGHT");
	bcls_rec_s.Tables.Add();
	int   blkNum;


	try
	{
		tap_date = datetime1.SubstringNE(0, 8);
		Log::Trace("", "", "-----------------执行程序-------------");
		iplat_Tab.Tables.Clear();
		iplat_Tab.Tables.Add();
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "JSON");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "URL");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "COL");
		iplat_Tab.Tables[0].Rows.Add();


		Log::Trace("", "", "-----------------执行最新程序的识别-221-------------", tap_date);
		iplat_Tab.Tables[0].Rows[0]["JSON"] = "{\"tappingDate\":\"" + tap_date + "\"}";
		iplat_Tab.Tables[0].Rows[0]["URL"] = "http://10.162.72.21:18816/getSouthProductInfoDay/findSckProductInfo?tappingDate=" + tap_date + "";
		iplat_Tab.Tables[0].Rows[0]["COL"] = "data";
		Log::Trace("", "", "-----------------调用sql代理-------------");

		doFlag = f_tran_json_func(&iplat_Tab, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "LXX1[{0}]", doFlag);
		}
		//删除

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			//Log::Trace("", "", "--共[{0}]行，第[{1}]行数据---", bcls_ret->Tables[0].Rows.get_Count(), i);

			if (i > 15)
			{
				break;
			}
			tmmsmnqday["PROD_DATE"] = bcls_ret->Tables[0].Rows[i]["tapDate"].ToString().Trim();
			tmmsmnqday["BOF_CNUMBER"] = bcls_ret->Tables[0].Rows[i]["bofcnumber"].ToString().Trim();
			tmmsmnqday["SI_NUMBER"] = bcls_ret->Tables[0].Rows[i]["sinumber"].ToString().Trim();
			tmmsmnqday["FE_NUMBER"] = bcls_ret->Tables[0].Rows[i]["fenumber"].ToString().Trim();
			tmmsmnqday["BOF_SNUMBER"] = bcls_ret->Tables[0].Rows[i]["bofsnumber"].ToString().Trim();
			tmmsmnqday["MOLD_NUMBER"] = bcls_ret->Tables[0].Rows[i]["moldnumber"].ToString().Trim();
			tmmsmnqday["CL_CNUMBER"] = bcls_ret->Tables[0].Rows[i]["clcnumber"].ToString().Trim();
			tmmsmnqday["CL_SNUMBER"] = bcls_ret->Tables[0].Rows[i]["clsnumber"].ToString().Trim();
			tmmsmnqday["CCM_CNUMBER"] = bcls_ret->Tables[0].Rows[i]["ccmcnumber"].ToString().Trim();
			tmmsmnqday["CCM_SNUMBER"] = bcls_ret->Tables[0].Rows[i]["ccmsnumber"].ToString().Trim();
			tmmsmnqday["SUM_COU"] = bcls_ret->Tables[0].Rows[i]["sumcou"].ToString().Trim();
			tmmsmnqday["SUM_WT"] = bcls_ret->Tables[0].Rows[i]["sumwt"].ToString().Trim();
			tmmsmnqday["JK_COUNT"] = bcls_ret->Tables[0].Rows[i]["jkcount"].ToString().Trim();
			tmmsmnqday["JK_SUM"] = bcls_ret->Tables[0].Rows[i]["jksum"].ToString().Trim();
			tmmsmnqday["RH_NUMBER"] = bcls_ret->Tables[0].Rows[i]["rhnumber"].ToString().Trim();
			tmmsmnqday["S_JK_CAR"] = bcls_ret->Tables[0].Rows[i]["sjkcar"].ToString().Trim();
			tmmsmnqday["S_JK_COUNT"] = bcls_ret->Tables[0].Rows[i]["sjkcount"].ToString().Trim();
			tmmsmnqday["S_JK_SUM"] = bcls_ret->Tables[0].Rows[i]["sjksum"].ToString().Trim();
			tmmsmnqday["C_JK_CAR"] = bcls_ret->Tables[0].Rows[i]["cjkcar"].ToString().Trim();
			tmmsmnqday["C_JK_COUNT"] = bcls_ret->Tables[0].Rows[i]["cjkcount"].ToString().Trim();
			tmmsmnqday["C_JK_SUM"] = bcls_ret->Tables[0].Rows[i]["cjksum"].ToString().Trim();
			tmmsmnqday["REC_CREATOR"] = s.userid;
			tmmsmnqday["REC_CREATE_TIME"] = datetime;
			tmmsmnqday.Insert();
			/*for (int t = 0; t < bcls_ret->Tables[0].Columns.get_Count(); t++)
			{
			//Log::Trace("", "", "--列名：[{0}],值：[{1}]---", bcls_ret->Tables[0].Columns[t].get_ColumnName(), bcls_ret->Tables[0].Rows[i][t].ToString());

			}*/
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
